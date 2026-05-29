#pragma once
#include "Models/ITableModel.h"
#include "Storages/IResourceStorage.h"
#include "Models/Cache.h"

namespace passtore
{
    /* ITableModel backed by an IResourceStorage; caches decoded rows with an LRU Cache. */
    class ResourceTableModel : public ITableModel
    {
    public:
        explicit ResourceTableModel(IResourceStorage* storage);
        ResourceTableModel(const ResourceTableModel&) = delete;

        int RowCount() const override;
        int ColumnCount() const override;
        std::string CellData(int row, int col) const override;
        bool IsBigColumn(int col) const override;
        bool IsVisibleColumn(int col) const override;
        std::string ColumnName(int col) const override;
        std::string RowSubject(int row) const override;
        bool SetCellData(int row, int col, const std::string& value) override;
        ResourceId RowId(int row) const override;
        int AddRow() override;
        bool DeleteRow(int row) override;
        void SetErrorCallback(ErrorCallback cb, void* ctx) override;

    private:
        Resource* GetResource(int row) const;
        void RefreshIds();

    private:
        IResourceStorage* m_storage = nullptr;
        ResourcesDefinition m_resourcesDefs;
        std::vector<ResourceId> m_ids;
        mutable Cache<int, Resource> m_cache;
        ErrorCallback m_errorCb = nullptr;
        void* m_errorCtx = nullptr;
    };
}
