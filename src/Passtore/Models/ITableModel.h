#pragma once
#include <string>
#include <cstdint>
#include "Storages/Resource.h"

namespace passtore
{
    // Abstract table model — no Qt dependency.
    // Rows map to Resources, columns map to ResourceValues.
    class ITableModel
    {
    public:
        virtual ~ITableModel() = default;

        virtual int rowCount() const = 0;
        virtual int columnCount() const = 0;

        virtual std::string cellData(int row, int col) const = 0;
        virtual bool isBigColumn(int col) const = 0;
        virtual std::string columnName(int col) const = 0;
        virtual std::string rowSubject(int row) const = 0;
        virtual bool setCellData(int row, int col, const std::string& value) = 0;

        // Returns resource ID for a row. Returns InvalidResourceId if out of range.
        virtual ResourceId rowId(int row) const = 0;

        // Add a new empty row. Returns new row index, or -1 on error.
        virtual int addRow() = 0;

        // Delete a row by index. Returns false if row doesn't exist.
        virtual bool deleteRow(int row) = 0;

        using ErrorCallback = void(*)(void* ctx, const std::string& message);
        virtual void setErrorCallback(ErrorCallback cb, void* ctx) = 0;
    };
}
