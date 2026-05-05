#pragma once
#include "Models/ITableModel.h"
#include "Storages/IResourceStorage.h"
#include "Models/Cache.h"

namespace passtore
{
    class ResourceTableModel : public ITableModel
    {
    public:
        explicit ResourceTableModel(IResourceStorage* storage);
        ResourceTableModel(const ResourceTableModel&) = delete;

        int rowCount() const override;
        int columnCount() const override;
        std::string cellData(int row, int col) const override;
        bool isBigColumn(int col) const override;
        std::string columnName(int col) const override;
        std::string rowSubject(int row) const override;
        bool setCellData(int row, int col, const std::string& value) override;
        ResourceId rowId(int row) const override;
        int addRow() override;
        bool deleteRow(int row) override;
        void setErrorCallback(ErrorCallback cb, void* ctx) override;

    private:
        Resource* GetResource(int row) const;

    private:
        IResourceStorage* m_storage = nullptr;
        ResourcesDefinition m_resourcesDefs;
        mutable Cache<int, Resource> m_cache;
        ErrorCallback m_errorCb = nullptr;
        void* m_errorCtx = nullptr;
    };
}
