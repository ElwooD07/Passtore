#include "MainWindow.h"
#include <QApplication>
#include "Database.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    try
    {
        Database db;
        db.Open("test", "000");

        MainWindow w(nullptr);
        w.show();

        return a.exec();
    }
    catch(const std::exception& ex)
    {
        std::string error = ex.what();
        QMessageBox::critical(nullptr, "Critical error", error.c_str());
    }
}
