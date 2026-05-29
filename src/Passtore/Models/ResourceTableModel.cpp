#include "pch.h"
#include "ResourceTableModel.h"

using namespace passtore;

ResourceTableModel::ResourceTableModel(IResourceStorage* storage)
    : m_storage(storage)
    , m_cache(1000)
{
    m_resourcesDefs = storage->GetResourcesDefinition();
    RefreshIds();
}

int ResourceTableModel::rowCount() const
{
    return static_cast<int>(m_ids.size());
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
        if (res && col < static_cast<int>(res->values.size())) {
            return res->values[col].value;
        }
    }
    catch (const std::exception& ex)
    {
        if (m_errorCb) {
            m_errorCb(m_errorCtx, ex.what());
        }
    }
    return {};
}

bool ResourceTableModel::isBigColumn(int col) const
{
    if (col < 0 || col >= static_cast<int>(m_resourcesDefs.size())) {
        return false;
    }
    return m_resourcesDefs[col].big;
}

bool ResourceTableModel::isVisibleColumn(int col) const
{
    if (col < 0 || col >= static_cast<int>(m_resourcesDefs.size())) {
        return true;
    }
    return m_resourcesDefs[col].visible;
}

std::string ResourceTableModel::columnName(int col) const
{
    if (col < 0 || col >= static_cast<int>(m_resourcesDefs.size())) {
        return {};
    }
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
        if (!res) {
            return false;
        }
        if (col >= static_cast<int>(res->values.size())) {
            return false;
        }
        res->values[col].value = value;
        m_storage->Upsert(*res);
        return true;
    }
    catch (const std::exception& ex)
    {
        if (m_errorCb) {
            m_errorCb(m_errorCtx, ex.what());
        }
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
        ResourceId newId = m_storage->Upsert(res);
        if (newId != InvalidResourceId) {
            m_ids.push_back(newId);
            return static_cast<int>(m_ids.size()) - 1;
        }
    }
    catch (const std::exception& ex)
    {
        if (m_errorCb) {
            m_errorCb(m_errorCtx, ex.what());
        }
    }
    return -1;
}

bool ResourceTableModel::deleteRow(int row)
{
    try
    {
        ResourceId id = rowId(row);
        if (id == InvalidResourceId) {
            return false;
        }
        m_storage->DeleteResource(id);
        m_ids.erase(m_ids.begin() + row);
        m_cache.Clear();
        return true;
    }
    catch (const std::exception& ex)
    {
        if (m_errorCb) {
            m_errorCb(m_errorCtx, ex.what());
        }
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
        if (cached) {
            return cached;
        }
        if (row < 0 || row >= static_cast<int>(m_ids.size())) {
            return nullptr;
        }
        Resource res;
        m_storage->GetOne(m_ids[row], res);
        return &m_cache.Set(row, res);
    }
    catch (const std::exception& ex)
    {
        if (m_errorCb) {
            m_errorCb(m_errorCtx, ex.what());
        }
    }
    return nullptr;
}

void ResourceTableModel::RefreshIds()
{
    m_ids.clear();
    m_cache.Clear();
    Resource res;
    ResourceId id = InvalidResourceId;
    while (m_storage->GetNext(id, res) == ResourceState::Present)
    {
        m_ids.push_back(res.id);
        id = res.id;
    }
}
