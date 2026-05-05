#include "pch.h"
#include "Widgets/MainWindow.h"
#include "Storages/SQLite/SQLiteDatabase.h"

using namespace passtore;

int main(int argc, char* argv[])
{
    try
    {
        sqlite::SQLiteDatabase db;
        db.Open("passtore.db", "000");

        MainWindow w(&db);
        w.show();

        return Fl::run();
    }
    catch (const std::exception& ex)
    {
        fl_alert("Critical error: %s", ex.what());
        return 1;
    }
}
