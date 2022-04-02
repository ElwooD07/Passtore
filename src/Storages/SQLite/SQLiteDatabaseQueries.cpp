#include "pch.h"
#include "Storages/SQLite/SQLiteDatabaseQueries.h"
#include "Storages/SQLite/SQLiteColumns.h"

namespace
{
    const QString& getCommaSeparatedColumns()
    {
        static QString s_result;
        if (s_result.isEmpty())
        {
            for (int i = passtore::ColumnFirst; i < passtore::ColumnCount; ++i)
            {
                s_result += QString(GetColumnName(static_cast<passtore::Column>(i))) + ", ";
            }
            s_result.chop(2);
        }

        return s_result;
    }
}

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

const QString& passtore::MakeResourcesCountQuery()
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "SELECT COUNT(*) FROM Resources;";
    }

    return s_query;
}

QString passtore::MakeResourceSelectQuery(int rowId)
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "SELECT ";
        s_query += getCommaSeparatedColumns();
        s_query += " FROM Resources WHERE ROWID=%1;";
    }

    QString newQuery(s_query);
    return newQuery.arg(rowId);
}

QString passtore::MakeResourcesSelectQuery(int fromRowId, int toRowId)
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "SELECT ";
        s_query += getCommaSeparatedColumns();
        s_query += "FROM Resources WHERE ROWID >= %1 AND ROWID <= %2;";
    }

    QString newQuery(s_query);
    return newQuery.arg(fromRowId).arg(toRowId);
}

QString passtore::MakeResourceSelectPropertyQuery(int rowId, Column col)
{
    QString query = "SELECT " + QString(GetColumnName(col)) + " FROM Resources WHERE ROWID=%1;";
    return query.arg(rowId);
}

QString passtore::MakeResourceUpdateQuery(int rowId)
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "UPDATE 'Resources' SET ";
        for (int i = ColumnFirst; i < ColumnCount; ++i)
        {
            auto col = static_cast<Column>(i);
            s_query += QString("'%1'=%2, ").arg(GetColumnName(col)).arg(GetColumnPlaceholder(col));
        }
        s_query.chop(2);
        s_query += " WHERE ROWID=%1;";
    }

    QString newQuery(s_query);
    return newQuery.arg(rowId);
}

QString passtore::MakeResourcePropertyUpdateQuery(int rowId, Column col)
{
    QString query("UPDATE 'Resources' SET '");
    query += GetColumnName(col);
    query += "'=? WHERE ROWID=%1";
    return query.arg(rowId);
}

const QString& passtore::MakeResourceInsertQuery()
{
    static QString s_query;
    if (s_query.isEmpty())
    {
        s_query = "INSERT INTO Resources DEFAULT VALUES;";
    }
    return s_query;
}

const QString& passtore::MakeResourceInsertQueryValues()
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
