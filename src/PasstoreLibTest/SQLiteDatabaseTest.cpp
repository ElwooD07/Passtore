#include "pch.h"
#include "Storages/SQLite/SQLiteDatabase.h"

using namespace passtore;

namespace
{
    Resource MakeResource(
        const std::string& subject,
        const std::string& name     = {},
        const std::string& url      = {},
        const std::string& login    = {},
        const std::string& password = {},
        const std::string& notes    = {})
    {
        Resource r;
        r.subject = subject;
        r.values = {
            { "Name",     name     },
            { "URL",      url      },
            { "Login",    login    },
            { "Password", password },
            { "Notes",    notes    },
        };
        return r;
    }
}

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

TEST(SQLiteDatabaseTest, Upsert_GetOne_RoundTrip)
{
    auto dbPath = std::filesystem::current_path().parent_path() / "test_roundtrip.db";
    std::filesystem::remove(dbPath);

    sqlite::SQLiteDatabase db;
    ASSERT_NO_THROW(db.Open(dbPath, "testpass"));

    ResourceId id = InvalidResourceId;
    ASSERT_NO_THROW(id = db.Upsert(MakeResource("Hello World")));
    ASSERT_NE(InvalidResourceId, id);

    Resource loaded;
    ASSERT_EQ(ResourceState::Present, db.GetOne(id, loaded));
    EXPECT_EQ("Hello World", loaded.subject);
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

TEST(SQLiteDatabaseTest, DeleteResource_RemovesFromDB)
{
    auto dbPath = std::filesystem::current_path().parent_path() / "test_delete.db";
    std::filesystem::remove(dbPath);

    sqlite::SQLiteDatabase db;
    ASSERT_NO_THROW(db.Open(dbPath, "testpass"));

    ResourceId id = InvalidResourceId;
    ASSERT_NO_THROW(id = db.Upsert(MakeResource("To be deleted")));
    ASSERT_NE(InvalidResourceId, id);
    EXPECT_EQ(1, db.GetResourcesCount());

    ASSERT_NO_THROW(db.DeleteResource(id));
    EXPECT_EQ(0, db.GetResourcesCount());

    Resource loaded;
    EXPECT_EQ(ResourceState::Deleted, db.GetOne(id, loaded));
}

TEST(SQLiteDatabaseTest, DeleteResource_PersistsAcrossReopen)
{
    auto dbPath = std::filesystem::current_path().parent_path() / "test_delete_reopen.db";
    std::filesystem::remove(dbPath);

    ResourceId id = InvalidResourceId;
    {
        sqlite::SQLiteDatabase db;
        ASSERT_NO_THROW(db.Open(dbPath, "testpass"));
        ASSERT_NO_THROW(id = db.Upsert(MakeResource("Ephemeral")));
        ASSERT_NE(InvalidResourceId, id);
        ASSERT_NO_THROW(db.DeleteResource(id));
    }

    {
        sqlite::SQLiteDatabase db;
        ASSERT_NO_THROW(db.Open(dbPath, "testpass"));
        EXPECT_EQ(0, db.GetResourcesCount());
        Resource loaded;
        EXPECT_EQ(ResourceState::Deleted, db.GetOne(id, loaded));
    }
}

TEST(SQLiteDatabaseTest, Swap_SameId_IsNoOp)
{
    auto dbPath = std::filesystem::current_path().parent_path() / "test_swap_sameid.db";
    std::filesystem::remove(dbPath);

    sqlite::SQLiteDatabase db;
    ASSERT_NO_THROW(db.Open(dbPath, "testpass"));

    ResourceId id = InvalidResourceId;
    ASSERT_NO_THROW(id = db.Upsert(MakeResource("Solo")));

    ASSERT_NO_THROW(db.Swap(id, id));

    Resource loaded;
    ASSERT_EQ(ResourceState::Present, db.GetOne(id, loaded));
    EXPECT_EQ("Solo", loaded.subject);
}

TEST(SQLiteDatabaseTest, Swap_ExchangesData)
{
    auto dbPath = std::filesystem::current_path().parent_path() / "test_swap.db";
    std::filesystem::remove(dbPath);

    sqlite::SQLiteDatabase db;
    ASSERT_NO_THROW(db.Open(dbPath, "testpass"));

    ResourceId id1 = InvalidResourceId;
    ResourceId id2 = InvalidResourceId;
    ASSERT_NO_THROW(id1 = db.Upsert(MakeResource("First")));
    ASSERT_NO_THROW(id2 = db.Upsert(MakeResource("Second")));
    ASSERT_NE(InvalidResourceId, id1);
    ASSERT_NE(InvalidResourceId, id2);

    ASSERT_NO_THROW(db.Swap(id1, id2));

    Resource loaded1;
    ASSERT_EQ(ResourceState::Present, db.GetOne(id1, loaded1));
    EXPECT_EQ("Second", loaded1.subject);

    Resource loaded2;
    ASSERT_EQ(ResourceState::Present, db.GetOne(id2, loaded2));
    EXPECT_EQ("First", loaded2.subject);
}

TEST(SQLiteDatabaseTest, Swap_PersistsAcrossReopen)
{
    auto dbPath = std::filesystem::current_path().parent_path() / "test_swap_reopen.db";
    std::filesystem::remove(dbPath);

    ResourceId id1 = InvalidResourceId;
    ResourceId id2 = InvalidResourceId;
    {
        sqlite::SQLiteDatabase db;
        ASSERT_NO_THROW(db.Open(dbPath, "testpass"));
        ASSERT_NO_THROW(id1 = db.Upsert(MakeResource("Alpha")));
        ASSERT_NO_THROW(id2 = db.Upsert(MakeResource("Beta")));
        ASSERT_NO_THROW(db.Swap(id1, id2));
    }

    {
        sqlite::SQLiteDatabase db;
        ASSERT_NO_THROW(db.Open(dbPath, "testpass"));

        Resource loaded1;
        ASSERT_EQ(ResourceState::Present, db.GetOne(id1, loaded1));
        EXPECT_EQ("Beta", loaded1.subject);

        Resource loaded2;
        ASSERT_EQ(ResourceState::Present, db.GetOne(id2, loaded2));
        EXPECT_EQ("Alpha", loaded2.subject);
    }
}
