#pragma once
#include "Storages/SQLite/SQLiteColumns.h"

namespace passtore
{
    const QString& MakeResourcesTableCreateQuery();
    const QString& MakeResourcesCountQuery();
    QString MakeResourceSelectQuery(int rowId);
    QString MakeResourceSelectPropertyQuery(int rowId, Column col);
    QString MakeResourceUpdateQuery(int rowId);
    QString MakeResourcePropertyUpdateQuery(int rowId, Column col);
    const QString& MakeResourceInsertQuery();
    const QString& MakeResourceInsertQueryValues();
}
