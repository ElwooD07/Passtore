#pragma once
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <vector>
#include "Settings.h"
#include "Storages/IResourceStorage.h"
#include "Widgets/ColumnSettingsWidget.h"

namespace passtore
{
    class SettingsDialog : public Fl_Window
    {
    public:
        explicit SettingsDialog(const ResourcesDefinition& defs);
        TableSettings getTableSettings() const;

    private:
        static void onSave(Fl_Widget*, void* ctx);
        static void onClose(Fl_Widget*, void* ctx);

    private:
        std::vector<ColumnSettingsWidget*> m_columnWidgets;
    };
}
