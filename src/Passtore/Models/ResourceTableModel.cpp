#include "pch.h"
#include "ResourceTableModel.h"
#include "ResourceTableModelRoles.h"

using namespace passtore;

ResourceTableModel::ResourceTableModel(QObject* parent, IResourceStorage* storage)
    : QAbstractTableModel(parent)
    , m_storage(storage)
    , m_cache(1000)
{ }

int ResourceTableModel::rowCount(const QModelIndex&) const
{
    // TODO
    return 0;
}

int ResourceTableModel::columnCount(const QModelIndex&) const
{
    // TODO
    return 0;
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
                return QString::fromUtf8(resource->values.at(index.column()).c_str());
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
        
        resource->values[index.column()] = value.toString().toUtf8().data();
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
        return QString::fromUtf8(m_resourcesDefs.at(section).name.c_str());
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
        m_storage->GetOne(id, newResource);
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
