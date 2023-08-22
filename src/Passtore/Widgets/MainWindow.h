#pragma once
#include <QMainWindow>
#include "ui_MainWindow.h"
#include "Storages/IStorage.h"

namespace Ui {
    class MainWindow;
}

namespace passtore
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent, IStorage* database);

    public slots:
        void OnErrorOccurred(const QString& message);

    private:
        Ui::MainWindow m_ui;
    };
}
