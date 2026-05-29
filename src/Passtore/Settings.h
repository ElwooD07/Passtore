#pragma once
#include <filesystem>
#include "Storages/TableSettings.h"

namespace passtore
{
    /* Top-level user settings loaded from and saved to a JSON file alongside the DB. */
    struct Settings
    {
        TableSettings table;

        void Load(const std::filesystem::path& settingsPath,
                  const ResourcesDefinition& defs);
        void Save(const std::filesystem::path& settingsPath) const;
    };
}
