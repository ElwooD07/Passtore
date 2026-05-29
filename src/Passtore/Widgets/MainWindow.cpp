#include "pch.h"
#include "MainWindow.h"
#include "Widgets/ResourcesTableWidget.h"
#include "Models/ResourceTableModel.h"
#include "Widgets/SettingsDialog.h"
#include "Widgets/ChangePasswordDialog.h"
#include <FL/fl_ask.H>

using namespace passtore;

static const int s_menuH   = 25;
static const int s_statusH = 22;
static const int s_winW    = 722;
static const int s_winH    = 421;

MainWindow::MainWindow(IResourceStorage* storage, const std::filesystem::path& settingsPath)
    : Fl_Window(s_winW, s_winH, PRODUCT_NAME)
    , m_storage(storage)
    , m_settingsPath(settingsPath)
{
    auto defs = m_storage->GetResourcesDefinition();
    m_settings.Load(settingsPath, defs);

    size_range(600, 360);
    begin();

    auto* menu = new Fl_Menu_Bar(0, 0, s_winW, s_menuH);
    menu->add("File/Settings",         0, onSettings,        this);
    menu->add("File/Change Password",  0, onChangePassword,  this);
    menu->add("File/Exit",             0, [](Fl_Widget*, void*){ exit(0); }, nullptr);

    int listH = s_winH - s_menuH - s_statusH;
    m_model = new ResourceTableModel(storage);
    m_model->SetErrorCallback(onError, this);

    m_listWidget = new ResourcesTableWidget(0, s_menuH, s_winW, listH);
    m_listWidget->SetModel(m_model);

    m_statusBar = new Fl_Box(0, s_menuH + listH, s_winW, s_statusH);
    m_statusBar->box(FL_FLAT_BOX);
    m_statusBar->color(FL_BACKGROUND_COLOR);
    m_statusBar->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);

    end();
    resizable(m_listWidget);
}

void MainWindow::onError(void* ctx, const std::string& message)
{
    auto* self = static_cast<MainWindow*>(ctx);
    self->m_statusBar->copy_label(("Error: " + message).c_str());
    fl_alert("Error: %s", message.c_str());
}

void MainWindow::onSettings(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<MainWindow*>(ctx);
    auto defs = self->m_storage->GetResourcesDefinition();
    SettingsDialog dlg(defs, self->m_settings.table);
    dlg.show();
    while (dlg.shown())
    {
        Fl::wait();
    }
    self->m_settings.table = dlg.GetTableSettings();
    self->m_settings.Save(self->m_settingsPath);
}

void MainWindow::onChangePassword(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<MainWindow*>(ctx);
    ChangePasswordDialog dlg(self->m_storage);
    dlg.show();
    while (dlg.shown())
    {
        Fl::wait();
    }
}

