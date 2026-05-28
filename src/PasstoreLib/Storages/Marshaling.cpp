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
    return document.dump();
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

TableSettings passtore::LoadTableSettings(const std::filesystem::path& path,
                                           const ResourcesDefinition& defs)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return TableSettings::FromDefinition(defs);
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    auto doc = json::JSON::Load(ss.str());

    TableSettings saved;
    auto cols = doc["columns"];
    if (!cols.IsNull())
    {
        auto range = cols.ArrayRange();
        for (auto it = range.begin(); it != range.end(); ++it)
        {
            ColumnSettings cs;
            cs.name    = (*it)["name"].ToString();
            cs.visible = (*it)["visible"].ToBool();
            cs.blured  = (*it)["blured"].ToBool();
            saved.columns.push_back(cs);
        }
    }

    return TableSettings::MergeWithSaved(defs, saved);
}

void passtore::SaveTableSettings(const std::filesystem::path& path,
                                  const TableSettings& settings)
{
    json::JSON doc;
    doc["columns"] = json::Array();

    for (size_t i = 0; i < settings.columns.size(); ++i)
    {
        const auto& col = settings.columns[i];
        auto idx = static_cast<unsigned int>(i);
        doc["columns"][idx]["name"]    = col.name;
        doc["columns"][idx]["visible"] = col.visible;
        doc["columns"][idx]["blured"]  = col.blured;
    }

    std::ofstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("SaveTableSettings: cannot open: " + path.string());
    }
    file << doc.dump();
}
