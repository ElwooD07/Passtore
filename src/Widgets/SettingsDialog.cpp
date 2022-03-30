#include "pch.h"
#include "SettingsDialog.h"
#include "ColumnSettingsWidget.h"

passtore::SettingsDialog::SettingsDialog(QWidget *parent, const ResourcesDefinition& defs)
    : QDialog(parent)
{
    m_ui.setupUi(this);

    for (int i = 0; i < defs.size(); ++i)
    {
        auto columnSets = ColumnSettings {
                defs.at(i).name,
                true,
                false
        };

        auto columnSetsWidget = new ColumnSettingsWidget(this, columnSets);
        m_ui.layoutTable->addWidget(columnSetsWidget);
    }

    m_ui.layoutTable->addItem(new QSpacerItem(1, 1, QSizePolicy::Ignored, QSizePolicy::Expanding));

    m_columnsCount = defs.size();
}

passtore::TableSettings passtore::SettingsDialog::getTableSettings() const
{
    TableSettings sets;
    for (int i = 0; i < m_columnsCount; ++i)
    {
        const auto columnSetsWidget = static_cast<ColumnSettingsWidget*>(m_ui.layoutTable->itemAt(i)->widget());
        sets.columns.push_back(columnSetsWidget->getSets());
    }

    return sets;
}
