#include "pch.h"
#include "Storages/Marshaling.h"
#include "Security/SecureMemory.h"
#include "Storages/SQLite/SQLiteDatabase.h"
#include "Utils/DataUtils.h"

using namespace passtore;

namespace
{
    const std::string s_phraseDecryptedExpected = PRODUCT_NAME + std::to_string(VER_MAJOR);

    Secret ToBytes(std::string_view text)
    {
        return Secret(reinterpret_cast<const uint8_t*>(text.data()), text.size());
    }
}

sqlite::SQLiteDatabase::SQLiteDatabase()
{ }

void sqlite::SQLiteDatabase::Open(const std::filesystem::path& path, const std::string& password)
{
    auto buildNewDb = !std::filesystem::exists(path);
    m_db.Reconnect(path.string());

    if (buildNewDb)
    {
        BuildOpenedDb(password);
    }
    else
    {
        InitOpenedDb(password);
    }

    // TODO : Collect data from DB
}

void sqlite::SQLiteDatabase::ChangePassword(const std::string& oldPassword, const std::string& newPassword)
{
    // TODO: read phrase and check old password
    Data passwordData(newPassword.begin(), newPassword.end());
    Data passwordHash = utils::Sha256Calculate(passwordData.data(), passwordData.size());
    Cryptor passwordCryptor(Secret(passwordHash.data(), passwordHash.size()));

    Data newPhrase;
    passwordCryptor.Encrypt(ToBytes(s_phraseDecryptedExpected), newPhrase);
    Data newKeys;
    passwordCryptor.Encrypt(m_cryptor.GetKeyAndIv(), newKeys);

    auto transaction = m_db.CreateTransaction();

    // Delete old Metadata table and create new one
    // TODO: Implement password chain to not delete the previous metadata
    m_db.ExecQuery("DROP TABLE IF EXISTS Metadata;");
    m_db.ExecQuery("CREATE TABLE Metadata ('phrase' BLOB, 'keys' BLOB);");

    auto query = m_db.CreateQuery("INSERT INTO Metadata ('phrase', 'keys') VALUES (?, ?);");
    query.BindBlob(1, newPhrase);
    query.BindBlob(2, newKeys);
    query.Step();

    transaction.Commit();

    m_cryptor.SetKeyAndIv(Secret(newKeys.data(), newKeys.size()));
    SecureWipe(passwordData.data(), passwordData.size());
    SecureWipe(passwordHash.data(), passwordHash.size());
    SecureWipe(newKeys.data(), newKeys.size());
}

ResourceState sqlite::SQLiteDatabase::GetOne(ResourceId id, Resource& resource)
{
    static std::string s_queryStr("SELECT Data FROM Resources WHERE ROWID=");
    auto queryStr = s_queryStr + std::to_string(id);
    auto query = m_db.CreateQuery(queryStr);

    if (!query.Step())
    {
        return ResourceState::Deleted;
    }

    resource.id = id;
    Data encryptedData;
    query.ColumnBlob(0, encryptedData);
    SensitiveData decrypted;
    m_cryptor.Decrypt(encryptedData, decrypted);
    UnmarshalResourceFromJSON(decrypted.View(), resource);

    return ResourceState::Present;
}

ResourceState sqlite::SQLiteDatabase::GetNext(ResourceId id, Resource& resource)
{
    std::string queryStr("SELECT RowId, Data FROM Resources WHERE ROWID>");
    if (id == InvalidResourceId)
    {
        id = 0;
    }
    queryStr += std::to_string(id);
    queryStr += " LIMIT 1;";
    auto query = m_db.CreateQuery(queryStr);

    if (!query.Step())
    {
        return ResourceState::Deleted;
    }

    resource.id = query.ColumnInt(0);
    Data encryptedData;
    query.ColumnBlob(1, encryptedData);
    SensitiveData decrypted;
    m_cryptor.Decrypt(encryptedData, decrypted);
    UnmarshalResourceFromJSON(decrypted.View(), resource);

    return ResourceState::Present;
}

