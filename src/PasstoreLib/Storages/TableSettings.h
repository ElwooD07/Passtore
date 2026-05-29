#pragma once
#include <string>
#include <vector>
#include "Storages/IResourceStorage.h"

namespace passtore
{
    struct ColumnSettings
    {
        std::string name;
        bool visible = true;
    };

    struct TableSettings
    {
        std::vector<ColumnSettings> columns;

        // Build defaults from the DB definition — all columns visible.
        static TableSettings FromDefinition(const ResourcesDefinition& defs);

        // Merge saved user preferences on top of a fresh definition.
        // - New columns in defs get defaults.
        // - Columns removed from defs are dropped.
        // - Saved visible value is preserved where column name matches.
        static TableSettings MergeWithSaved(const ResourcesDefinition& defs,
                                            const TableSettings& saved);
    };
}
