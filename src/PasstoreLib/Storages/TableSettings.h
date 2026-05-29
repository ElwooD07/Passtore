#pragma once
#include <string>
#include <vector>
#include "Storages/IResourceStorage.h"

namespace passtore
{
    // TODO: merge ColumnSettings::visible (hide column) with ResourceDefinition::visible (mask content)
    //       into a unified per-column descriptor once the distinction is reflected in the UI.
    /* Per-column user preference: name mirrors ResourceDefinition::name, visible controls rendering. */
    struct ColumnSettings
    {
        std::string name;
        bool visible = true;
    };

    /* Serialisable user preferences for the resource table — column visibility overrides. */
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
