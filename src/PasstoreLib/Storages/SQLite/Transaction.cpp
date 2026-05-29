#include "pch.h"
#include "Transaction.h"
#include "Connection.h"

using namespace passtore;

void sqlite::detail::CreateNewSavepointName(std::string& name)
{
    static const char s_minLetter = 'a';
    static const char s_maxLetter = 'z';

    if (name.empty())
    {
        name = s_minLetter;
    }
    else
    {
        char& lastLetter = name.back();
        if (name.back() != s_maxLetter)
        {
            ++lastLetter;
        }
        else
        {
            name += s_minLetter;
        }
    }
}

sqlite::TransactionsResources::TransactionsResources(Connection* connection)
    : m_connection(connection)
{ }

sqlite::Connection* sqlite::TransactionsResources::GetConnection()
{
    return m_connection;
}

std::string sqlite::TransactionsResources::NextTransactionName()
{
    std::unique_lock<std::mutex> lock(m_changeNameMutex);
    detail::CreateNewSavepointName(m_lastTransactionName);
    return m_lastTransactionName;
}

sqlite::Transaction::Transaction(TransactionsResources& resources)
    : m_resources(resources)
    , m_committed(false)
{
    m_transactionName = m_resources.NextTransactionName();
    m_resources.GetConnection()->ExecQuery("SAVEPOINT " + m_transactionName + ";");
}

sqlite::Transaction::~Transaction()
{
    try
    {
        if (!m_committed)
        {
            m_resources.GetConnection()->ExecQuery("ROLLBACK TO SAVEPOINT " + m_transactionName + ";");
        }
    }
    catch(const std::exception& ex)
    {
        LOGE << "Failed to rollback transaction '" << m_transactionName << "': " << ex.what();
    }
}

void sqlite::Transaction::Commit()
{
    m_resources.GetConnection()->ExecQuery("RELEASE SAVEPOINT " + m_transactionName + ";");
    m_committed = true;
}
