#include "stdafx.h"
#include "ResourcesModel.h"

ResourcesModel::ResourcesModel(QObject* parent, Database& database)
    : QAbstractTableModel(parent)
    , m_db(database)
    , m_defs(m_db.GetResourceDefinitions())
{ }

int ResourcesModel::rowCount(const QModelIndex& parent) const
{
    return m_defs.size();
}

int ResourcesModel::columnCount(const QModelIndex&) const
{
    return ResourcePropertiesCount;
}

bool ResourcesModel::hasChildren(const QModelIndex& parent) const
{
    return !parent.isValid();
}

QVariant ResourcesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return { };
    }

    try
    {
        if (role == Qt::DisplayRole)
        {
            if (index.column() == ResourcePropertyName)
            {
                return m_defs.at(index.row()).name;
            }
            return m_db.GetResourcePropertyValue(m_defs.at(index.row()).id, static_cast<ResourceProperty>(index.column()));
        }
    }
    catch(const std::exception& ex)
    {
        emit ErrorOccurred(ex.what());
    }
    return { };
}

bool ResourcesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    try
    {
        m_db.SetResourcePropertyValue(index.row(), static_cast<ResourceProperty>(index.column()), value.toString());
        return true;
    }
    catch (const std::exception& ex)
    {
        emit ErrorOccurred(ex.what());
    }
    return false;
}

Qt::ItemFlags ResourcesModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant ResourcesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return Resource::PropertyNameTranslated(static_cast<ResourceProperty>(section));
    }
    return { };
}
