#pragma once
#include "Storages/Resource.h"

namespace passtore
{
class IResourceStorage
    {
    public:
        virtual ~IResourceStorage() { }

        // Path in UTF8
        virtual void Open(const std::filesystem::path& path, const std::string& password) = 0;
        virtual void ChangePassword(const std::string& oldPassword, const std::string& newPassword) = 0;

        virtual ResourceState GetOne(ResourceId id, Resource& resource) = 0;
        // Should return first resource when id == InvalidResourceId
        virtual ResourceState GetNext(ResourceId id, Resource& resource) = 0;
        // Set resource.id = InvalidResourceId to add resource - it should return Id of created resource.
        // Returns InvalidResourceId if resource.id is not InvalidResourceId and doesn't exist.
        virtual ResourceId Upsert(const Resource& resource) = 0;
        virtual void DeleteResource(ResourceId id) = 0;
        virtual void Swap(ResourceId first, ResourceId second) = 0;
    };
}
