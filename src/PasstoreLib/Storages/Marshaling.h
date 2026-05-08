#pragma once
#include "Resource.h"
#include "Utils/DataUtils.h"

namespace passtore
{
    // Doesn't marshal id field
    std::string MarshalResourceToJSON(const Resource& resource);
    // Doesn't unmarshal id field
    void UnmarshalResourceFromJSON(const std::string& data, Resource& resource);
    // Doesn't unmarshal id field; expects UTF-8 JSON with optional trailing zero/padding.
    void UnmarshalResourceFromJSON(Secret data, Resource& resource);
}
