#include "pch.h"
#include "IndexConverter.h"
#include "SQLiteDatabaseQueries.h"

using namespace passtore;

sqlite::IndexConverter::IndexConverter(Connection& connection)
    : m_connection(connection)
{ }

int sqlite::IndexConverter::Count()
{
    return GetRowIds().size();
}

int sqlite::IndexConverter::ToRowId(int dbId)
{
    return static_cast<int>(GetRowIds().at(dbId));
}

int sqlite::IndexConverter::ToId(int rowId)
{
    if (rowId <= 0) // RowID in SQLite starts from 1
    {
        return InvalidId;
    }

    auto& rowIds = GetRowIds();
    if (rowIds.isEmpty())
    {
        return InvalidId;
    }

    auto id = rowId - 1;
    if (id >= rowIds.size())
    {
        id = rowIds.size() - 1;
    }

    auto closestRowId = rowIds.at(id);
    if (closestRowId < rowId)
    {
        /* RowId is always greater then Id because removed RowIds are not reused:
         *
         * id rowid
         * 0  1
         * 1  2
         * 2  3
         * 3  8
         * ...
         * 35 1024
        */
        qDebug() << "Something wrong with DB: unexpected order of RowIDs";
        return InvalidId;
    }

    do
    {
        if (rowIds.at(id) == rowId)
        {
            return id;
        }

        if (rowIds.at(id) < rowId)
        {
            // record witht this rowId is probably removed, so skip the search
            return InvalidId;
        }
    } while(--id);

    return InvalidId;
}

int sqlite::IndexConverter::AddRowId(int rowId)
{
    auto& rowIds = GetRowIds();
    rowIds.push_back(rowId);
    return rowIds.size();
}

void sqlite::IndexConverter::RemoveId(int dbId)
{
    GetRowIds().removeAt(dbId);
}

bool sqlite::IndexConverter::isEmpty()
{
    return GetRowIds().isEmpty();
}

QVector<size_t>& sqlite::IndexConverter::GetRowIds()
{
    if (!m_rowIds.isEmpty())
    {
        return m_rowIds;
    }

    static QVector<size_t> s_empty;

    try
    {
        auto query = m_connection.CreateQuery(MakeResourcesCountQuery());
        query.Step();
        m_rowIds.resize(query.ColumnInt64(0));

        query = m_connection.CreateQuery(MakeResourcesListQuery());
        for (size_t idx = 0; idx < static_cast<int>(m_rowIds.size()); ++idx)
        {
            query.Step();
            m_rowIds[idx] = query.ColumnInt64(0);
        }
    }
    catch(const std::exception& ex)
    {
        LOGE << "SQLite error: " << ex.what();
        return s_empty;
    }

    return m_rowIds;
}
