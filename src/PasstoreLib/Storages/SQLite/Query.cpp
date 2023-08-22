#include "pch.h"
#include "Storages/SQLite/Query.h"

using namespace passtore;

sqlite::Query::Query(sqlite3* db, const std::string& query)
    : m_db(db)
{
    assert(m_db != nullptr);

    Prepare(query);
}

sqlite::Query::Query(Query&& other)
{
    *this = std::move(other);
}

sqlite::Query& sqlite::Query::operator=(Query&& other)
{
    m_db = other.m_db;
    m_stmt = other.m_stmt;
    m_lastRowId = other.m_lastRowId;

    other.m_db = nullptr;
    other.m_stmt = nullptr;
    other.m_lastRowId = 0;

    return *this;
}

sqlite::Query::~Query()
{
    if (m_stmt)
    {
        auto res = sqlite3_finalize(m_stmt);
        if (res != SQLITE_OK)
        {
            LOGE << "Failed to finalyze statement: " << res;
        }
    }
}

void sqlite::Query::Prepare(const std::string& query)
{
    if (!m_db)
    {
        throw std::runtime_error("Database is disconnected");
    }

    if (m_stmt)
    {
        sqlite3_finalize(m_stmt);
    }

    if (!query.empty())
    {
        int err = sqlite3_prepare_v2(m_db, query.c_str(), static_cast<int>(query.size()), &m_stmt, 0);
        DecideToThrow(err);
    }
}

void sqlite::Query::BindBool(int column, bool value)
{
    CheckSTMT();
    DecideToThrow(sqlite3_bind_int(m_stmt, column, value ? 1 : 0));
}

void sqlite::Query::BindInt(int column, int value)
{
    CheckSTMT();
    DecideToThrow(sqlite3_bind_int(m_stmt, column, value));
}

void sqlite::Query::BindInt64(int column, int64_t value)
{
    CheckSTMT();
    DecideToThrow(sqlite3_bind_int64(m_stmt, column, value));
}

void sqlite::Query::BindReal(int column, double value)
{
    CheckSTMT();
    DecideToThrow(sqlite3_bind_double(m_stmt, column, value));
}

void sqlite::Query::BindText(int column, const std::string& value)
{
    CheckSTMT();
    DecideToThrow(sqlite3_bind_text(m_stmt, column, value.c_str(), static_cast<int>(value.length() * sizeof(char)), nullptr));
}

void sqlite::Query::BindBlob(int column, const BlobData& data)
{
    CheckSTMT();
    BindBlob(column, reinterpret_cast<const char*>(data.data()), data.size());
}

void sqlite::Query::BindBlob(int column, const char* data, size_t size)
{
    CheckSTMT();
    DecideToThrow(sqlite3_bind_blob(m_stmt, column, data, static_cast<int>(size), nullptr));
}

bool sqlite::Query::Step()
{
    CheckSTMT();
    int err = sqlite3_step(m_stmt);
    m_lastRowId = sqlite3_last_insert_rowid(m_db);
    DecideToThrow(err);

    return err == SQLITE_ROW;
}

void sqlite::Query::Reset()
{
    if (m_stmt)
    {
        DecideToThrow(sqlite3_reset(m_stmt));
    }
}

bool sqlite::Query::ColumnBool(int column)
{
    CheckSTMT();
    if (sqlite3_column_int(m_stmt, column) == 0)
    {
        return false;
    }
    return true;
}

int sqlite::Query::ColumnInt(int column)
{
    CheckSTMT();
    return sqlite3_column_int(m_stmt, column);
}

int64_t sqlite::Query::ColumnInt64(int column)
{
    CheckSTMT();
    return sqlite3_column_int64(m_stmt, column);
}

double sqlite::Query::ColumnReal(int column)
{
    CheckSTMT();
    return sqlite3_column_double(m_stmt, column);
}

void sqlite::Query::ColumnText(int column, std::string& out)
{
    CheckSTMT();

    const char* textRef = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, column));
    if (textRef)
    {
        out = textRef;
    }
    else
    {
        out.clear();
    }
}

void sqlite::Query::ColumnBlob(int column, BlobData& data)
{
    CheckSTMT();

    const char* textRef = static_cast<const char *>(sqlite3_column_blob(m_stmt, column));
    data.clear();
    if (textRef != nullptr)
    {
        data.assign(textRef, textRef + sqlite3_column_bytes(m_stmt, column));
    }
}

int64_t sqlite::Query::LastRowId()
{
    return m_lastRowId;
}

void sqlite::Query::CheckSTMT()
{
    if (!m_stmt)
    {
        throw std::runtime_error("SQLite statement is not prepared");
    }
}

void sqlite::Query::DecideToThrow(int errCode)
{
    if (errCode != SQLITE_OK && errCode != SQLITE_ROW && errCode != SQLITE_DONE)
    {
        throw std::runtime_error(sqlite3_errmsg(m_db));
    }
}
