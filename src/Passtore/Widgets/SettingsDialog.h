#pragma once
#include "ui_SettingsDialog.h"
#include "Settings.h"
#include "Storages/IResourceStorage.h"

namespace passtore
{
    class SettingsDialog : public QDialog
    {
    public:
        SettingsDialog(QWidget* parent, const ResourcesDefinition& defs);

        TableSettings getTableSettings() const;

    private:
        Ui::SettingsDialog m_ui;
        size_t m_columnsCount;
    };
}
