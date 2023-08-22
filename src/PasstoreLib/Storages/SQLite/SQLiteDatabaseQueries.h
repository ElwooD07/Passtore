#pragma once
#include "Storages/SQLite/SQLiteColumns.h"

namespace passtore
{
    const std::string& MakeResourcesTableCreateQuery();
    const std::string& MakeResourcesCountQuery();
    const std::string& MakeResourcesListQuery();
    std::string MakeResourceSelectQuery(int rowId);
    std::string MakeResourceValueUpdateQuery(int rowId, int valueId);
    std::string MakeResourcesSelectQuery(int fromRowId, int toRowId);
    std::string MakeResourceSelectPropertyQuery(int rowId, Column col);
    std::string MakeResourceUpdateQuery(int rowId);
    QString MakeResourcePropertyUpdateQuery(int rowId, Column col);
    const std::string& MakeResourceInsertQuery();
    const std::string& MakeResourceInsertQueryValues();
}
