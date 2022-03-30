#pragma once
#include <QString>
#include <QSqlDatabase>
#include "Storages/IStorage.h"
#include "Cryptor.h"
#include "Storages/SQLite/IndexConverter.h"

namespace passtore
{
    // TODO: move to plugin
    class SQLiteDatabase: public IStorage
    {
    public:
        SQLiteDatabase();
        SQLiteDatabase(const SQLiteDatabase&) = delete;

        virtual void Open(const QString& path, const QString& password) override;
        virtual void ChangePassword(const QString& newPassword) override;

        virtual int GetResourcesCount() override;
        virtual void GetResourcesDefinition(ResourcesDefinition& definition) override;

        virtual void GetResource(int id, Resource& resource) override;
        virtual void GetResources(int from, int to, QVector<Resource>& resources) override;
        virtual void SetResource(int id, const Resource& resource) override;
        virtual int AddResource(const Resource& resource) override; // Returns RowId-1 of created record
        virtual void SwapResources(int first, int second) override;

    private:
        void CreateNewConnection(const QString& path, const QString& password);

    private:
        QSqlDatabase m_db;
        Cryptor m_cryptor;
        ResourcesDefinition m_defs;
        IndexConverter m_idx;
    };
}
