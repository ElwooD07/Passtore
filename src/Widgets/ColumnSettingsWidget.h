#pragma once
#include "ui_ColumnSettingsWidget.h"
#include "Settings.h"

namespace passtore
{
    class ColumnSettingsWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit ColumnSettingsWidget(QWidget *parent, ColumnSettings sets);

        ColumnSettings getSets() const;

    private:
        Ui::ColumnSettingsWidget m_ui;
    };
}
