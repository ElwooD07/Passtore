#include <pch.h>
#include "ColumnSettingsWidget.h"

ColumnSettingsWidget::ColumnSettingsWidget(QWidget* parent, ColumnSettings sets)
    : QWidget(parent)
{
    m_ui.setupUi(this);
    m_ui.lblName->setText(sets.name);
    m_ui.chkVisible->setChecked(sets.visible);
    m_ui.chkBlured->setChecked(sets.blured);
}

ColumnSettings ColumnSettingsWidget::getSets() const
{
    return ColumnSettings { m_ui.lblName->text(), m_ui.chkVisible->isChecked(), m_ui.chkBlured->isChecked() };
}
