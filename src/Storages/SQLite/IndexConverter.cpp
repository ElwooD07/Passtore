#include "pch.h"
#include "IndexConverter.h"

size_t passtore::IndexConverter::Count()
{
    return GetRowIds().size();
}

size_t passtore::IndexConverter::ToRowId(size_t dbId)
{
    return GetRowIds().at(dbId);
}

size_t passtore::IndexConverter::ToId(size_t rowId)
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
            // record witht= this rowId is probably removed, so skip the search
            return InvalidId;
        }
    } while(--id);

    return InvalidId;
}

size_t passtore::IndexConverter::AddRowId(size_t rowId)
{
    auto& rowIds = GetRowIds();
    rowIds.push_back(rowId);
    return rowIds.size();
}

void passtore::IndexConverter::RemoveId(size_t dbId)
{
    GetRowIds().removeAt(dbId);
}

bool passtore::IndexConverter::isEmpty()
{
    return GetRowIds().isEmpty();
}

QVector<size_t>& passtore::IndexConverter::GetRowIds()
{
    if (!m_rowIds.isEmpty())
    {
        return m_rowIds;
    }

    static QVector<size_t> s_empty;

    QSqlQuery query;
    if (!query.exec("SELECT COUNT(ROWID) FROM Resources"))
    {
        qDebug() << "Unable to init resources list: " << query.lastError();
        return s_empty;
    }

    bool ok = false;
    auto count = query.value(0).toULongLong(&ok);
    if (!ok)
    {
        qDebug() << "Unable to init resources list: unexpected query result";
        return s_empty;
    }

    m_rowIds.resize(count);
    if (!query.exec("SELECT ROWID FROM Resources"))
    {
        qDebug() << "Unable to init resources list: " << query.lastError();
        m_rowIds.clear();
        return s_empty;
    }

    for (size_t idx = 0; idx < count; ++idx)
    {
        m_rowIds[idx] = query.value(0).toULongLong(&ok);
        if (!ok)
        {
            qDebug() << "Unable to init resources list: unexpected query result";
            m_rowIds.clear();
            return s_empty;
        }
        query.next();
    }

    return m_rowIds;
}
