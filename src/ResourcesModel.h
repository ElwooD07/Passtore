#pragma once
#include <QAbstractTableModel>
#include "Database.h"
#include "Resource.h"

class ResourcesModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ResourcesModel(QObject* parent, Database& database);
    ResourcesModel(const ResourcesModel&) = delete;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags( const QModelIndex& index) const override;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

signals:
    void ErrorOccurred(QString text) const;

private:
    Database& m_db;
    ResourceDefinitions m_defs;
};
