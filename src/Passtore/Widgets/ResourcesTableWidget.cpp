#include "pch.h"
#include "ResourcesTableWidget.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Return_Button.H>
#include <FL/fl_ask.H>

using namespace passtore;

static const int s_fontSize = 13;
static const int s_rowH     = s_fontSize * 2;
static const int s_hdrH     = s_fontSize * 2;
static const int s_btnH     = s_fontSize * 2 + 4;
static const int s_btnW     = 80;
static const int s_btnGap   = 4;
static const int s_colWNorm = 150;
static const int s_colWBig  = 220;
static const int s_textPadX = 8;
static const int s_editInset = 1;
static const int s_popupW = 560;
static const int s_popupH = 320;

/* Fl_Multiline_Input that routes Escape (cancel) and Ctrl+Enter (commit) to a parent callback. */
class BigEditorInput : public Fl_Multiline_Input
{
public:
    using EventCallback = void(*)(bool commit, void* ctx);

    BigEditorInput(int x, int y, int w, int h)
        : Fl_Multiline_Input(x, y, w, h)
    { }

    void SetEventCallback(EventCallback cb, void* ctx)
    {
        m_eventCb = cb;
        m_eventCtx = ctx;
    }

    int handle(int event) override
    {
        if (event == FL_KEYDOWN)
        {
            if (Fl::event_key() == FL_Escape)
            {
                if (m_eventCb) { m_eventCb(false, m_eventCtx); }
                return 1;
            }
            if (Fl::event_key() == FL_Enter && (Fl::event_state() & FL_CTRL))
            {
                if (m_eventCb) { m_eventCb(true, m_eventCtx); }
                return 1;
            }
        }
        return Fl_Multiline_Input::handle(event);
    }

private:
    EventCallback m_eventCb = nullptr;
    void* m_eventCtx = nullptr;
};

/* Modal editor used for big fields (like Notes) to provide a native multiline editing surface. */
class BigEditorPopup : public Fl_Double_Window
{
public:
    BigEditorPopup()
        : Fl_Double_Window(s_popupW, s_popupH, "Edit Value")
    {
        set_modal();
        begin();

        int margin = 12;
        int btnH = 28;
        int btnW = 90;
        int inputX = margin;
        int inputY = margin;
        int inputW = w() - (margin * 2);
        int inputH = h() - (margin * 3) - btnH;
        m_input = new BigEditorInput(inputX, inputY, inputW, inputH);
        m_input->box(FL_BORDER_BOX);
        m_input->textfont(FL_HELVETICA);
        m_input->textsize(s_fontSize);
        m_input->SetEventCallback(onEditorEvent, this);

        int btnY = h() - margin - btnH;
        int cancelX = w() - margin - btnW - btnW - 8;
        int saveX = w() - margin - btnW;

        m_btnCancel = new Fl_Button(cancelX, btnY, btnW, btnH, "Cancel");
        m_btnCancel->callback(onCancel, this);

        m_btnSave = new Fl_Return_Button(saveX, btnY, btnW, btnH, "Save");
        m_btnSave->callback(onSave, this);

        end();
        callback(onWindowClose, this);
    }

    bool Edit(int centerX, int centerY, const std::string& current, std::string& updated)
    {
        m_saved = false;
        m_input->value(current.c_str());
        int len = static_cast<int>(current.size());
        m_input->insert_position(len);
        m_input->mark(0);

        int px = std::max(0, centerX - (w() / 2));
        int py = std::max(0, centerY - (h() / 2));
        if (px + w() > Fl::w())
        {
            px = std::max(0, Fl::w() - w());
        }
        if (py + h() > Fl::h())
        {
            py = std::max(0, Fl::h() - h());
        }

        position(px, py);
        show();
        m_input->take_focus();
        while (shown())
        {
            Fl::wait();
        }

        if (!m_saved)
        {
            return false;
        }

        updated = m_input->value();
        return true;
    }

private:
    static void onSave(Fl_Widget*, void* ctx)
    {
        auto* self = static_cast<BigEditorPopup*>(ctx);
        self->m_saved = true;
        self->hide();
    }

