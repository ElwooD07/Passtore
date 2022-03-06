#include "pch.h"
#include "MainWindow.h"
#include "ResourcesListWidget.h"

MainWindow::MainWindow(QWidget* parent, Database& database)
    : QMainWindow(parent)
{
    m_ui.setupUi(this);
    ResourcesModel* model = new ResourcesModel(this, database);
    m_ui.centralWidget->layout()->addWidget(new ResourcesListWidget(this, model));
}

