#include "pch.h"
#include "ResourceTableModel.h"

using namespace passtore;

ResourceTableModel::ResourceTableModel(IResourceStorage* storage)
    : m_storage(storage)
    , m_cache(1000)
{
    m_resourcesDefs = storage->GetResourcesDefinition();
}

int ResourceTableModel::rowCount() const
{
    return static_cast<int>(m_storage->GetResourcesCount());
}

int ResourceTableModel::columnCount() const
{
    return static_cast<int>(m_resourcesDefs.size());
}

std::string ResourceTableModel::cellData(int row, int col) const
{
    try
    {
        auto* res = GetResource(row);
        if (res && col < static_cast<int>(res->values.size()))
            return res->values[col].value;
    }
    catch (const std::exception& ex)
    {
        if (m_errorCb) m_errorCb(m_errorCtx, ex.what());
    }
    return {};
}

bool ResourceTableModel::isBigColumn(int col) const
{
    if (col < 0 || col >= static_cast<int>(m_resourcesDefs.size()))
        return false;
    return m_resourcesDefs[col].big;
}

std::string ResourceTableModel::columnName(int col) const
{
    if (col < 0 || col >= static_cast<int>(m_resourcesDefs.size()))
        return {};
    return m_resourcesDefs[col].name;
}

std::string ResourceTableModel::rowSubject(int row) const
{
    auto* res = GetResource(row);
    return res ? res->subject : std::string{};
}

bool ResourceTableModel::setCellData(int row, int col, const std::string& value)
{
    try
    {
        auto* res = GetResource(row);
        if (!res) return false;
        if (col >= static_cast<int>(res->values.size())) return false;
        res->values[col].value = value;
        m_storage->Upsert(*res);
        return true;
    }
    catch (const std::exception& ex)
    {
        if (m_errorCb) m_errorCb(m_errorCtx, ex.what());
    }
    return false;
}

ResourceId ResourceTableModel::rowId(int row) const
{
    auto* res = GetResource(row);
    return res ? res->id : InvalidResourceId;
}

int ResourceTableModel::addRow()
{
    try
    {
        Resource res;
        res.id = InvalidResourceId;
        res.subject = "New entry";
        res.values.resize(m_resourcesDefs.size());
        m_storage->Upsert(res);
        return rowCount() - 1;
    }
    catch (const std::exception& ex)
    {
        if (m_errorCb) m_errorCb(m_errorCtx, ex.what());
    }
    return -1;
}

bool ResourceTableModel::deleteRow(int row)
{
    try
    {
        ResourceId id = rowId(row);
        if (id == InvalidResourceId) return false;
        m_storage->DeleteResource(id);
        m_cache.Remove(row);
        return true;
    }
    catch (const std::exception& ex)
    {
        if (m_errorCb) m_errorCb(m_errorCtx, ex.what());
    }
    return false;
}

void ResourceTableModel::setErrorCallback(ErrorCallback cb, void* ctx)
{
    m_errorCb = cb;
    m_errorCtx = ctx;
}

Resource* ResourceTableModel::GetResource(int row) const
{
    try
    {
        auto* cached = m_cache.Get(row);
        if (cached) return cached;
        Resource res;
        m_storage->GetOne(static_cast<ResourceId>(row), res);
        return &m_cache.Set(row, res);
    }
    catch (const std::exception& ex)
    {
        if (m_errorCb) m_errorCb(m_errorCtx, ex.what());
    }
    return nullptr;
}
