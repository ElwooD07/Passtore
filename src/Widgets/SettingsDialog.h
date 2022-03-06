#pragma once
#include "ui_SettingsDialog.h"
#include "Settings.h"

class SettingsDialog : public QDialog
{
public:
    SettingsDialog(QWidget* parent);

    TableSettings getTableSettings() const;

private:
    Ui::SettingsDialog m_ui;
};
