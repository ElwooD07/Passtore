#pragma once
#include "Security/SensitiveData.h"
#include "Storages/IResourceStorage.h"
#include "Storages/SQLite/Connection.h"

namespace passtore
{
    namespace sqlite
    {
        /* Concrete IResourceStorage backed by an encrypted SQLite file. */
        class SQLiteDatabase: public IResourceStorage
        {
        public:
            SQLiteDatabase();
            SQLiteDatabase(const SQLiteDatabase&) = delete;

            void Open(const std::filesystem::path& path, const std::string& password) override;
            void ChangePassword(const std::string& oldPassword, const std::string& newPassword) override;

            ResourceState GetOne(ResourceId id, Resource& resource) override;
            ResourceState GetNext(ResourceId id, Resource& resource) override;
            ResourceId Upsert(const Resource& resource) override;
            void DeleteResource(ResourceId id) override;
            void Swap(ResourceId first, ResourceId second) override;
            ResourcesDefinition GetResourcesDefinition() override;

        private:
            void BuildOpenedDb(const std::string& password);
            void InitOpenedDb(const std::string& password);
            void ReadMetadata(Data& salt, Data& phrase, Data& phraseHmac, Data& encryptedKeys, Data& keysHmac);
            void VerifyPhrase(const Data& phrase, const Data& phraseHmac, const Data& passwordKey, const SensitiveData& passwordKeyMaterial);
            void DecryptMasterKey(const Data& encryptedKeys, const Data& keysHmac, const Data& passwordKey, const SensitiveData& passwordKeyMaterial);
            void EnsureResourceDefinitionsTable();
            void SeedDefaultResourceDefinitions();

        private:
            Connection m_db;
            SensitiveData m_key;
        };
    }
}
