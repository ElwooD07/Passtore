#pragma once
#include <cstdint>
#include <vector>

struct sqlite3;
struct sqlite3_stmt;

namespace passtore
{
    namespace sqlite
    {
        class Connection;

        using BlobData = std::vector<uint8_t>;

        // https://github.com/ElwooD07/DBContainer/blob/master/src/DbContainerLib/impl/SQLQuery.h
        class Query
        {
        public:
            explicit Query(sqlite3* db, const std::string& query = "");
            ~Query();

            Query(const Query&) = delete;
            Query& operator=(const Query&) = delete;

            Query(Query&& other);
            Query& operator=(Query&& other);

            void Prepare(const std::string& query);

            // The leftmost SQL parameter has an index of 1
            void BindBool(int column, bool value);
            void BindInt(int column, int value);
            void BindInt64(int column, int64_t value);
            void BindReal(int column, double value);
            void BindText(int column, const std::string& value);
            void BindBlob(int column, const BlobData& data);
            void BindBlob(int column, const char* data, size_t size);

            bool Step(); // Returns true if sqlite API returns SQLITE_ROW
            void Reset();

            // The leftmost column of the result set has the index 0
            bool ColumnBool(int column);
            int ColumnInt(int column);
            int64_t ColumnInt64(int column);
            double ColumnReal(int column);
            void ColumnText(int column, std::string& out);
            void ColumnBlob(int column, BlobData& data);

            int64_t LastRowId();

        private:
            void CheckSTMT();
            void DecideToThrow(int errCode);

        private:
            ::sqlite3* m_db = nullptr;
            ::sqlite3_stmt* m_stmt = nullptr;
            int64_t m_lastRowId = 0;
        };
    }
}