    static void onCancel(Fl_Widget*, void* ctx)
    {
        auto* self = static_cast<BigEditorPopup*>(ctx);
        self->m_saved = false;
        self->hide();
    }

    static void onWindowClose(Fl_Widget*, void* ctx)
    {
        auto* self = static_cast<BigEditorPopup*>(ctx);
        self->m_saved = false;
        self->hide();
    }

    static void onEditorEvent(bool commit, void* ctx)
    {
        auto* self = static_cast<BigEditorPopup*>(ctx);
        if (commit)
        {
            self->m_saved = true;
            self->hide();
        }
        else
        {
            self->m_saved = false;
            self->hide();
        }
    }

private:
    BigEditorInput* m_input = nullptr;
    Fl_Button* m_btnCancel = nullptr;
    Fl_Return_Button* m_btnSave = nullptr;
    bool m_saved = false;
};

/* Fl_Table_Row subclass that renders the resource grid and drives inline cell editing. */
class passtore::ResourcesTable : public Fl_Table_Row
{
public:
    ResourcesTable(int x, int y, int w, int h)
        : Fl_Table_Row(x, y, w, h)
    {
        col_header(1);
        col_header_height(s_hdrH);
        row_height_all(s_rowH);
        col_width_all(s_colWNorm);
        col_resize(1);
        row_resize(0);
        type(SELECT_SINGLE);

        begin();
        m_editor = new Fl_Input(0, 0, 0, 0);
        m_editor->hide();
        m_editor->box(FL_FLAT_BOX);
        m_editor->color(fl_rgb_color(255, 255, 255));
        m_editor->textcolor(fl_rgb_color(16, 24, 40));
        m_editor->textfont(FL_HELVETICA);
        m_editor->textsize(s_fontSize);
        end();

        selection_color(fl_rgb_color(36, 99, 180));
    }

    ~ResourcesTable() { CancelEdit(); }

    void SetModel(ITableModel* model)
    {
        m_model = model;
        RedistributeColumns();
        Refresh();
    }

    void resize(int x, int y, int w, int h) override
    {
        Fl_Table_Row::resize(x, y, w, h);
        RedistributeColumns();
    }

    void Refresh()
    {
        rows(m_model ? m_model->RowCount()    : 0);
        cols(m_model ? m_model->ColumnCount() : 0);
        redraw();
    }

    int SelectedRow()
    {
        for (int r = 0; r < rows(); ++r) {
            if (row_selected(r)) {
                return r;
            }
        }
        return -1;
    }

protected:
    void draw_cell(TableContext ctx, int R, int C, int X, int Y, int W, int H) override
    {
        switch (ctx)
        {
        case CONTEXT_STARTPAGE:
            fl_font(FL_HELVETICA, s_fontSize);
            return;

        case CONTEXT_COL_HEADER:
            fl_push_clip(X, Y, W, H);
            fl_color(fl_rgb_color(224, 229, 236));
            fl_rectf(X, Y, W, H);
            fl_color(fl_rgb_color(178, 186, 197));
            fl_line(X, Y + H - 1, X + W, Y + H - 1);
            if (m_model)
            {
                fl_color(fl_rgb_color(45, 55, 72));
                fl_font(FL_HELVETICA_BOLD, s_fontSize);
                fl_draw(m_model->ColumnName(C).c_str(), X + s_textPadX, Y, W - (s_textPadX * 2), H, FL_ALIGN_LEFT | FL_ALIGN_CLIP);
            }
            fl_pop_clip();
            return;

        case CONTEXT_CELL:
            fl_push_clip(X, Y, W, H);
            {
                const bool selected = row_selected(R);
                const bool hovered = (R == m_hoverRow);

                Fl_Color bg = selected
                    ? selection_color()
                    : (R % 2 == 0 ? fl_rgb_color(252, 253, 255) : fl_rgb_color(245, 248, 252));
                if (!selected && hovered)
                {
                    bg = fl_rgb_color(236, 243, 252);
                }
                fl_color(bg);
                fl_rectf(X, Y, W, H);

                if (m_model)
                {
                    std::string data = m_model->CellData(R, C);
                    std::string display = (!m_model->IsVisibleColumn(C) && !data.empty())
                        ? std::string(data.size(), '*') : data;
                    std::fill(data.begin(), data.end(), '\0');

                    fl_color(selected ? FL_WHITE : fl_rgb_color(31, 41, 55));
                    fl_font(FL_HELVETICA, s_fontSize);
                    fl_draw(display.c_str(), X + s_textPadX, Y, W - (s_textPadX * 2), H, FL_ALIGN_LEFT | FL_ALIGN_CLIP);
                    std::fill(display.begin(), display.end(), '\0');
                }
                fl_color(fl_rgb_color(220, 226, 234));
                fl_rect(X, Y, W, H);
            }
            fl_pop_clip();
            return;

        default:
            return;
        }
    }

