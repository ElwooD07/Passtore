#include "pch.h"
#include "Storages/TableSettings.h"

using namespace passtore;

namespace
{
    ResourcesDefinition MakeDefs(std::initializer_list<std::pair<const char*, bool>> cols)
    {
        ResourcesDefinition defs;
        for (const auto& [name, big] : cols)
        {
            defs.push_back({ name, big });
        }
        return defs;
    }
}

TEST(TableSettingsTest, FromDefinition_AllVisibleByDefault)
{
    auto defs = MakeDefs({ {"Name", false}, {"Password", true}, {"Notes", false} });
    auto ts = TableSettings::FromDefinition(defs);

    ASSERT_EQ(3u, ts.columns.size());
    EXPECT_EQ("Name",     ts.columns[0].name);
    EXPECT_TRUE(ts.columns[0].visible);
    EXPECT_FALSE(ts.columns[0].blured);

    EXPECT_EQ("Password", ts.columns[1].name);
    EXPECT_TRUE(ts.columns[1].visible);
    EXPECT_TRUE(ts.columns[1].blured);   // big=true → blured=true

    EXPECT_EQ("Notes",    ts.columns[2].name);
    EXPECT_TRUE(ts.columns[2].visible);
    EXPECT_FALSE(ts.columns[2].blured);
}

TEST(TableSettingsTest, MergeWithSaved_PreservesSavedValues)
{
    auto defs = MakeDefs({ {"Name", false}, {"URL", false}, {"Password", true} });

    TableSettings saved;
    saved.columns = {
        { "Name",     false, false },
        { "URL",      false, false },
        { "Password", false, false },  // user turned off blur
    };

    auto ts = TableSettings::MergeWithSaved(defs, saved);

    ASSERT_EQ(3u, ts.columns.size());
    EXPECT_FALSE(ts.columns[2].blured);   // saved value preserved
    EXPECT_FALSE(ts.columns[2].visible);  // saved value preserved
}

TEST(TableSettingsTest, MergeWithSaved_NewColumnGetsDefault)
{
    auto defs = MakeDefs({ {"Name", false}, {"Login", false} });

    TableSettings saved;
    saved.columns = {
        { "Name", true, false },
        // "Login" missing from saved
    };

    auto ts = TableSettings::MergeWithSaved(defs, saved);

    ASSERT_EQ(2u, ts.columns.size());
    EXPECT_EQ("Login", ts.columns[1].name);
    EXPECT_TRUE(ts.columns[1].visible);   // default
    EXPECT_FALSE(ts.columns[1].blured);   // default
}

TEST(TableSettingsTest, MergeWithSaved_DropsRemovedColumns)
{
    auto defs = MakeDefs({ {"Name", false} });

    TableSettings saved;
    saved.columns = {
        { "Name",     true, false },
        { "OldField", true, false },  // removed from DB
    };

    auto ts = TableSettings::MergeWithSaved(defs, saved);

    ASSERT_EQ(1u, ts.columns.size());
    EXPECT_EQ("Name", ts.columns[0].name);
}

TEST(TableSettingsTest, MergeWithSaved_EmptySavedEqualsFromDefinition)
{
    auto defs = MakeDefs({ {"Name", false}, {"Password", true} });
    TableSettings empty;

    auto merged  = TableSettings::MergeWithSaved(defs, empty);
    auto defaults = TableSettings::FromDefinition(defs);

    ASSERT_EQ(defaults.columns.size(), merged.columns.size());
    for (size_t i = 0; i < defaults.columns.size(); ++i)
    {
        EXPECT_EQ(defaults.columns[i].name,    merged.columns[i].name);
        EXPECT_EQ(defaults.columns[i].visible, merged.columns[i].visible);
        EXPECT_EQ(defaults.columns[i].blured,  merged.columns[i].blured);
    }
}
