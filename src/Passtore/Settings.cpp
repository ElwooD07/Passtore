#include "pch.h"
#include "Settings.h"
#include "Storages/Marshaling.h"

using namespace passtore;

void Settings::Load(const std::filesystem::path& settingsPath,
                    const ResourcesDefinition& defs)
{
    table = LoadTableSettings(settingsPath, defs);
}

void Settings::Save(const std::filesystem::path& settingsPath) const
{
    SaveTableSettings(settingsPath, table);
}