    int handle(int event) override
    {
        // While an editor is active, route all keyboard events directly to it so
        // Fl_Table_Row never intercepts arrow keys for row navigation.
        if (m_editRow >= 0 && (event == FL_KEYDOWN || event == FL_KEYUP))
        {
            if (Fl::event_key() == FL_Escape) { CancelEdit(); return 1; }
            if (Fl::event_key() == FL_Enter)  { CommitEdit();  return 1; }

            if (event == FL_KEYDOWN)
            {
                int key = Fl::event_key();
                if (key == FL_Left || key == FL_Right)
                {
                    int pos = m_editor->insert_position();
                    int mark = m_editor->mark();
                    int len = static_cast<int>(std::string(m_editor->value()).size());
                    bool allSelected = (std::min(pos, mark) == 0) && (std::max(pos, mark) == len);
                    if (allSelected)
                    {
                        m_editor->insert_position(len);
                        m_editor->mark(len);
                    }
                }
            }

            return m_editor->handle(event);
        }

        int result = Fl_Table_Row::handle(event);

        switch (event)
        {
        case FL_MOVE:
        {
            int R = 0;
            int C = 0;
            ResizeFlag rf;
            TableContext ctx = cursor2rowcol(R, C, rf);
            int newHover = (ctx == CONTEXT_CELL) ? R : -1;
            if (newHover != m_hoverRow)
            {
                int oldHover = m_hoverRow;
                m_hoverRow = newHover;
                RedrawRow(oldHover);
                RedrawRow(newHover);
            }
            break;
        }

        case FL_LEAVE:
            if (m_hoverRow != -1)
            {
                int oldHover = m_hoverRow;
                m_hoverRow = -1;
                RedrawRow(oldHover);
            }
            break;

        case FL_PUSH:
        {
            int R = 0;
            int C = 0;
            ResizeFlag rf;
            TableContext ctx = cursor2rowcol(R, C, rf);

            if (m_editRow >= 0)
            {
                if (ctx == CONTEXT_CELL && R == m_editRow && C == m_editCol)
                {
                    m_editor->take_focus();
                    return 1;
                }

                CommitEdit();
            }

            if (Fl::event_clicks() > 0 && m_model)
            {
                if (ctx == CONTEXT_CELL)
                {
                    int cx, cy, cw, ch;
                    find_cell(CONTEXT_CELL, R, C, cx, cy, cw, ch);
                    StartEditing(R, C, cx, cy, cw, ch);
                    return 1;
                }
            }
            break;
        }
        }

        return result;
    }

private:
    void RedrawRow(int row)
    {
        if (row < 0 || row >= rows() || cols() <= 0)
        {
            return;
        }

        redraw_range(row, row, 0, cols() - 1);
    }

    void StartEditing(int row, int col, int x, int y, int w, int h)
    {
        if (m_editRow >= 0) {
            CommitEdit();
        }

        bool big = m_model && m_model->IsBigColumn(col);
        std::string current = m_model ? m_model->CellData(row, col) : "";

        if (big)
        {
            std::string updated;
            int centerX = this->x() + x + (w / 2);
            int centerY = this->y() + y + (h / 2);
            if (m_bigPopup.Edit(centerX, centerY, current, updated))
            {
                m_model->SetCellData(row, col, updated);
            }
            std::fill(updated.begin(), updated.end(), '\0');
            redraw();
        }
        else
        {
            m_editRow = row;
            m_editCol = col;
            m_editor->resize(x + s_editInset, y + s_editInset, std::max(40, w - (s_editInset * 2)), std::max(24, h - (s_editInset * 2)));
            m_editor->value(current.c_str());
            m_editor->show();
            m_editor->take_focus();
            int len = static_cast<int>(current.size());
            m_editor->insert_position(len);
            m_editor->mark(0);
        }
        std::fill(current.begin(), current.end(), '\0');
    }

