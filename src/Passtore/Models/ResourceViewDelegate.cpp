#include "pch.h"
#include "Models/ResourceViewDelegate.h"
#include "Models/ResourceTableModelRoles.h"

using namespace passtore;

ResourceViewDelegate::ResourceViewDelegate(QObject* parent, const ResourcesDefinition& defs)
    : QStyledItemDelegate(parent)
    , m_defs(defs)
{ }

QWidget* ResourceViewDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const
{
    if (index.data(ResourceTableModelRole::IsBigColumn).toBool())
    {
        return new QTextEdit(parent);
    }
    return new QLineEdit(parent);
}

void ResourceViewDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto data = index.data(Qt::DisplayRole).toString();
    if (index.data(ResourceTableModelRole::IsBigColumn).toBool())
    {
        static_cast<QTextEdit*>(editor)->setText(data);
    }
    else
    {
        static_cast<QLineEdit*>(editor)->setText(data);
    }
}

void ResourceViewDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QString data;
    if (index.data(ResourceTableModelRole::IsBigColumn).toBool())
    {
        data = static_cast<QTextEdit*>(editor)->toPlainText();
    }
    else
    {
        data = static_cast<QLineEdit*>(editor)->text();
    }
    model->setData(index, data);
}

void ResourceViewDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
    editor->setGeometry(option.rect);
}
