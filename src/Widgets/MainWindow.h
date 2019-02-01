#pragma once
#include <QMainWindow>
#include "ui_MainWindow.h"
#include "Database.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent, Database& database);

private:
    Ui::MainWindow m_ui;
};
