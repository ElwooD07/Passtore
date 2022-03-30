#include "pch.h"
#include "ResourcesListWidget.h"

passtore::ResourcesListWidget::ResourcesListWidget(QWidget* parent, QAbstractTableModel* model, QStyledItemDelegate* delegate)
    : QWidget(parent)
{
    m_ui.setupUi(this);
    m_ui.listView->setModel(model);
    m_ui.listView->setItemDelegate(delegate);
}
