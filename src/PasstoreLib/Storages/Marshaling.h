#pragma once
#include <filesystem>
#include "Resource.h"
#include "Storages/TableSettings.h"
#include "Utils/DataUtils.h"

namespace passtore
{
    // Doesn't marshal id field
    std::string MarshalResourceToJSON(const Resource& resource);
    // Doesn't unmarshal id field
    void UnmarshalResourceFromJSON(const std::string& data, Resource& resource);
    // Doesn't unmarshal id field; expects UTF-8 JSON with optional trailing zero/padding.
    void UnmarshalResourceFromJSON(Secret data, Resource& resource);

    // Returns defaults if the file is absent or unreadable.
    TableSettings LoadTableSettings(const std::filesystem::path& path,
                                    const ResourcesDefinition& defs);
    void SaveTableSettings(const std::filesystem::path& path,
                           const TableSettings& settings);
}
