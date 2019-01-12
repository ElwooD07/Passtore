#include "MainWindow.h"
#include "ResourcesListWidget.h"
#include "ResourceWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_ui.setupUi(this);
    m_ui.centralWidget->layout()->addWidget(new ResourcesListWidget(this));
    m_ui.centralWidget->layout()->addWidget(new ResourceWidget(this));
}

