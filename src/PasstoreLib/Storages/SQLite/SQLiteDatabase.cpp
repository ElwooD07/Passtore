#include "pch.h"
#include "Storages/SQLite/SQLiteColumns.h"
#include "Storages/SQLite/SQLiteDatabase.h"
#include "Storages/SQLite/SQLiteDatabaseQueries.h"
#include "Utils/DataUtils.h"

using namespace passtore;

namespace
{
    const std::string s_phraseDecryptedExpected = PRODUCT_NAME + std::to_string(VER_MAJOR);
}

sqlite::SQLiteDatabase::SQLiteDatabase()
    : m_idx(m_db)
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
    res.data.push_back("ResName");
    res.data.push_back("arbadakarba");
    res.data.push_back("bla-bla-bla");
    int id = AddResource(res);
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

int sqlite::SQLiteDatabase::GetResourcesCount()
{
    return static_cast<int>(m_idx.Count());
}

void sqlite::SQLiteDatabase::GetResourcesDefinition(ResourcesDefinition& defs)
{
    if (m_defs.isEmpty())
    {
        for (size_t col = ColumnFirst; col < ColumnCount; ++col)
        {
            m_defs.emplace_back(ResourceValueDefinition{ QObject::tr(GetColumnName(static_cast<Column>(col))).toStdString() });
        }
        m_defs[ColumnPassword].critical = true;
        m_defs[ColumnDescription].big = true;
        m_defs[ColumnAdditional].big = true;
    }
    defs = m_defs;
}

void sqlite::SQLiteDatabase::GetResource(int id, Resource& resource)
{
    auto rowId = static_cast<int>(m_idx.ToRowId(id));
    if (rowId == IndexConverter::InvalidId)
    {
        throw std::runtime_error(QObject::tr("Cannot match id %1 with ROWID").arg(id).toStdString());
    }

    auto query = m_db.CreateQuery(MakeResourceSelectQuery(rowId));
    query.Step();

    Resource result;
    resource.data.resize(ColumnCount);
    for (int i = ColumnName; i < ColumnCount; ++i)
    {
        BlobData data;
        query.ColumnBlob(i, data);
        resource.data[i] = m_cryptor.DecryptAsStdString(data);
    }
}

void sqlite::SQLiteDatabase::GetResources(int from, int to, std::vector<Resource>& resources)
{
    auto fromRowid = m_idx.ToRowId(from);
    auto toRowId = m_idx.ToRowId(to);

    if (fromRowid == IndexConverter::InvalidId || toRowId == IndexConverter::InvalidId)
    {
        throw std::runtime_error(QObject::tr("Unable to match IDs %1 and %2 to ROWIDs").arg(from).arg(to).toStdString());
    }

    auto query = m_db.CreateQuery(MakeResourcesSelectQuery(fromRowid, toRowId));
    while (query.Step())
    {
        Resource result;
        for (int i = ColumnName; i < ColumnCount; ++i)
        {
            Data data;
            query.ColumnBlob(i, data);
            result.data[i] = m_cryptor.DecryptAsStdString(data);
        }
        resources.emplace_back(std::move(result));
    };
}

void sqlite::SQLiteDatabase::SetResource(int id, const Resource& resource)
{
    auto query = m_db.CreateQuery(MakeResourceUpdateQuery(m_idx.ToRowId(id)));
    for (int i = ColumnName; i < ColumnCount; ++i)
    {
        Data data;
        m_cryptor.Encrypt(resource.data.at(i), data);
        query.BindBlob(i + 1, data);
    }
    query.Step();
}

void sqlite::SQLiteDatabase::SetResourceValue(int id, int valueId, const QString& value)
{
    auto query = m_db.CreateQuery(MakeResourceValueUpdateQuery(m_idx.ToRowId(id), valueId));
    Data data;
    m_cryptor.Encrypt(value, data);
    query.BindBlob(1, data);
    query.Step();
}

uint64_t sqlite::SQLiteDatabase::AddResource(const Resource& resource)
{
    auto query = m_db.CreateQuery(MakeResourceInsertQueryValues());
    for (int i = ColumnName; i < ColumnCount; ++i)
    {
        Data data;
        if (resource.data.size() > static_cast<Column>(i))
        {
            m_cryptor.Encrypt(resource.data.at(static_cast<Column>(i)), data);
        }
        query.BindBlob(i + 1, data);
    }

    query.Step();
    return query.LastRowId();
}

void sqlite::SQLiteDatabase::SwapResources(int first, int second)
{
    // TODO
}

void sqlite::SQLiteDatabase::BuildOpenedDb(const std::string& password)
{
    Data keys;
    Cryptor::GenerateRandomKeyAndIv(keys);
    m_cryptor.SetKeyAndIv(std::move(keys));

    ChangePassword("", password);

    m_db.ExecQuery(MakeResourcesTableCreateQuery());
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
    if (phraseDecrypted == s_phraseDecryptedExpected)
    {
        throw std::runtime_error("Invalid password");
    }
    m_cryptor.SetKeyAndIv(std::move(encryptedKeys));
}
