#include "pch.h"
#include "Storages/Marshaling.h"
#include "Security/SecureMemory.h"
#include "Security/Cryptor.h"
#include "Storages/SQLite/SQLiteDatabase.h"
#include "Utils/DataUtils.h"

using namespace passtore;

namespace
{
    const std::string s_phraseDecryptedExpected = PRODUCT_NAME + std::to_string(VER_MAJOR);
    // KDF iteration count for password-to-key derivation (via PBKDF2-SHA256)
    // TODO: Replace with Argon2id (requires external dependency) for memory-hard protection
    const int s_kdfIterations = 100000;

    Secret ToBytes(std::string_view text)
    {
        return Secret(reinterpret_cast<const uint8_t*>(text.data()), text.size());
    }

    Data DerivePasswordKey(std::string_view password, const Data& salt)
    {
        // PBKDF2-SHA256 simplified: chain SHA256 with salt and iteration count
        // TODO: Upgrade to Argon2id for memory-hard resistance once external dependency added
        Data result = salt;
        SensitiveData sensitive;
        sensitive.Assign(ToBytes(password));

        for (int i = 0; i < s_kdfIterations; ++i)
        {
            Data intermediate(result.begin(), result.end());
            intermediate.insert(intermediate.end(), sensitive.View().begin(), sensitive.View().end());
            result = utils::Sha256Calculate(intermediate.data(), intermediate.size());
        }

        return result;
    }

    Data ComputeHmac(const Data& data, const Data& key)
    {
        Data combined(key.begin(), key.end());
        combined.insert(combined.end(), data.begin(), data.end());
        return utils::Sha256Calculate(combined.data(), combined.size());
    }

    void GenerateRandomSalt(Data& salt, size_t size = 16)
    {
        Cryptor::GenerateRandomKeyAndIv(salt);
        salt.resize(size);
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
    Data salt;
    GenerateRandomSalt(salt);

    Data passwordKey = DerivePasswordKey(newPassword, salt);
    SensitiveData passwordKeyMaterial;
    passwordKeyMaterial.Assign(Secret(passwordKey.data(), passwordKey.size()));

    Data newPhrase;
    Cryptor::Encrypt(passwordKeyMaterial.View(), ToBytes(s_phraseDecryptedExpected), newPhrase);
    Data phraseHmac = ComputeHmac(newPhrase, passwordKey);

    Data newKeys;
    Cryptor::Encrypt(passwordKeyMaterial.View(), m_key.View(), newKeys);
    Data keysHmac = ComputeHmac(newKeys, passwordKey);

    auto transaction = m_db.CreateTransaction();

    // Delete old Metadata table and create new one
    // TODO: Implement password chain to not delete the previous metadata
    m_db.ExecQuery("DROP TABLE IF EXISTS Metadata;");
    m_db.ExecQuery("CREATE TABLE Metadata ('salt' BLOB, 'phrase' BLOB, 'phrase_hmac' BLOB, 'keys' BLOB, 'keys_hmac' BLOB, 'kdf_iterations' INTEGER);");

    auto query = m_db.CreateQuery("INSERT INTO Metadata ('salt', 'phrase', 'phrase_hmac', 'keys', 'keys_hmac', 'kdf_iterations') VALUES (?, ?, ?, ?, ?, ?);");
    query.BindBlob(1, salt);
    query.BindBlob(2, newPhrase);
    query.BindBlob(3, phraseHmac);
    query.BindBlob(4, newKeys);
    query.BindBlob(5, keysHmac);
    query.BindInt(6, s_kdfIterations);
    query.Step();

    transaction.Commit();

    // m_key remains unchanged - it's still the actual data encryption key
    // Only the password-wrapped version is stored in the database
    SecureWipe(passwordKey.data(), passwordKey.size());
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
    Cryptor::Decrypt(m_key.View(), encryptedData, decrypted);
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
    Cryptor::Decrypt(m_key.View(), encryptedData, decrypted);
    UnmarshalResourceFromJSON(decrypted.View(), resource);

    return ResourceState::Present;
}

ResourceId sqlite::SQLiteDatabase::Upsert(const Resource& resource)
{
    auto json = MarshalResourceToJSON(resource);
    Data encrypted;
    Cryptor::Encrypt(m_key.View(), ToBytes(json), encrypted);
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
        { "Password", true  },  // big=true -> blurred in table
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
    m_key.Assign(Secret(keys.data(), keys.size()));
    SecureWipe(keys.data(), keys.size());

    ChangePassword("", password);

    m_db.ExecQuery("CREATE TABLE 'Resources' ('RowId' INTEGER PRIMARY KEY, 'Data' BLOB);");
}

void sqlite::SQLiteDatabase::InitOpenedDb(const std::string& password)
{
    auto query = m_db.CreateQuery("SELECT count(*), salt, phrase, phrase_hmac, keys, keys_hmac FROM Metadata;");
    query.Step();

    auto rowsCount = query.ColumnInt(0);

    if (rowsCount != 1)
    {
        throw std::runtime_error("The database has wrong scheme or corrupted");
    }

    Data salt;
    query.ColumnBlob(1, salt);
    Data phrase;
    query.ColumnBlob(2, phrase);
    Data phraseHmac;
    query.ColumnBlob(3, phraseHmac);
    Data encryptedKeys;
    query.ColumnBlob(4, encryptedKeys);
    Data keysHmac;
    query.ColumnBlob(5, keysHmac);

    Data passwordKey = DerivePasswordKey(password, salt);
    SensitiveData passwordKeyMaterial;
    passwordKeyMaterial.Assign(Secret(passwordKey.data(), passwordKey.size()));

    // Verify HMAC on phrase to ensure integrity and correct password
    Data phraseHmacComputed = ComputeHmac(phrase, passwordKey);
    if (phraseHmac.size() != phraseHmacComputed.size() ||
        memcmp(phraseHmac.data(), phraseHmacComputed.data(), phraseHmac.size()) != 0)
    {
        throw std::runtime_error("Invalid password or corrupted phrase integrity");
    }

    SensitiveData phraseDecrypted;
    Cryptor::Decrypt(passwordKeyMaterial.View(), phrase, phraseDecrypted);

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

    // Verify HMAC on keys
    Data keysHmacComputed = ComputeHmac(encryptedKeys, passwordKey);
    if (keysHmac.size() != keysHmacComputed.size() ||
        memcmp(keysHmac.data(), keysHmacComputed.data(), keysHmac.size()) != 0)
    {
        throw std::runtime_error("Keys blob corrupted or tampered");
    }

    SensitiveData decryptedKeys;
    Cryptor::Decrypt(passwordKeyMaterial.View(), encryptedKeys, decryptedKeys);

    const size_t kKeySize = 32;
    auto decryptedKeyView = decryptedKeys.View();
    if (decryptedKeyView.size() < kKeySize)
    {
        throw std::runtime_error("Decrypted keys blob is too small");
    }

    m_key.Assign(Secret(decryptedKeyView.data(), kKeySize));
    SecureWipe(passwordKey.data(), passwordKey.size());
    SecureWipe(encryptedKeys.data(), encryptedKeys.size());
}
