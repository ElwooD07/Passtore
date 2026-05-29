#pragma once
#include <string>
#include <cstdint>
#include "Storages/Resource.h"

namespace passtore
{
    /* Abstract table model used by ResourcesTableWidget; rows map to Resources, columns to field values. */
    class ITableModel
    {
    public:
        virtual ~ITableModel() = default;

        virtual int RowCount() const = 0;
        virtual int ColumnCount() const = 0;

        virtual std::string CellData(int row, int col) const = 0;
        virtual bool IsBigColumn(int col) const = 0;
        virtual bool IsVisibleColumn(int col) const = 0;
        virtual std::string ColumnName(int col) const = 0;
        virtual std::string RowSubject(int row) const = 0;
        virtual bool SetCellData(int row, int col, const std::string& value) = 0;

        virtual ResourceId RowId(int row) const = 0;
        virtual int AddRow() = 0;
        virtual bool DeleteRow(int row) = 0;

        using ErrorCallback = void(*)(void* ctx, const std::string& message);
        virtual void SetErrorCallback(ErrorCallback cb, void* ctx) = 0;
    };
}
