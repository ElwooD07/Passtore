#include "pch.h"
#include "Marshaling.h"
#include "Security/SecureMemory.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)
#endif
#include "json.hpp"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

using namespace passtore;

namespace
{
    json::JSON MarshalValuesRecursively(const std::vector<ResourceValue>& values)
    {
        auto doc = json::Array();
        for (size_t i = 0; i < values.size(); ++i)
        {
            auto index = static_cast<unsigned int>(i);
            doc[index]["n"] = values[i].name;
            doc[index]["v"] = values[i].value;
            if (!values[i].children.empty())
            {
                doc[index]["c"] = MarshalValuesRecursively(values[i].children);
            }
        }
        return doc;
    }

    void UnmarshalValuesRecursively(json::JSON& doc, std::vector<ResourceValue>& values)
    {
        if (doc.IsNull())
        {
            return;
        }

        auto range = doc.ArrayRange();

        for (auto it = range.begin(); it != range.end(); ++it)
        {
            ResourceValue value;
            value.name = (*it)["n"].ToString();
            value.value = (*it)["v"].ToString();
            UnmarshalValuesRecursively((*it)["c"], value.children);
        }
    }
}

std::string passtore::MarshalResourceToJSON(const Resource& resource)
{
    json::JSON document;
    document["subject"] = resource.subject;
    if (!resource.values.empty())
    {
        document["values"] = MarshalValuesRecursively(resource.values);
    }
    return document.ToString();
}

void passtore::UnmarshalResourceFromJSON(const std::string& data, Resource& resource)
{
    json::JSON document = json::JSON::Load(data);
    resource.subject = document["subject"].ToString();
    UnmarshalValuesRecursively(document["values"], resource.values);
}

void passtore::UnmarshalResourceFromJSON(Secret data, Resource& resource)
{
    size_t effectiveSize = data.size();
    for (size_t i = 0; i < data.size(); ++i)
    {
        if (data[i] == '\0')
        {
            effectiveSize = i;
            break;
        }
    }

    std::string jsonText(reinterpret_cast<const char*>(data.data()), effectiveSize);
    UnmarshalResourceFromJSON(jsonText, resource);
    SecureWipe(jsonText.data(), jsonText.size());
}
