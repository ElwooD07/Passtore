#pragma once

namespace passtore
{
    enum Column
    {
        ColumnUnknown = -1,
        ColumnFirst = 0,
        ColumnName = ColumnFirst,
        ColumnEmail,
        ColumnPassword,
        ColumnUsername,
        ColumnDescription,
        ColumnAdditional,
        ColumnCount
    };

    const char* GetColumnName(Column col);
}
