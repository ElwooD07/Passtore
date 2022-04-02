#include "pch.h"
#include "ResourceTableModel.h"
#include "ResourceTableModelRoles.h"

using namespace passtore;

ResourceTableModel::ResourceTableModel(QObject* parent, IStorage* storage)
    : QAbstractTableModel(parent)
    , m_storage(storage)
    , m_cache(1000)
{
    storage->GetResourcesDefinition(m_resourcesDefs);
}

int ResourceTableModel::rowCount(const QModelIndex&) const
{
    return m_storage->GetResourcesCount();
}

int ResourceTableModel::columnCount(const QModelIndex&) const
{
    return m_resourcesDefs.size();
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
        switch (role)
        {
        case Qt::DisplayRole:
        {
            auto resource = GetResource(index.row());
            if (resource != nullptr)
            {
                return resource->data.at(index.column());
            }
            break;
        }
        case ResourceTableModelRole::IsBigColumn:
            return m_resourcesDefs.at(index.column()).big;
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
    if (!IndexIsValid(index) || role != Qt::EditRole)
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
        m_storage->SetResource(index.row(), *resource);
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

passtore::Resource* ResourceTableModel::GetResource(int id) const
{
    try
    {
        auto cachedResource = m_cache.Get(id);
        if (cachedResource != nullptr)
        {
            return cachedResource;
        }

        Resource newResource;
        m_storage->GetResource(id, newResource);
        return &m_cache.Set(id, newResource);
    }
    catch(const std::exception& ex)
    {
        emit ErrorOccurred(tr("Failed to get resource %1: '%2'").arg(id).arg(ex.what()));
    }
    return nullptr;
}

bool ResourceTableModel::IndexIsValid(const QModelIndex& index) const
{
    return index.isValid() &&
        (index.row() < m_storage->GetResourcesCount()) &&
        (index.column() < m_resourcesDefs.size());
}
