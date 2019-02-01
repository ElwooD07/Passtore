#include "ResourcesListWidget.h"

ResourcesListWidget::ResourcesListWidget(QWidget* parent, ResourcesModel* model)
    : QWidget(parent)
{
    m_ui.setupUi(this);
    m_ui.listView->setModel(model);
}
