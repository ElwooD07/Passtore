#pragma once
#include <string>
#include <vector>

namespace passtore
{
    struct ResourceValueDefinition
    {
        std::string name; // translated column name in UTF8
        bool critical = false; // will be blured by default
        bool big = false; // indicates that this value may need big area to display and edit
    };

    // indexes are column numbers started from 0
    using ResourcesDefinition = QVector<ResourceValueDefinition>;

    struct Resource
    {
        // The array of data for each column. Each data considered as UTF8 string and sensitive
        std::vector<std::string> data;

        ~Resource()
        {
            // all the data considered as sensitive
            for (auto it = data.begin(); it != data.end(); ++it)
            {
                // Erase data to not keep it in heap
                memset(it->data(), 0, it->size());
            }
        }

        Resource() = default;
        Resource(const Resource&) = default;
        Resource& operator=(Resource&&) = default;
        Resource& operator=(const Resource&) = default;
        Resource(Resource&&) noexcept = default;
    };

    class IStorage
    {
    public:
        virtual ~IStorage() { }

        // Path in UTF8
        virtual void Open(const std::filesystem::path& path, const std::string& password) = 0;
        virtual void ChangePassword(const std::string& oldPassword, const std::string& newPassword) = 0;

        virtual int GetResourcesCount() = 0;
        virtual void GetResourcesDefinition(ResourcesDefinition& definition) = 0;

        virtual void GetResource(int id, Resource& resource) = 0;
        virtual void GetResources(int from, int to, std::vector<Resource>& resources) = 0;
        virtual void SetResource(int id, const Resource& resource) = 0;
        virtual void SetResourceValue(int id, int valueId, const QString& value) = 0;
        virtual uint64_t AddResource(const Resource& resource) = 0; // Returns Id of created record
        virtual void SwapResources(int first, int second) = 0;
    };
}
