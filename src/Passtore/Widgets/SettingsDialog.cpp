#include "pch.h"
#include "SettingsDialog.h"

using namespace passtore;

static const int DLG_W   = 383;
static const int TAB_TOP = 30;
static const int ROW_H   = 32;
static const int BTN_H   = 28;

SettingsDialog::SettingsDialog(const ResourcesDefinition& defs)
    : Fl_Window(DLG_W, 100, "Settings")
{
    int scrollH = std::min(static_cast<int>(defs.size()) * ROW_H + 10, 260);
    int dlgH = TAB_TOP + 10 + scrollH + 40 + BTN_H + 10;
    resize(x(), y(), DLG_W, dlgH);

    begin();
    auto* tabs = new Fl_Tabs(10, 10, DLG_W - 20, dlgH - BTN_H - 30);
    {
        tabs->begin();
        auto* tabTable = new Fl_Group(10, TAB_TOP, DLG_W - 20, dlgH - BTN_H - 30 - TAB_TOP, "Table");
        {
            tabTable->begin();
            auto* scroll = new Fl_Scroll(10, TAB_TOP + 5, DLG_W - 20, dlgH - BTN_H - 30 - TAB_TOP - 10);
            scroll->begin();
            for (size_t i = 0; i < defs.size(); ++i) {
                ColumnSettings cs{ defs[i].name, true, false };
                auto* w = new ColumnSettingsWidget(10, TAB_TOP + 5 + static_cast<int>(i) * ROW_H, DLG_W - 36, cs);
                m_columnWidgets.push_back(w);
            }
            scroll->end();
            tabTable->end();
        }
        tabTable->box(FL_FLAT_BOX);

        auto* tabCreds = new Fl_Group(10, TAB_TOP, DLG_W - 20, dlgH - BTN_H - 30 - TAB_TOP, "Credentials");
        tabCreds->end();
        tabs->end();
    }

    int btnY = dlgH - BTN_H - 8;
    auto* btnSave  = new Fl_Button(DLG_W - 180, btnY, 80, BTN_H, "Save");
    btnSave->callback(onSave, this);

    auto* btnClose = new Fl_Button(DLG_W - 90, btnY, 80, BTN_H, "Close");
    btnClose->callback(onClose, this);
    end();

    set_modal();
}

TableSettings SettingsDialog::getTableSettings() const
{
    TableSettings sets;
    for (auto* w : m_columnWidgets)
        sets.columns.push_back(w->getSets());
    return sets;
}

void SettingsDialog::onSave(Fl_Widget*, void* ctx)
{
    static_cast<SettingsDialog*>(ctx)->hide();
}

void SettingsDialog::onClose(Fl_Widget*, void* ctx)
{
    static_cast<SettingsDialog*>(ctx)->hide();
}
