#include "stdafx.h"
#include "MainWindow.h"
#include "ResourcesListWidget.h"
#include "ResourceWidget.h"

MainWindow::MainWindow(QWidget* parent, Database& database)
    : QMainWindow(parent)
{
    m_ui.setupUi(this);
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    ResourcesModel* model = new ResourcesModel(this, database);
    m_ui.centralWidget->layout()->addWidget(new ResourcesListWidget(splitter, model));
}

