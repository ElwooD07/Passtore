#pragma once
#include <QStyledItemDelegate>
#include "Storages/IResourceStorage.h"

namespace passtore
{
    class ResourceViewDelegate: public QStyledItemDelegate
    {
    public:
        ResourceViewDelegate(QObject* parent, const ResourcesDefinition& defs);

        QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        void setEditorData(QWidget *editor, const QModelIndex &index) const override;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
        ResourcesDefinition m_defs;
    };
}
