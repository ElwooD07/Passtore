#include "stdafx.h"
#include "ResourcesListWidget.h"
#include "ResourceViewDelegate.h"

ResourcesListWidget::ResourcesListWidget(QWidget* parent, ResourcesModel* model)
    : QWidget(parent)
{
    m_ui.setupUi(this);
    m_ui.listView->setModel(model);
    m_ui.listView->setItemDelegate(new ResourceViewDelegate(this));
}
