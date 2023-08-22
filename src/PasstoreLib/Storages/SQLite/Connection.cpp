#include "pch.h"
#include "Storages/SQLite/Connection.h"

using namespace passtore;

sqlite::Connection::Connection()
    : m_transactionResources(this)
{ }

sqlite::Connection::Connection(const std::filesystem::path& dbPath)
    : m_transactionResources(this)
{
    Connect(dbPath);
}

sqlite::Connection::~Connection()
{
    Disconnect();
}

void sqlite::Connection::Reconnect(const std::filesystem::path& dbPath)
{
    if (!dbPath.empty())
    {
        Disconnect();
        Connect(dbPath);
    }
}

void sqlite::Connection::Disconnect()
{
    int retCode = SQLITE_OK;
    if (m_db)
    {
        retCode = sqlite3_close(m_db);
        m_db = nullptr;
    }

    if (retCode != SQLITE_OK)
    {
        LOGE << "Failed to disconnect from database: " << retCode;
    }
}

sqlite3* sqlite::Connection::Release() noexcept
{
    auto db = m_db;
    m_db = nullptr;
    return db;
}

void sqlite::Connection::ExecQuery(const std::string& query)
{
    CheckDB();

    char* errStr = 0;
    if (sqlite3_exec(m_db, query.c_str(), 0, 0, &errStr) != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Failed to execute query: ") + errStr);
    }
}

sqlite::Query passtore::sqlite::Connection::CreateQuery(const std::string& query /*= ""*/)
{
    CheckDB();

    return Query(m_db, query);
}

sqlite::Transaction sqlite::Connection::CreateTransaction()
{
    return Transaction(m_transactionResources);
}

void sqlite::Connection::Connect(const std::filesystem::path& dbPath)
{
    int retCode = sqlite3_open_v2(dbPath.string().c_str(), &m_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_PRIVATECACHE, NULL);

    if (retCode != SQLITE_OK)
    {
        throw std::runtime_error("Failed to connect to the database '" + dbPath.string() + "': " + std::to_string(retCode));
    }
}

void sqlite::Connection::CheckDB()
{
    if (m_db == nullptr)
    {
        throw std::runtime_error("Database is disconnected");
    }
}
