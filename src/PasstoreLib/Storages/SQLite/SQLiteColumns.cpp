#include "pch.h"
#include "Storages/SQLite/SQLiteColumns.h"

using namespace passtore;

namespace
{
    static const char* s_columnNames[] = {
        QT_TR_NOOP("Resource"),
        QT_TR_NOOP("Email"),
        QT_TR_NOOP("Password"),
        QT_TR_NOOP("Username"),
        QT_TR_NOOP("Description"),
        QT_TR_NOOP("Additional"),
    };
    static_assert (sizeof(s_columnNames)/ sizeof(const char*) == ColumnCount,
                   "Not all names for resource properties are specified");
}


const char* passtore::GetColumnName(Column col)
{
    return s_columnNames[col];
}

const std::string& passtore::GetColumnPlaceholder(passtore::Column col)
{
    static std::vector<std::string> s_placeholders;
    if (s_placeholders.empty())
    {
        s_placeholders.resize(passtore::ColumnCount);
    }

    if (s_placeholders[col].empty())
    {
        s_placeholders[col] = std::string(":") + GetColumnName(col);
    }

    return s_placeholders[col];
}
