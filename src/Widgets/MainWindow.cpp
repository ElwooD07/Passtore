#include "pch.h"
#include "MainWindow.h"
#include "ResourcesListWidget.h"
#include "Models/ResourceTableModel.h"
#include "Models/ResourceViewDelegate.h"

passtore::MainWindow::MainWindow(QWidget* parent, IStorage* storage)
    : QMainWindow(parent)
{
    m_ui.setupUi(this);
    auto* model = new ResourceTableModel(this, storage);
    ResourcesDefinition defs;
    storage->GetResourcesDefinition(defs);
    auto* delegate = new ResourceViewDelegate(this, defs);

    m_ui.centralWidget->layout()->addWidget(new ResourcesListWidget(this, model, delegate));
}

