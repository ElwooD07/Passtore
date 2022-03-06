#pragma once
#include <QString>
#include <QSqlDatabase>
#include "Cryptor.h"
#include "Resource.h"

struct ResourceDefinition
{
    int id;
    QString name; // Corresponds to ResourcePropertyName
};
using ResourceDefinitions = QVector<ResourceDefinition>;

class Database
{
public:
    Database();
    Database(const Database&) = delete;

    void Open(const QString& path, const QString& password);
    void ChangePassword(const QString& newPassword);

    ResourceDefinitions GetResourceDefinitions();
    Resource GetResource(int id);
    QString GetResourcePropertyValue(int id, ResourceProperty prop);

    void SetResource(int id, const Resource& resource);
    void SetResourcePropertyValue(int id, ResourceProperty prop, const QString& value);
    int CreateResource(); // Returns RowId of created record
    int AddResource(const Resource& resource); // Returns RowId of created record

private:
    void CreateNewConnection(const QString& path, const QString& password);

private:
    QSqlDatabase m_db;
    Cryptor m_cryptor;
};