ResourceId sqlite::SQLiteDatabase::Upsert(const Resource& resource)
{
    auto json = MarshalResourceToJSON(resource);
    Data encrypted;
    m_cryptor.Encrypt(ToBytes(json), encrypted);
    SecureWipe(json.data(), json.size());

    auto query = m_db.CreateQuery();
    if (resource.id == InvalidResourceId)
    {
        static const std::string s_queryStr("INSERT INTO Resources (Data) VALUES (?);");
        query.Prepare(s_queryStr);
    }
    else
    {
        static const std::string s_queryStr("UPDATE Resources SET Data=? WHERE ROWID=");
        query.Prepare(s_queryStr + std::to_string(resource.id) + ";");
    }

    query.BindBlob(1, encrypted);
    try
    {
        query.Step();
    }
    catch (const std::exception& ex)
    {
        LOGE << "UpsertResource failed: " << ex.what();
        return InvalidResourceId;
    }

    return resource.id == InvalidResourceId ? query.LastRowId() : resource.id;
}

void sqlite::SQLiteDatabase::DeleteResource(ResourceId id)
{
    // TODO
}

void sqlite::SQLiteDatabase::Swap(ResourceId first, ResourceId second)
{
    // TODO
}

ResourcesDefinition sqlite::SQLiteDatabase::GetResourcesDefinition()
{
    // TODO: persist and load resource definitions from database
    return {
        { "Name",     false },
        { "URL",      false },
        { "Login",    false },
        { "Password", true  },  // big=true → blurred in table
        { "Notes",    false },
    };
}

ResourceId sqlite::SQLiteDatabase::GetResourcesCount()
{
    auto query = m_db.CreateQuery("SELECT count(*) FROM Resources;");
    query.Step();
    return static_cast<ResourceId>(query.ColumnInt(0));
}

void sqlite::SQLiteDatabase::BuildOpenedDb(const std::string& password)
{
    Data keys;
    Cryptor::GenerateRandomKeyAndIv(keys);
    m_cryptor.SetKeyAndIv(Secret(keys.data(), keys.size()));
    SecureWipe(keys.data(), keys.size());

    ChangePassword("", password);

    m_db.ExecQuery("CREATE TABLE 'Resources' ('RowId' INTEGER PRIMARY KEY, 'Data' BLOB);");
}

void sqlite::SQLiteDatabase::InitOpenedDb(const std::string& password)
{
    auto query = m_db.CreateQuery("SELECT count(*), phrase, keys FROM Metadata;");
    query.Step();

    auto rowsCount = query.ColumnInt(0);

    if (rowsCount != 1)
    {
        throw std::runtime_error("The database has wrong scheme or corrupted");
    }

    Data phrase;
    query.ColumnBlob(1, phrase);
    Data encryptedKeys;
    query.ColumnBlob(2, encryptedKeys);

    Data passwordHash = utils::Sha256Calculate(password.data(), password.size());
    Cryptor passwordCryptor(Secret(passwordHash.data(), passwordHash.size()));
    SensitiveData phraseDecrypted;
    passwordCryptor.Decrypt(phrase, phraseDecrypted);

    auto expected = ToBytes(s_phraseDecryptedExpected);
    auto decrypted = phraseDecrypted.View();
    size_t decryptedLen = 0;
    while (decryptedLen < decrypted.size() && decrypted[decryptedLen] != '\0')
    {
        ++decryptedLen;
    }

    if (decryptedLen != expected.size() || memcmp(decrypted.data(), expected.data(), expected.size()) != 0)
    {
        throw std::runtime_error("Invalid password");
    }

    m_cryptor.SetKeyAndIv(Secret(encryptedKeys.data(), encryptedKeys.size()));
    SecureWipe(passwordHash.data(), passwordHash.size());
    SecureWipe(encryptedKeys.data(), encryptedKeys.size());
}
