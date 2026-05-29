#include "pch.h"
#include "ResourcesTableWidget.h"

using namespace passtore;

static const int s_fontSize = 13;
static const int s_rowH     = s_fontSize * 2;
static const int s_hdrH     = s_fontSize * 2;
static const int s_btnH     = s_fontSize * 2 + 4;
static const int s_btnW     = 80;
static const int s_btnGap   = 4;
static const int s_colWNorm = 150;
static const int s_colWBig  = 220;

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
        m_editor->box(FL_BORDER_BOX);
        m_editor->textfont(FL_HELVETICA);
        m_editor->textsize(s_fontSize);
        m_bigEditor = new Fl_Multiline_Input(0, 0, 0, 0);
        m_bigEditor->hide();
        m_bigEditor->box(FL_BORDER_BOX);
        m_bigEditor->textfont(FL_COURIER);
        m_bigEditor->textsize(s_fontSize);
        end();
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
            fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_BACKGROUND_COLOR);
            if (m_model)
            {
                fl_color(FL_FOREGROUND_COLOR);
                fl_font(FL_HELVETICA_BOLD, s_fontSize);
                fl_draw(m_model->ColumnName(C).c_str(), X, Y, W, H, FL_ALIGN_CENTER | FL_ALIGN_CLIP);
            }
            fl_pop_clip();
            return;

        case CONTEXT_CELL:
            if (R == m_editRow && C == m_editCol) {
                return;
            }
            fl_push_clip(X, Y, W, H);
            {
                Fl_Color bg = row_selected(R) ? selection_color() : FL_BACKGROUND2_COLOR;
                fl_color(bg);
                fl_rectf(X, Y, W, H);

                if (m_model)
                {
                    std::string data = m_model->CellData(R, C);
                    std::string display = (!m_model->IsVisibleColumn(C) && !data.empty())
                        ? std::string(data.size(), '*') : data;
                    std::fill(data.begin(), data.end(), '\0');

                    fl_color(row_selected(R) ? FL_WHITE : FL_FOREGROUND_COLOR);
                    fl_font(FL_HELVETICA, s_fontSize);
                    fl_draw(display.c_str(), X + 4, Y, W - 8, H, FL_ALIGN_LEFT | FL_ALIGN_CLIP);
                    std::fill(display.begin(), display.end(), '\0');
                }
                fl_color(FL_DARK3);
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
        int result = Fl_Table_Row::handle(event);

        switch (event)
        {
        case FL_PUSH:
            if (Fl::event_clicks() >= 1 && m_model)
            {
                int R = 0, C = 0;
                ResizeFlag rf;
                TableContext ctx = cursor2rowcol(R, C, rf);
                if (ctx == CONTEXT_CELL)
                {
                    int cx, cy, cw, ch;
                    find_cell(CONTEXT_CELL, R, C, cx, cy, cw, ch);
                    StartEditing(R, C, cx, cy, cw, ch);
                    return 1;
                }
            }
            break;

        case FL_KEYDOWN:
            if (m_editRow >= 0) {
                if (Fl::event_key() == FL_Escape) {
                    CancelEdit();
                    return 1;
                }
                if (Fl::event_key() == FL_Enter && m_model && !m_model->IsBigColumn(m_editCol)) {
                    CommitEdit();
                    return 1;
                }
            }
            break;
        }

        return result;
    }

private:
    void StartEditing(int row, int col, int x, int y, int w, int h)
    {
        if (m_editRow >= 0) {
            CommitEdit();
        }
        m_editRow = row;
        m_editCol = col;

        bool big = m_model && m_model->IsBigColumn(col);
        std::string current = m_model ? m_model->CellData(row, col) : "";

        if (big)
        {
            m_bigEditor->resize(x, y, w, std::min(h * 4, 96));
            m_bigEditor->value(current.c_str());
            m_bigEditor->show();
            m_bigEditor->take_focus();
        }
        else
        {
            m_editor->resize(x, y, w, h);
            m_editor->value(current.c_str());
            m_editor->show();
            m_editor->take_focus();
        }
        std::fill(current.begin(), current.end(), '\0');
    }

    void CommitEdit()
    {
        if (m_editRow < 0 || !m_model) {
            return;
        }

        bool big = m_model->IsBigColumn(m_editCol);
        Fl_Input_* active = big ? static_cast<Fl_Input_*>(m_bigEditor)
                                : static_cast<Fl_Input_*>(m_editor);

        std::string newValue(active->value());
        m_model->SetCellData(m_editRow, m_editCol, newValue);
        std::fill(newValue.begin(), newValue.end(), '\0');

        active->value("");
        active->hide();
        m_editRow = -1;
        m_editCol = -1;
        redraw();
    }

    void CancelEdit()
    {
        if (m_editRow < 0) {
            return;
        }

        bool big = m_model && m_model->IsBigColumn(m_editCol);
        Fl_Input_* active = big ? static_cast<Fl_Input_*>(m_bigEditor)
                                : static_cast<Fl_Input_*>(m_editor);
        active->value("");
        active->hide();
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

        int available = w();
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
    Fl_Multiline_Input* m_bigEditor = nullptr;
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
    m_btnDelete = new Fl_Button(x,             btnY, s_btnW, s_btnH, "Delete");
    m_btnDelete->callback(onDelete, this);

    m_btnAdd    = new Fl_Button(x + w - s_btnW, btnY, s_btnW, s_btnH, "Add");
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
    self->m_model->DeleteRow(row);
    self->m_tableWidget->Refresh();
}

