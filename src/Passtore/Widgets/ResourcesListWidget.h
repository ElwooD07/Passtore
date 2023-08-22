#pragma once
#include "ui_ResourcesListWidget.h"

namespace passtore
{
    class ResourcesListWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit ResourcesListWidget(QWidget* parent, QAbstractTableModel* model, QStyledItemDelegate* delegate);

    private:
        Ui::ResourceListWidget m_ui;
    };
}
