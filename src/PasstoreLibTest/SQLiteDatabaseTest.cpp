#include "pch.h"
#include "Storages/SQLite/SQLiteDatabase.h"

using namespace passtore;

TEST(SQLiteDatabaseTest, UpsertTest_Insert)
{
    auto dbPath = std::filesystem::current_path().parent_path() / "test.db";
    std::filesystem::remove(dbPath);

    sqlite::SQLiteDatabase db;
    ASSERT_NO_THROW(db.Open(dbPath, "abcd"));

    Resource resource;
    resource.subject = "Test Subject";
    ResourceId id = 0;
    ASSERT_NO_THROW(id = db.Upsert(resource));

    EXPECT_EQ(1, id);
}
