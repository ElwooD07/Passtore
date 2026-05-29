#include <pch.h>
#include "ColumnSettingsWidget.h"

using namespace passtore;

static const int ROW_H = 32;

ColumnSettingsWidget::ColumnSettingsWidget(int x, int y, int w, const ColumnSettings& sets)
    : Fl_Group(x, y, w, ROW_H)
    , m_name(sets.name)
{
    begin();
    int nameW = w / 2;
    m_lblName = new Fl_Box(x + 6, y + 4, nameW - 12, 24, m_name.c_str());
    m_lblName->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);

    m_chkVisible = new Fl_Check_Button(x + nameW, y + 4, 80, 24, "Visible");
    m_chkVisible->value(sets.visible ? 1 : 0);
    end();

    box(FL_FLAT_BOX);
}

ColumnSettings ColumnSettingsWidget::getSets() const
{
    return ColumnSettings{ m_name, m_chkVisible->value() != 0 };
}
