#pragma once
#include "Security/SensitiveData.h"
#include "Storages/IResourceStorage.h"
#include "Storages/SQLite/Connection.h"

namespace passtore
{
    namespace sqlite
    {
        // TODO: move to plugin
        class SQLiteDatabase: public IResourceStorage
        {
        public:
            SQLiteDatabase();
            SQLiteDatabase(const SQLiteDatabase&) = delete;

            virtual void Open(const std::filesystem::path& path, const std::string& password) override;
            virtual void ChangePassword(const std::string& oldPassword, const std::string& newPassword) override;

            virtual ResourceState GetOne(ResourceId id, Resource& resource) override;
            virtual ResourceState GetNext(ResourceId id, Resource& resource) override;
            virtual ResourceId Upsert(const Resource& resource) override;
            virtual void DeleteResource(ResourceId id) override;
            virtual void Swap(ResourceId first, ResourceId second) override;
            virtual ResourcesDefinition GetResourcesDefinition() override;

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
