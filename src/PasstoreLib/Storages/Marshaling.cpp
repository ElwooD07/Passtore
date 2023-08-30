#include "pch.h"
#include "Marshaling.h"
#include "json.hpp"

using namespace passtore;

namespace
{
    json::JSON MarshalValuesRecursively(const std::vector<ResourceValue>& values)
    {
        auto doc = json::Array();
        for (unsigned int i = 0; i < values.size(); ++i)
        {
            doc[i]["n"] = values[i].name;
            doc[i]["v"] = values[i].value;
            if (!values[i].children.empty())
            {
                doc["c"] = MarshalValuesRecursively(values);
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
