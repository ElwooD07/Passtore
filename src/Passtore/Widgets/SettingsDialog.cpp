#include "pch.h"
#include "SettingsDialog.h"

using namespace passtore;

static const int s_dlgW  = 383;
static const int s_tabTop = 30;
static const int s_rowH  = 32;
static const int s_btnH  = 28;

SettingsDialog::SettingsDialog(const ResourcesDefinition& defs, const TableSettings& current,
                               SaveCallback saveCallback)
    : Fl_Window(s_dlgW, 100, "Settings")
    , m_saveCallback(std::move(saveCallback))
{
    int scrollH = std::min(static_cast<int>(defs.size()) * s_rowH + 10, 260);
    int dlgH = s_tabTop + 10 + scrollH + 40 + s_btnH + 10;
    resize(x(), y(), s_dlgW, dlgH);

    begin();
    auto* tabs = new Fl_Tabs(10, 10, s_dlgW - 20, dlgH - s_btnH - 30);
    {
        tabs->begin();
        auto* tabTable = new Fl_Group(10, s_tabTop, s_dlgW - 20, dlgH - s_btnH - 30 - s_tabTop, "Table");
        {
            tabTable->begin();
            auto* scroll = new Fl_Scroll(10, s_tabTop + 5, s_dlgW - 20, dlgH - s_btnH - 30 - s_tabTop - 10);
            scroll->begin();
            for (size_t i = 0; i < defs.size(); ++i)
            {
                ColumnSettings cs{ defs[i].name, true };
                for (const auto& saved : current.columns)
                {
                    if (saved.name == defs[i].name)
                    {
                        cs = saved;
                        break;
                    }
                }
                auto* w = new ColumnSettingsWidget(
                    10, s_tabTop + 5 + static_cast<int>(i) * s_rowH, s_dlgW - 36, cs);
                m_columnWidgets.push_back(w);
            }
            scroll->end();
            tabTable->end();
        }
        tabTable->box(FL_FLAT_BOX);

        auto* tabCreds = new Fl_Group(10, s_tabTop, s_dlgW - 20, dlgH - s_btnH - 30 - s_tabTop, "Credentials");
        tabCreds->end();
        tabs->end();
    }

    int btnY = dlgH - s_btnH - 8;
    auto* btnSave  = new Fl_Button(s_dlgW - 180, btnY, 80, s_btnH, "Save");
    btnSave->callback(onSave, this);

    auto* btnClose = new Fl_Button(s_dlgW - 90, btnY, 80, s_btnH, "Close");
    btnClose->callback(onClose, this);
    end();

    set_modal();
}

void SettingsDialog::onSave(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<SettingsDialog*>(ctx);
    TableSettings result;
    for (auto* w : self->m_columnWidgets)
    {
        result.columns.push_back(w->GetSets());
    }
    self->m_saveCallback(result);
    self->hide();
}

void SettingsDialog::onClose(Fl_Widget*, void* ctx)
{
    static_cast<SettingsDialog*>(ctx)->hide();
}
