#pragma once
#include <QMainWindow>
#include "ui_MainWindow.h"
#include "Storages/IResourceStorage.h"

namespace Ui {
    class MainWindow;
}

namespace passtore
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent, IResourceStorage* database);

    public slots:
        void OnErrorOccurred(const QString& message);

    private:
        Ui::MainWindow m_ui;
    };
}
