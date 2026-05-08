#pragma once
#include <string>
#include <vector>

namespace passtore
{
    struct ColumnSettings
    {
        std::string name;
        bool visible = false;
        bool blured = false;
    };

    struct TableSettings
    {
        std::vector<ColumnSettings> columns;
    };
}
