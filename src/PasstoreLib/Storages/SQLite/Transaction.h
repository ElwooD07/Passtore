#pragma once

namespace passtore
{
    namespace sqlite
    {
        class Connection;

        namespace detail
        {
            void CreateNewSavepointName(std::string& name);
        }

        class TransactionsResources
        {
        public:
            explicit TransactionsResources(Connection* connection);
            Connection* GetConnection();
            std::string NextTransactionName();

        private:
            std::string m_lastTransactionName;
            std::mutex m_changeNameMutex;
            Connection* m_connection;
        };

        class Transaction
        {
        public:
            explicit Transaction(TransactionsResources& resources);
            ~Transaction(); // Rollback transaction if it is not commited
            void Commit();

        private:
            TransactionsResources& m_resources;
            bool m_committed = false;
            std::string m_transactionName;
        };
    }
}
