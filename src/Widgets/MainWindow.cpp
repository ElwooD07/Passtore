#include "stdafx.h"
#include "MainWindow.h"
#include "ResourcesListWidget.h"
#include "ResourceWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_ui.setupUi(this);
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(new ResourcesListWidget(splitter));
    splitter->addWidget(new ResourceWidget(splitter));
    splitter->setChildrenCollapsible(false);
    splitter->handle(1)->setStyleSheet("background: #aaaaaa;");
    m_ui.centralWidget->layout()->addWidget(splitter);
}

