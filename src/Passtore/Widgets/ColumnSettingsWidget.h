#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <string>
#include "Settings.h"

namespace passtore
{
    // Single row in SettingsDialog — label | Visible checkbox | Blured checkbox
    class ColumnSettingsWidget : public Fl_Group
    {
    public:
        ColumnSettingsWidget(int x, int y, int w, const ColumnSettings& sets);
        ColumnSettings getSets() const;

    private:
        Fl_Box*          m_lblName;
        Fl_Check_Button* m_chkVisible;
        Fl_Check_Button* m_chkBlured;
        std::string      m_name;
    };
}
