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

    return resource.id == InvalidResourceId ? static_cast<ResourceId>(query.LastRowId()) : resource.id;
}

void sqlite::SQLiteDatabase::DeleteResource(ResourceId id)
{
    auto query = m_db.CreateQuery("DELETE FROM Resources WHERE ROWID=?;");
    query.BindInt64(1, static_cast<int64_t>(id));
    query.Step();
}

void sqlite::SQLiteDatabase::Swap(ResourceId first, ResourceId second)
{
    auto readBlob = [this](ResourceId id, Data& out)
    {
        auto q = m_db.CreateQuery("SELECT Data FROM Resources WHERE ROWID=" + std::to_string(id) + ";");
        q.Step();
        q.ColumnBlob(0, out);
    };

    auto transaction = m_db.CreateTransaction();

    Data dataFirst, dataSecond;
    readBlob(first, dataFirst);
    readBlob(second, dataSecond);

    auto q1 = m_db.CreateQuery("UPDATE Resources SET Data=? WHERE ROWID=" + std::to_string(first) + ";");
    q1.BindBlob(1, dataSecond);
    q1.Step();

    auto q2 = m_db.CreateQuery("UPDATE Resources SET Data=? WHERE ROWID=" + std::to_string(second) + ";");
    q2.BindBlob(1, dataFirst);
    q2.Step();

    transaction.Commit();
}

ResourcesDefinition sqlite::SQLiteDatabase::GetResourcesDefinition()
{
    static const ResourcesDefinition s_result = [this]()
    {
        auto query = m_db.CreateQuery(
            "SELECT name, big, visible FROM ResourceDefinitions ORDER BY sort_order ASC;");

        ResourcesDefinition result;
        while (query.Step())
        {
            ResourceDefinition def;
            query.ColumnText(0, def.name);
            def.big     = query.ColumnInt(1) != 0;
            def.visible = query.ColumnInt(2) != 0;
            result.push_back(std::move(def));
        }
        return result;
    }();
    return s_result;
}

void sqlite::SQLiteDatabase::EnsureResourceDefinitionsTable()
{
    m_db.ExecQuery(
        "CREATE TABLE IF NOT EXISTS ResourceDefinitions ("
        "  id         INTEGER PRIMARY KEY,"
        "  name       TEXT NOT NULL UNIQUE,"
        "  big        INTEGER NOT NULL DEFAULT 0,"
        "  visible    INTEGER NOT NULL DEFAULT 1,"
        "  sort_order INTEGER NOT NULL DEFAULT 0"
        ");");
}

void sqlite::SQLiteDatabase::SeedDefaultResourceDefinitions()
{
    const ResourcesDefinition defaults = {
        { "Name",     false },
        { "URL",      false },
        { "Login",    false },
        { "Password", true  },
        { "Notes",    false },
    };

    static const std::string s_insertStr(
        "INSERT INTO ResourceDefinitions (name, big, visible, sort_order) "
        "VALUES (?, ?, ?, (SELECT COALESCE(MAX(sort_order), -1) + 1 FROM ResourceDefinitions));");

    for (const auto& def : defaults)
    {
        auto q = m_db.CreateQuery(s_insertStr);
        q.BindText(1, def.name);
        q.BindInt(2, def.big     ? 1 : 0);
        q.BindInt(3, def.visible ? 1 : 0);
        q.Step();
    }
}

void sqlite::SQLiteDatabase::BuildOpenedDb(const std::string& password)
{
    Data keys;
    Cryptor::GenerateRandomKeyAndIv(keys);
    m_key.Assign(Secret(keys.data(), keys.size()));
    SecureWipe(keys.data(), keys.size());

    ChangePassword("", password);

    m_db.ExecQuery("CREATE TABLE 'Resources' ('RowId' INTEGER PRIMARY KEY, 'Data' BLOB);");
    EnsureResourceDefinitionsTable();
    SeedDefaultResourceDefinitions();
}

void sqlite::SQLiteDatabase::ReadMetadata(
    Data& salt, Data& phrase, Data& phraseHmac,
    Data& encryptedKeys, Data& keysHmac)
{
    auto query = m_db.CreateQuery("SELECT count(*), salt, phrase, phrase_hmac, keys, keys_hmac FROM Metadata;");
    query.Step();

    if (query.ColumnInt(0) != 1)
    {
        throw std::runtime_error("The database has wrong scheme or corrupted");
    }

    query.ColumnBlob(1, salt);
    query.ColumnBlob(2, phrase);
    query.ColumnBlob(3, phraseHmac);
    query.ColumnBlob(4, encryptedKeys);
    query.ColumnBlob(5, keysHmac);
}

void sqlite::SQLiteDatabase::VerifyPhrase(
    const Data& phrase, const Data& phraseHmac,
    const Data& passwordKey, const SensitiveData& passwordKeyMaterial)
{
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
}

void sqlite::SQLiteDatabase::DecryptMasterKey(
    const Data& encryptedKeys, const Data& keysHmac,
    const Data& passwordKey, const SensitiveData& passwordKeyMaterial)
{
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
}

void sqlite::SQLiteDatabase::InitOpenedDb(const std::string& password)
{
    Data salt, phrase, phraseHmac, encryptedKeys, keysHmac;
    ReadMetadata(salt, phrase, phraseHmac, encryptedKeys, keysHmac);

    Data passwordKey = DerivePasswordKey(password, salt);
    SensitiveData passwordKeyMaterial;
    passwordKeyMaterial.Assign(Secret(passwordKey.data(), passwordKey.size()));

    VerifyPhrase(phrase, phraseHmac, passwordKey, passwordKeyMaterial);
    DecryptMasterKey(encryptedKeys, keysHmac, passwordKey, passwordKeyMaterial);

    SecureWipe(passwordKey.data(), passwordKey.size());
    SecureWipe(encryptedKeys.data(), encryptedKeys.size());
}
