#include "pch.h"
#include "MainWindow.h"
#include "Widgets/ResourcesListWidget.h"
#include "Models/ResourceTableModel.h"
#include "Widgets/SettingsDialog.h"
#include <FL/fl_ask.H>

using namespace passtore;

static const int MENU_H = 25;
static const int BTN_H  = 28;
static const int WIN_W  = 722;
static const int WIN_H  = 421;

MainWindow::MainWindow(IResourceStorage* storage)
    : Fl_Window(WIN_W, WIN_H, PRODUCT_NAME)
    , m_storage(storage)
{
    size_range(600, 360);
    begin();

    auto* menu = new Fl_Menu_Bar(0, 0, WIN_W, MENU_H);
    menu->add("File/Settings", 0, onSettings, this);
    menu->add("File/Exit",     0, [](Fl_Widget*, void*){ exit(0); }, nullptr);

    int tableH = WIN_H - MENU_H - BTN_H - 6;
    m_model = new ResourceTableModel(storage);
    m_model->setErrorCallback(onError, this);

    m_table = new ResourcesListWidget(0, MENU_H, WIN_W, tableH, m_model);

    auto* btnDelete = new Fl_Button(0, MENU_H + tableH + 4, 80, BTN_H, "Delete");
    btnDelete->callback(onDelete, this);

    auto* btnAdd = new Fl_Button(WIN_W - 82, MENU_H + tableH + 4, 80, BTN_H, "Add");
    btnAdd->callback(onAdd, this);

    end();
    resizable(m_table);
}

void MainWindow::onError(void* /*ctx*/, const std::string& message)
{
    fl_alert("Error: %s", message.c_str());
}

void MainWindow::onSettings(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<MainWindow*>(ctx);
    auto defs = self->m_storage->GetResourcesDefinition();
    SettingsDialog dlg(defs);
    dlg.show();
    while (dlg.shown()) Fl::wait();
}

void MainWindow::onAdd(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<MainWindow*>(ctx);
    self->m_model->addRow();
    self->m_table->refresh();
}

void MainWindow::onDelete(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<MainWindow*>(ctx);
    int row = self->m_table->callback_row();
    if (row < 0) return;
    self->m_model->deleteRow(row);
    self->m_table->refresh();
}

