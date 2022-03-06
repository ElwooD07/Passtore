#include "pch.h"
#include "DatabaseQueries.h"
#include "Resource.h"

const QString& MakeResourcesTableCreateQuery()
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "CREATE TABLE 'Resources' (";
        for (int i = ResourcePropertyName; i < ResourcePropertyCount; ++i)
        {
            s_query += "'" + Resource::PropertyName(static_cast<ResourceProperty>(i)) + "' BLOB,";
        }
        s_query.chop(1);
        s_query += ");";
    }

    return s_query;
}

QString MakeResourceSelectQuery(int rowId)
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "SELECT ";
        for (int i = ResourcePropertyName; i < ResourcePropertyCount; ++i)
        {
            s_query += Resource::PropertyName(static_cast<ResourceProperty>(i)) + ", ";
        }
        s_query.chop(2);
        s_query += "FROM Resources WHERE ROWID=%1;";
    }

    QString newQuery(s_query);
    return newQuery.arg(rowId);
}

QString MakeResourceSelectPropertyQuery(int rowId, ResourceProperty prop)
{
    QString query = "SELECT " + Resource::PropertyName(prop) + " FROM Resources WHERE ROWID=%1;";
    return query.arg(rowId);
}

QString MakeResourceUpdateQuery(int rowId)
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "UPDATE 'Resources' SET ";
        for (int i = ResourcePropertyName; i < ResourcePropertyCount; ++i)
        {
            s_query += "'" + Resource::PropertyName(static_cast<ResourceProperty>(i)) + "'=?, ";
        }
        s_query.chop(2);
        s_query += ") WHERE ROWID=%1;";
    }

    QString newQuery(s_query);
    return newQuery.arg(rowId);
}

QString MakeResourcePropertyUpdateQuery(int rowId, ResourceProperty property)
{
    QString query("UPDATE 'Resources' SET '");
    query += Resource::PropertyName(property);
    query += "'=? WHERE ROWID=%1";
    return query.arg(rowId);
}

const QString& MakeResourceSelectDefinitionsQuery()
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "SELECT ROWID, ";
        s_query += Resource::PropertyName(ResourcePropertyName);
        s_query += " FROM Resources;";
    }
    return s_query;
}

QString MakeResourceInsertQuery()
{
    QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "INSERT INTO Resources DEFAULT VALUES;";
    }
    return s_query;
}

QString MakeResourceInsertQueryValues()
{
    QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "INSERT INTO Resources (";
        for (int i = ResourcePropertyName; i < ResourcePropertyCount; ++i)
        {
            s_query += "'" + Resource::PropertyName(static_cast<ResourceProperty>(i)) + "', ";
        }
        s_query.chop(2);
        s_query += ") VALUES (" + QString("?, ").repeated(ResourcePropertyCount);
        s_query.chop(2);
        s_query += ");";
    }
    return s_query;
}
