#pragma once
#include <QSortFilterProxyModel>
#include "Storages/IStorage.h"
#include "Settings.h"
#include "Cache.h"

namespace passtore
{
    class ResourceTableModel: public QAbstractTableModel
    {
        Q_OBJECT

    public:
        ResourceTableModel(QObject* parent, IStorage* storage);
        ResourceTableModel(const ResourceTableModel&) = delete;

        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

        Qt::ItemFlags flags(const QModelIndex& index) const override;

        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    signals:
        void ErrorOccurred(QString text) const;

    private:
        // The returned Resource is editable, it can be explicitly saved to the storage by caller.
        // Returns nullptr if Resource with given id is not found.
        Resource* GetResource(int id) const;
        bool IndexIsValid(const QModelIndex& idx) const;

    private:
        IStorage* m_storage = nullptr;
        ResourcesDefinition m_resourcesDefs;
        QVector<bool> m_columnsVisibility;
        TableSettings m_sets;
        mutable Cache<int, Resource> m_cache;
    };
}
