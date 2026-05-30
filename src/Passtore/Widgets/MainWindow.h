#pragma once
#include <filesystem>
#include <memory>
#include "Settings.h"
#include "Storages/IResourceStorage.h"

namespace passtore
{
    class ResourceTableModel;
    class ResourcesTableWidget;

    /* Top-level application window: menu bar, resource table, and status bar. */
    class MainWindow
    {
    public:
        MainWindow(IResourceStorage* storage, const std::filesystem::path& settingsPath);
        ~MainWindow();

        void Show();
        void OnErrorPublic(const std::string& message);

    private:
        void OnError(const std::string& message);
        void OnSettings();
        void OnChangePassword();
        void OnUnload(const nana::arg_unload& arg);

    private:
        IResourceStorage* m_storage;
        std::unique_ptr<ResourceTableModel> m_model;
        std::unique_ptr<ResourcesTableWidget> m_listWidget;
        std::unique_ptr<nana::form> m_window;
        nana::label* m_statusBar = nullptr;
        bool m_isClosing = false;
        Settings m_settings;
        std::filesystem::path m_settingsPath;
    };
}
