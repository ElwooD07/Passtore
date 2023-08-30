#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace passtore
{
    enum class ResourceValueFlags: uint32_t
    {
        Empty = 0x0,
        Critical = 0x1, // will be blured by default
        Big = 0x2, // indicates that this value may need big area to display and edit
    };

    struct ResourceValue
    {
        ~ResourceValue()
        {
            memset(name.data(), 0, name.size());
            memset(value.data(), 0, value.size());
        }

        std::string name;
        std::string value;
        ResourceValueFlags flags = ResourceValueFlags::Empty;
        std::vector<ResourceValue> children;
    };

    using ResourceId = uint32_t;
    constexpr ResourceId InvalidResourceId = -1;
    using ResourceIds = std::vector<ResourceId>;

    // Resource is a tree of ResourceValues
    struct Resource
    {
        ResourceId id = InvalidResourceId;
        std::string subject; // literally - caption
        // The array of data for each column. Each data considered as UTF8 string and sensitive
        std::vector<ResourceValue> values;

        Resource() = default;
        Resource(const Resource&) = default;
        Resource& operator=(Resource&&) = default;
        Resource& operator=(const Resource&) = default;
        Resource(Resource&&) noexcept = default;
    };

    enum class ResourceState: uint8_t
    {
        Unknown = 0,
        Present,
        Deleted,
        // Add others if you need to
    };
}
