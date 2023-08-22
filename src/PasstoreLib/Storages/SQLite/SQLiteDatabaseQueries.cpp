#include "pch.h"
#include "Storages/SQLite/SQLiteDatabaseQueries.h"
#include "Storages/SQLite/SQLiteColumns.h"

namespace
{
    const std::string& getCommaSeparatedColumns()
    {
        static std::string s_result;
        if (s_result.empty())
        {
            for (int i = passtore::ColumnFirst; i < passtore::ColumnCount; ++i)
            {
                s_result += std::string(GetColumnName(static_cast<passtore::Column>(i))) + ", ";
            }
            s_result.resize(s_result.size() - 2);
        }

        return s_result;
    }
}

const std::string& passtore::MakeResourcesTableCreateQuery()
{
    static std::string s_query;
    if (s_query.empty())
    {
        s_query = "CREATE TABLE 'Resources' (";
        for (int i = ColumnFirst; i < ColumnCount; ++i)
        {
                s_query += "'" + std::string(GetColumnName(static_cast<Column>(i))) + "' BLOB,";
        }
        s_query.resize(s_query.size() - 1);
        s_query += ");";
    }

    return s_query;
}

const std::string& passtore::MakeResourcesCountQuery()
{
    static std::string s_query;
    if (s_query.empty())
    {
        s_query = "SELECT COUNT(ROWID) FROM Resources;";
    }

    return s_query;
}

const std::string& passtore::MakeResourcesListQuery()
{
    static std::string s_query;
    if (s_query.empty())
    {
        s_query = "SELECT ROWID FROM Resources;";
    }

    return s_query;
}

std::string passtore::MakeResourceSelectQuery(int rowId)
{
    static std::string s_query;
    if (s_query.empty())
    {
        s_query = "SELECT ";
        s_query += getCommaSeparatedColumns();
        s_query += " FROM Resources WHERE ROWID=";
    }

    std::string newQuery(s_query);
    newQuery += std::to_string(rowId) + ";";
    return newQuery;
}

std::string passtore::MakeResourcesSelectQuery(int fromRowId, int toRowId)
{
    std::string query("SELECT ");
    query += getCommaSeparatedColumns();
    query += "FROM Resources WHERE ROWID >= " + std::to_string(fromRowId);
    query += + " AND ROWID <= " + std::to_string(toRowId) + ";";
    return query;
}

std::string passtore::MakeResourceSelectPropertyQuery(int rowId, Column col)
{
    auto query = "SELECT " + std::string(GetColumnName(col)) + " FROM Resources WHERE ROWID=;";
    query += std::to_string(rowId) + ";";
    return query;
}

std::string passtore::MakeResourceUpdateQuery(int rowId)
{
    static std::string s_query;
    if (s_query.empty())
    {
        s_query = "UPDATE 'Resources' SET ";
        for (int i = ColumnFirst; i < ColumnCount; ++i)
        {
            auto col = static_cast<Column>(i);
                s_query += std::string("'") + GetColumnName(col) + "'=?, ";
        }
        s_query.resize(s_query.size() - 2);
        s_query += " WHERE ROWID=";
    }

    std::string newQuery(s_query);
    newQuery += std::to_string(rowId) + ";";
    return newQuery;
}

std::string passtore::MakeResourceValueUpdateQuery(int rowId, int valueId)
{
    std::string query("UPDATE 'Resources' SET ");
    auto col = static_cast<Column>(valueId);
    query += GetColumnName(col);
    query += "=? WHERE ROWID=" + std::to_string(rowId);
    return query;
}

QString passtore::MakeResourcePropertyUpdateQuery(int rowId, Column col)
{
    QString query("UPDATE 'Resources' SET '");
    query += GetColumnName(col);
    query += "'=? WHERE ROWID=%1";
    return query.arg(rowId);
}

const std::string& passtore::MakeResourceInsertQuery()
{
    static std::string s_query;
    if (s_query.empty())
    {
        s_query = "INSERT INTO Resources DEFAULT VALUES;";
    }
    return s_query;
}

const std::string& passtore::MakeResourceInsertQueryValues()
{
    static std::string s_query;
    if (s_query.empty())
    {
        s_query = "INSERT INTO Resources (";
        for (int i = ColumnFirst; i < ColumnCount; ++i)
        {
                s_query += "'" + std::string(GetColumnName(static_cast<Column>(i))) + "', ";
        }
        s_query.resize(s_query.size() - 2);
        s_query += ") VALUES (" + QString("?, ").repeated(ColumnCount).toStdString();
        s_query.resize(s_query.size() - 2);
        s_query += ");";
    }
    return s_query;
}
