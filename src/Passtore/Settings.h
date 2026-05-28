#pragma once
#include <filesystem>
#include "Storages/TableSettings.h"

namespace passtore
{
    struct Settings
    {
        TableSettings table;

        void Load(const std::filesystem::path& settingsPath,
                  const ResourcesDefinition& defs);
        void Save(const std::filesystem::path& settingsPath) const;
    };
}
