#include "pch.h"
#include "Storages/SQLite/SQLiteDatabaseQueries.h"
#include "Storages/SQLite/SQLiteColumns.h"

using namespace passtore;

const QString& passtore::MakeResourcesTableCreateQuery()
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "CREATE TABLE 'Resources' (";
        for (int i = ColumnFirst; i < ColumnCount; ++i)
        {
            s_query += "'" + QString(GetColumnName(static_cast<Column>(i))) + "' BLOB,";
        }
        s_query.chop(1);
        s_query += ");";
    }

    return s_query;
}

const QString& MakeResourcesCountQuery()
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "SELECT COUNT(*) FROM Resources;";
    }

    return s_query;
}

QString MakeResourceSelectQuery(int rowId)
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "SELECT ";
        for (int i = ColumnFirst; i < ColumnCount; ++i)
        {
            s_query += QString(GetColumnName(static_cast<Column>(i))) + ", ";
        }
        s_query.chop(2);
        s_query += "FROM Resources WHERE ROWID=%1;";
    }

    QString newQuery(s_query);
    return newQuery.arg(rowId);
}

QString MakeResourceSelectPropertyQuery(int rowId, Column col)
{
    QString query = "SELECT " + QString(GetColumnName(col)) + " FROM Resources WHERE ROWID=%1;";
    return query.arg(rowId);
}

QString MakeResourceUpdateQuery(int rowId)
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "UPDATE 'Resources' SET ";
        for (int i = ColumnFirst; i < ColumnCount; ++i)
        {
            s_query += "'" + QString(GetColumnName(static_cast<Column>(i))) + "'=?, ";
        }
        s_query.chop(2);
        s_query += ") WHERE ROWID=%1;";
    }

    QString newQuery(s_query);
    return newQuery.arg(rowId);
}

QString MakeResourcePropertyUpdateQuery(int rowId, Column col)
{
    QString query("UPDATE 'Resources' SET '");
    query += GetColumnName(col);
    query += "'=? WHERE ROWID=%1";
    return query.arg(rowId);
}

const QString& MakeResourceInsertQuery()
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "INSERT INTO Resources DEFAULT VALUES;";
    }
    return s_query;
}

const QString& MakeResourceInsertQueryValues()
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "INSERT INTO Resources (";
        for (int i = ColumnFirst; i < ColumnCount; ++i)
        {
            s_query += "'" + QString(GetColumnName(static_cast<Column>(i))) + "', ";
        }
        s_query.chop(2);
        s_query += ") VALUES (" + QString("?, ").repeated(ColumnCount);
        s_query.chop(2);
        s_query += ");";
    }
    return s_query;
}
