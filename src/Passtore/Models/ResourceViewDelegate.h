#pragma once
#include <QStyledItemDelegate>
#include "Storages/IResourceStorage.h"

namespace passtore
{
    class ResourceViewDelegate: public QStyledItemDelegate
    {
    public:
        ResourceViewDelegate(QObject* parent, IResourceStorage& storage);

        QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        void setEditorData(QWidget *editor, const QModelIndex &index) const override;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
        IResourceStorage& m_storage;
}
