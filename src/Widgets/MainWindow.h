#pragma once
#include <QMainWindow>
#include "ui_MainWindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent);

private:
    Ui::MainWindow m_ui;
};
