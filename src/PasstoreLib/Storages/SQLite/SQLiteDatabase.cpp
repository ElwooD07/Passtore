#include "pch.h"
#include "Storages/Marshaling.h"
#include "Storages/SQLite/SQLiteDatabase.h"
#include "Utils/DataUtils.h"

using namespace passtore;

namespace
{
    const std::string s_phraseDecryptedExpected = PRODUCT_NAME + std::to_string(VER_MAJOR);
}

sqlite::SQLiteDatabase::SQLiteDatabase()
{ }

void sqlite::SQLiteDatabase::Open(const std::filesystem::path& path, const std::string& password)
{
    auto buildNewDb = !std::filesystem::exists(path);
    m_db.Reconnect(path);

    if (buildNewDb)
    {
        BuildOpenedDb(password);
    }
    else
    {
        InitOpenedDb(password);
    }

    // Uncomment it for tests when the database is empty
    Resource res;
    res.id = InvalidResourceId;
    res.subject = "test";
    int id = Upsert(res);
    qDebug() << id;

    // TODO : Collect data from DB
}

void sqlite::SQLiteDatabase::ChangePassword(const std::string& oldPassword, const std::string& newPassword)
{
    // TODO: read phrase and check old password
    Data passwordData(newPassword.begin(), newPassword.end());
    Cryptor passwordCryptor(utils::Sha256Calculate(passwordData.data(), passwordData.size()));

    Data newPhrase;
    passwordCryptor.Encrypt(s_phraseDecryptedExpected, newPhrase);
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

    m_cryptor.SetKeyAndIv(newKeys);
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
    UnmarshalResourceFromJSON(m_cryptor.DecryptAsStdString(encryptedData), resource);

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
    UnmarshalResourceFromJSON(m_cryptor.DecryptAsStdString(encryptedData), resource);

    return ResourceState::Present;
}

ResourceId sqlite::SQLiteDatabase::Upsert(const Resource& resource)
{
    auto json = MarshalResourceToJSON(resource);
    Data encrypted;
    m_cryptor.Encrypt(json, encrypted);

    auto query = m_db.CreateQuery();
    if (resource.id == InvalidResourceId)
    {
        static std::string s_queryStr("INSERT INTO Resources (Data) VALUES (?) WHERE RowId=");
        query.Prepare(s_queryStr + std::to_string(resource.id));
    }
    else
    {
        static std::string s_queryStr("UPDATE 'Resources' SET 'Data'=? WHERE ROWID='");
        query.Prepare(s_queryStr + std::to_string(resource.id));
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

void sqlite::SQLiteDatabase::BuildOpenedDb(const std::string& password)
{
    Data keys;
    Cryptor::GenerateRandomKeyAndIv(keys);
    m_cryptor.SetKeyAndIv(std::move(keys));

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
        throw std::runtime_error(QObject::tr("The database has wrong scheme or corrupted").toStdString());
    }

    BlobData phrase;
    query.ColumnBlob(1, phrase);
    BlobData encryptedKeys;
    query.ColumnBlob(2, encryptedKeys);

    Cryptor passwordCryptor(utils::Sha256Calculate(password.data(), password.size()));
    std::string phraseDecrypted = passwordCryptor.DecryptAsStdString(phrase);
    if (phraseDecrypted != s_phraseDecryptedExpected)
    {
        throw std::runtime_error("Invalid password");
    }
    m_cryptor.SetKeyAndIv(std::move(encryptedKeys));
}
