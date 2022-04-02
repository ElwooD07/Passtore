#pragma once

namespace passtore
{
    struct ResourceValueDefinition
    {
        QString name; // translated column name
        bool critical = false; // will be blured by default
        bool big = false; // indicates that this value may need big area to display and edit
    };

    // indexes are column numbers started from 0
    using ResourcesDefinition = QVector<ResourceValueDefinition>;

    struct Resource
    {
        QVector<QString> data; // change to std::vector or something more common for plugin support

        ~Resource()
        {
            // all the data considered as sensitive
            for (auto it = data.begin(); it != data.end(); ++it)
            {
                // Erase data to not keep sensitive data in memory
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

        virtual void Open(const QString& path, const QString& password) = 0;
        virtual void ChangePassword(const QString& newPassword) = 0;

        virtual int GetResourcesCount() = 0;
        virtual void GetResourcesDefinition(ResourcesDefinition& definition) = 0;

        virtual void GetResource(int id, Resource& resource) = 0;
        virtual void GetResources(int from, int to, QVector<Resource>& resources) = 0;
        virtual void SetResource(int id, const Resource& resource) = 0;
        virtual int AddResource(const Resource& resource) = 0; // Returns Id of created record
        virtual void SwapResources(int first, int second) = 0;
    };
}
