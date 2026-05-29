#include "pch.h"
#include "Storages/TableSettings.h"

using namespace passtore;

TableSettings TableSettings::FromDefinition(const ResourcesDefinition& defs)
{
    TableSettings result;
    for (const auto& def : defs)
    {
        result.columns.push_back({ def.name, true });
    }
    return result;
}

TableSettings TableSettings::MergeWithSaved(const ResourcesDefinition& defs,
                                             const TableSettings& saved)
{
    TableSettings result;
    for (const auto& def : defs)
    {
        ColumnSettings col{ def.name, true };

        for (const auto& s : saved.columns)
        {
            if (s.name == def.name)
            {
                col.visible = s.visible;
                break;
            }
        }

        result.columns.push_back(col);
    }
    return result;
}
