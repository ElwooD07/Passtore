#include "pch.h"
#include "ResourceTableModel.h"

using namespace passtore;

ResourceTableModel::ResourceTableModel(QObject* parent, IStorage* storage)
    : QAbstractTableModel(parent)
    , m_storage(storage)
{ }

int ResourceTableModel::rowCount(const QModelIndex&) const
{
    return m_resourcesDefs.size();
}

int ResourceTableModel::columnCount(const QModelIndex&) const
{
    return m_storage->GetResourcesCount();
}

bool ResourceTableModel::hasChildren(const QModelIndex& parent) const
{
    return !parent.isValid();
}

QVariant ResourceTableModel::data(const QModelIndex& index, int role) const
{
    if (!IndexIsValid(index))
    {
        return {};
    }

    try
    {
        if (role == Qt::DisplayRole)
        {
            auto resource = GetResource(index.row());
            if (resource != nullptr)
            {
                return resource->data.at(index.column());
            }
        }
    }
    catch(const std::exception& ex)
    {
        emit ErrorOccurred(ex.what());
    }
    return { };
}

bool ResourceTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!IndexIsValid(index) || role != Qt::DisplayRole)
    {
        return {};
    }

    try
    {
        auto resource = GetResource(index.row());
        if (resource == nullptr)
        {
            return false;
        }

        resource->data[index.column()] = value.toString();
        return SetResource(index.row(), resource);
    }
    catch (const std::exception& ex)
    {
        emit ErrorOccurred(ex.what());
    }
    return false;
}

Qt::ItemFlags ResourceTableModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant ResourceTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return m_resourcesDefs.at(section).name;
    }
    return { };
}

Resource* ResourceTableModel::GetResource(int id) const
{
    try
    {
        auto cachedResource = m_cache.take(id);
        if (cachedResource != nullptr)
        {
            return cachedResource;
        }

        Resource newResource;
        m_storage->GetResource(id, newResource);

        // Move resource to the heap to make it cacheable
        auto newResourceInHeap = new Resource(std::move(newResource));
        auto inserted = m_cache.insert(id, newResourceInHeap);
        if (!inserted)
        {
            // Delete the resource because it will leak
            delete newResourceInHeap;
            newResourceInHeap = nullptr;
            emit ErrorOccurred(tr("Failed to cache resource %1").arg(id));
        }

        return newResourceInHeap;
    }
    catch(const std::exception& ex)
    {
        emit ErrorOccurred(tr("Failed to get resource %1: '%2'").arg(id).arg(ex.what()));
    }
}

bool ResourceTableModel::SetResource(int id, Resource* resource)
{
    try
    {
        auto inserted = m_cache.insert(id, resource);
        if (inserted)
        {
            m_storage->SetResource(id, *resource);
        }
        else
        {
            // Do not delete the resource because it is already owned by cache
            emit ErrorOccurred(tr("Failed to cache resource %1").arg(id));
        }

        return inserted;
    }
    catch(const std::exception& ex)
    {
        emit ErrorOccurred(tr("Failed to save resource %1: '%2'").arg(id).arg(ex.what()));
    }

    return false;
}

bool ResourceTableModel::IndexIsValid(const QModelIndex& index) const
{
    return index.isValid() &&
        (index.row() < m_storage->GetResourcesCount()) &&
        (index.column() < m_resourcesDefs.size());
}
