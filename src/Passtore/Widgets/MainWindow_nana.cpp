#include "pch.h"
#include "MainWindow.h"
#include "ResourcesTableWidget.h"
#include "Models/ResourceTableModel.h"
#include "../Settings.h"

using namespace passtore;

static void ErrorCallbackAdapter(void* ctx, const std::string& message)
{
    auto* self = static_cast<MainWindow*>(ctx);
    self->OnErrorPublic(message);
}

MainWindow::MainWindow(IResourceStorage* storage, const std::filesystem::path& settingsPath)
    : m_storage(storage)
    , m_settingsPath(settingsPath)
{
    auto defs = m_storage->GetResourcesDefinition();
    m_settings.Load(settingsPath, defs);

    m_window = std::make_unique<nana::form>(nana::API::make_center(900, 560));
    m_window->caption(PRODUCT_NAME);
    m_window->events().unload([this](const nana::arg_unload& arg)
    {
        OnUnload(arg);
    });

    m_model = std::make_unique<ResourceTableModel>(storage);
    m_model->SetErrorCallback(ErrorCallbackAdapter, this);

    /* Menu bar */
    auto menuBar = new nana::menubar(*m_window);
    auto& fileMenu = menuBar->push_back("File");
    fileMenu.append("Settings", [this](const nana::menu::item_proxy&)
    {
        OnSettings();
    });
    fileMenu.append("Change Password", [this](const nana::menu::item_proxy&)
    {
        OnChangePassword();
    });
    fileMenu.append("Exit", [this](const nana::menu::item_proxy&)
    {
        m_window->close();
    });

    /* Resource list widget */
    m_listWidget = std::make_unique<ResourcesTableWidget>(*m_window, 0, 30, 900, 480);
    m_listWidget->SetModel(m_model.get());

    /* Status bar */
    m_statusBar = new nana::label(*m_window, nana::rectangle{ 0u, 510u, 900u, 50u });
}

MainWindow::~MainWindow() = default;

void MainWindow::Show()
{
    m_window->show();
}

void MainWindow::OnUnload(const nana::arg_unload&)
{
    if (m_isClosing)
    {
        return;
    }

    m_isClosing = true;
    if (m_listWidget)
    {
        m_listWidget->BeginShutdown();
    }
}

void MainWindow::OnErrorPublic(const std::string& message)
{
    if (m_isClosing)
    {
        return;
    }

    if (m_statusBar)
    {
        m_statusBar->caption("Error: " + message);
    }

    nana::msgbox errorBox(*m_window, "Error");
    errorBox << message;
    errorBox.show();
}

void MainWindow::OnSettings()
{
    if (m_isClosing)
    {
        return;
    }

    // TODO: Implement settings dialog
    nana::msgbox infoBox(*m_window, "Settings");
    infoBox << "Settings dialog coming soon";
    infoBox.show();
}

void MainWindow::OnChangePassword()
{
    if (m_isClosing)
    {
        return;
    }

    // TODO: Implement change password dialog
    nana::msgbox infoBox(*m_window, "Change Password");
    infoBox << "Change password dialog coming soon";
    infoBox.show();
}
