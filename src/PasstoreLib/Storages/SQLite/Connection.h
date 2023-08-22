#pragma once
#include "Storages/SQLite/Query.h"
#include "Storages/SQLite/Transaction.h"

namespace passtore
{
    namespace sqlite
    {
        // https://github.com/ElwooD07/DBContainer/blob/master/src/DbContainerLib/impl/Connection.h
        class Connection
        {
        public:
            Connection();
            Connection(const std::filesystem::path& dbPath);
            ~Connection();

            void Reconnect(const std::filesystem::path& dbPath);
            void Disconnect();
            sqlite3* Release() noexcept;

            void ExecQuery(const std::string& query);
            Query CreateQuery(const std::string& query = "");

            Transaction CreateTransaction();

        private:
            void Connect(const std::filesystem::path& dbPath);
            void CheckDB();

        private:
            sqlite3* m_db = nullptr;
            TransactionsResources m_transactionResources;
        };
    }
}

