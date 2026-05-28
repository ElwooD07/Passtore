#include "pch.h"
#include "Widgets/MainWindow.h"
#include "Storages/SQLite/SQLiteDatabase.h"

using namespace passtore;

static void SeedFakeData(IResourceStorage* storage)
{
    if (storage->GetResourcesCount() > 0) {
        return;
    }

    auto seed = [&](const char* name, const char* url, const char* login, const char* password, const char* notes)
    {
        Resource r;
        r.id = InvalidResourceId;
        r.subject = name;
        r.values = {
            { "Name",     name,     ResourceValueFlags::Empty },
            { "URL",      url,      ResourceValueFlags::Empty },
            { "Login",    login,    ResourceValueFlags::Empty },
            { "Password", password, ResourceValueFlags::Critical },
            { "Notes",    notes,    ResourceValueFlags::Empty },
        };
        storage->Upsert(r);
    };

    seed("GitHub",        "https://github.com",       "user@example.com",   "hunter2",       "Personal account");
    seed("Gmail",         "https://mail.google.com",  "user@gmail.com",     "P@ssw0rd!",     "");
    seed("Work VPN",      "vpn.company.internal",     "jdoe",               "S3cur3VPN#",    "Cisco AnyConnect");
    seed("Home Router",   "192.168.1.1",              "admin",              "admin1234",     "Change this!");
}

int main(int argc, char* argv[])
{
    try
    {
        std::filesystem::path dbPath = "passtore.db";
        std::filesystem::path settingsPath = "passtore.json";

        sqlite::SQLiteDatabase db;
        db.Open(dbPath, "000");
        SeedFakeData(&db);

        MainWindow w(&db, settingsPath);
        w.show();

        return Fl::run();
    }
    catch (const std::exception& ex)
    {
        fl_alert("Critical error: %s", ex.what());
        return 1;
    }
}
