#pragma once
#include "Cryptor.h"
#include "Storages/IStorage.h"
#include "Storages/SQLite/IndexConverter.h"
#include "Storages/SQLite/Connection.h"

namespace passtore
{
    namespace sqlite
    {
        // TODO: move to plugin
        class SQLiteDatabase: public IStorage
        {
        public:
            SQLiteDatabase();
            SQLiteDatabase(const SQLiteDatabase&) = delete;

            virtual void Open(const std::filesystem::path& path, const std::string& password) override;
            virtual void ChangePassword(const std::string& oldPassword, const std::string& newPassword) override;

            virtual int GetResourcesCount() override;
            virtual void GetResourcesDefinition(ResourcesDefinition& definition) override;

            virtual void GetResource(int id, Resource& resource) override;
            virtual void GetResources(int from, int to, std::vector<Resource>& resources) override;
            virtual void SetResource(int id, const Resource& resource) override;
            virtual void SetResourceValue(int id, int valueId, const QString& value) override;
            virtual uint64_t AddResource(const Resource& resource) override;
            virtual void SwapResources(int first, int second) override;

        private:
            void BuildOpenedDb(const std::string& password);
            void InitOpenedDb(const std::string& password);

        private:
            Connection m_db;
            Cryptor m_cryptor;
            ResourcesDefinition m_defs;
            IndexConverter m_idx;
        };
    }
}
