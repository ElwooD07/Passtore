#pragma once
#include "Resource.h"

namespace passtore
{
    // Doesn't marshal id field
    std::string MarshalResourceToJSON(const Resource& resource);
    // Doesn't unmarshal id field
    void UnmarshalResourceFromJSON(const std::string& data, Resource& resource);
}
