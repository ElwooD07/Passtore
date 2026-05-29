#include "pch.h"
#include "Widgets/MainWindow.h"
#include "Storages/SQLite/SQLiteDatabase.h"

using namespace passtore;

static void SeedFakeData(IResourceStorage* storage)
{
    Resource probe;
    if (storage->GetNext(InvalidResourceId, probe) == ResourceState::Present)
    {
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

    seed("GitHub",           "https://github.com",            "user@example.com",     "hunter2",         "Personal account");
    seed("Gmail",            "https://mail.google.com",       "user@gmail.com",       "P@ssw0rd!",       "");
    seed("Work VPN",         "vpn.company.internal",          "jdoe",                 "S3cur3VPN#",      "Cisco AnyConnect");
    seed("Home Router",      "192.168.1.1",                   "admin",                "admin1234",       "Change this!");
    seed("AWS Console",      "https://console.aws.amazon.com","john.doe@company.com", "Amz!2024xK9",     "eu-west-1 prod");
    seed("Namecheap",        "https://www.namecheap.com",     "jdoe_domains",         "Nc$Reg77!",       "domain registrar");
    seed("Cloudflare",       "https://dash.cloudflare.com",   "user@example.com",     "Cf#Zone2024",     "DNS + CDN");
    seed("Digital Ocean",    "https://cloud.digitalocean.com","user@example.com",     "D0!Droplet9",     "staging droplet");
    seed("Jira",             "https://company.atlassian.net", "jdoe",                 "Jir@Work!1",      "project tracker");
    seed("Slack",            "https://company.slack.com",     "jdoe",                 "Slk#Chat22",      "");
    seed("Figma",            "https://figma.com",             "user@example.com",     "Fig!Design3",     "design team");
    seed("Steam",            "https://store.steampowered.com","gamer_handle",         "Stm$G4mer!",      "");
    seed("Bitwarden",        "https://vault.bitwarden.com",   "user@example.com",     "Bw!Vault88",      "backup vault");
    seed("1Password",        "https://my.1password.com",      "user@example.com",     "1P@ss#Gold",      "family plan");
    seed("LinkedIn",         "https://linkedin.com",          "john.doe@example.com", "Lnkd!Prof2",      "");
    seed("Twitter/X",        "https://x.com",                 "jdoe_x",               "Tw!tX2024$",      "");
    seed("Office 365",       "https://portal.office.com",     "jdoe@company.com",     "O365!Work#9",     "work tenant");
    seed("Azure DevOps",     "https://dev.azure.com/company", "jdoe@company.com",     "Az!Dev0ps#3",     "CI/CD pipelines");
    seed("Docker Hub",       "https://hub.docker.com",        "jdoe_docker",          "Dkr!Hub$55",      "");
    seed("npm registry",     "https://npmjs.com",             "jdoe_npm",             "Npm!Pkg@7",       "");
}

int main(int argc, char* argv[])
{
    try
    {
        // Step 13: neutral palette — light gray background, near-white inputs
        Fl::scheme("gtk+");
        Fl::background(235, 235, 235);
        Fl::background2(252, 252, 252);
        Fl::foreground(20, 20, 20);

        std::filesystem::path dbPath = "passtore.db";
        std::filesystem::path settingsPath = "passtore.json";

        sqlite::SQLiteDatabase db;
        db.Open(dbPath.string(), "000");
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