    void CommitEdit()
    {
        if (m_editRow < 0 || !m_model) {
            return;
        }

        std::string newValue(m_editor->value());
        m_model->SetCellData(m_editRow, m_editCol, newValue);
        std::fill(newValue.begin(), newValue.end(), '\0');

        m_editor->value("");
        m_editor->hide();
        m_editRow = -1;
        m_editCol = -1;
        redraw();
    }

    void CancelEdit()
    {
        if (m_editRow < 0) {
            return;
        }

        m_editor->value("");
        m_editor->hide();
        m_editRow = -1;
        m_editCol = -1;
        redraw();
    }

    void RedistributeColumns()
    {
        if (!m_model || m_model->ColumnCount() == 0)
        {
            return;
        }

        int nCols = m_model->ColumnCount();
        int naturalTotal = 0;
        for (int c = 0; c < nCols; ++c)
        {
            naturalTotal += m_model->IsBigColumn(c) ? s_colWBig : s_colWNorm;
        }

        int available = tiw;
        if (available >= naturalTotal)
        {
            // stretch columns proportionally to fill available width
            int distributed = 0;
            for (int c = 0; c < nCols - 1; ++c)
            {
                int nat = m_model->IsBigColumn(c) ? s_colWBig : s_colWNorm;
                int cw = nat * available / naturalTotal;
                col_width(c, cw);
                distributed += cw;
            }
            // last column absorbs rounding remainder
            col_width(nCols - 1, available - distributed);
        }
        else
        {
            // restore natural widths; Fl_Table_Row shows horizontal scrollbar automatically
            for (int c = 0; c < nCols; ++c)
            {
                col_width(c, m_model->IsBigColumn(c) ? s_colWBig : s_colWNorm);
            }
        }
    }

    ITableModel* m_model = nullptr;
    Fl_Input* m_editor = nullptr;
    BigEditorPopup m_bigPopup;
    int m_hoverRow = -1;
    int m_editRow = -1;
    int m_editCol = -1;
};

// ─── Container widget ──────────────────────────────────────────────────────

ResourcesTableWidget::ResourcesTableWidget(int x, int y, int w, int h)
    : Fl_Group(x, y, w, h)
{
    begin();

    int tableH = h - s_btnGap - s_btnH - s_btnGap;
    m_tableWidget = new ResourcesTable(x, y, w, tableH);

    int btnY = y + tableH + s_btnGap;
    int addX = x + w - s_btnW;
    int deleteX = addX - s_btnGap - s_btnW;

    m_btnDelete = new Fl_Button(deleteX, btnY, s_btnW, s_btnH, "Delete");
    m_btnDelete->callback(onDelete, this);

    m_btnAdd = new Fl_Button(addX, btnY, s_btnW, s_btnH, "Add");
    m_btnAdd->callback(onAdd, this);

    end();
    resizable(m_tableWidget);
}

void ResourcesTableWidget::SetModel(ITableModel* model)
{
    m_model = model;
    m_tableWidget->SetModel(model);
}

void ResourcesTableWidget::Refresh()
{
    m_tableWidget->Refresh();
}

void ResourcesTableWidget::onAdd(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<ResourcesTableWidget*>(ctx);
    if (self->m_model)
    {
        self->m_model->AddRow();
        self->m_tableWidget->Refresh();
    }
}

void ResourcesTableWidget::onDelete(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<ResourcesTableWidget*>(ctx);
    int row = self->m_tableWidget->SelectedRow();
    if (row < 0 || !self->m_model) {
        return;
    }

    int result = fl_choice("Delete selected entry?", "Cancel", "Delete", nullptr);
    if (result != 1)
    {
        return;
    }

    self->m_model->DeleteRow(row);
    self->m_tableWidget->Refresh();
}

