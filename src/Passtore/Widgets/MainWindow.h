#pragma once
#include <filesystem>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Box.H>
#include "Settings.h"
#include "Storages/IResourceStorage.h"

namespace passtore
{
    class ResourceTableModel;
    class ResourcesTableWidget;

    /* Top-level application window: menu bar, resource table, and status bar. */
    class MainWindow : public Fl_Window
    {
    public:
        MainWindow(IResourceStorage* storage, const std::filesystem::path& settingsPath);

    private:
        static void onError(void* ctx, const std::string& message);
        static void onSettings(Fl_Widget*, void* ctx);
        static void onChangePassword(Fl_Widget*, void* ctx);

    private:
        IResourceStorage* m_storage;
        ResourceTableModel* m_model;
        ResourcesTableWidget* m_listWidget;
        Fl_Box* m_statusBar;
        Settings m_settings;
        std::filesystem::path m_settingsPath;
    };
}
