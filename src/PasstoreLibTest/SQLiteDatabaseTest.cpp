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

TEST(SQLiteDatabaseTest, ChangePassword_ReopenWithNewPassword)
{
    auto dbPath = std::filesystem::current_path().parent_path() / "test_changepwd.db";
    std::filesystem::remove(dbPath);

    {
        sqlite::SQLiteDatabase db;
        ASSERT_NO_THROW(db.Open(dbPath, "initialPassword"));
        ASSERT_NO_THROW(db.ChangePassword("initialPassword", "newPassword"));
    }

    {
        sqlite::SQLiteDatabase db;
        EXPECT_THROW(db.Open(dbPath, "initialPassword"), std::runtime_error);
    }

    {
        sqlite::SQLiteDatabase db;
        ASSERT_NO_THROW(db.Open(dbPath, "newPassword"));

        // Smoke-check that the vault is usable after password rotation.
        Resource resource;
        resource.subject = "Post rotation write";
        ResourceId id = InvalidResourceId;
        ASSERT_NO_THROW(id = db.Upsert(resource));
        EXPECT_NE(InvalidResourceId, id);
    }
}
