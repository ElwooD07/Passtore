#include "pch.h"
#include "SettingsDialog.h"
#include "Resource.h"
#include "ColumnSettingsWidget.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);

    for (size_t i = ResourcePropertyFirst; i < ResourcePropertyCount; ++i)
    {
        auto columnSets = ColumnSettings {
                Resource::PropertyNameTranslated(static_cast<ResourceProperty>(i)),
                true,
                false
        };

        auto columnSetsWidget = new ColumnSettingsWidget(this, columnSets);
        m_ui.layoutTable->addWidget(columnSetsWidget);
    }

    m_ui.layoutTable->addItem(new QSpacerItem(1, 1, QSizePolicy::Ignored, QSizePolicy::Expanding));
}

TableSettings SettingsDialog::getTableSettings() const
{
    TableSettings sets;
    for (size_t i = ResourcePropertyFirst; i < ResourcePropertyCount; ++i)
    {
        const auto columnSetsWidget = static_cast<ColumnSettingsWidget*>(m_ui.layoutTable->itemAt(i)->widget());
        sets.columns.insert(static_cast<ResourceProperty>(i), columnSetsWidget->getSets());
    }

    return sets;
}
