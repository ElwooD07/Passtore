#include "pch.h"
#include "ResourcesListWidget.h"

using namespace passtore;

static const int FONT_SIZE  = 13;
static const int ROW_H      = FONT_SIZE * 2;      // 26
static const int HDR_H      = FONT_SIZE * 2;      // 26
static const int BTN_H      = FONT_SIZE * 2 + 4;  // 30
static const int BTN_W      = 80;
static const int BTN_GAP    = 4;
static const int COL_W_NORM = 150;  // default column width
static const int COL_W_BIG  = 220;  // wider for big (multiline) columns

// ─── Inner table widget ────────────────────────────────────────────────────

class passtore::ResourcesTable : public Fl_Table_Row
{
public:
    ResourcesTable(int x, int y, int w, int h)
        : Fl_Table_Row(x, y, w, h)
    {
        col_header(1);
        col_header_height(HDR_H);
        row_height_all(ROW_H);
        col_width_all(COL_W_NORM);
        col_resize(1);
        row_resize(0);
        type(SELECT_SINGLE);

        begin();
        m_editor = new Fl_Input(0, 0, 0, 0);
        m_editor->hide();
        m_editor->box(FL_BORDER_BOX);
        m_editor->textfont(FL_HELVETICA);
        m_editor->textsize(FONT_SIZE);
        m_bigEditor = new Fl_Multiline_Input(0, 0, 0, 0);
        m_bigEditor->hide();
        m_bigEditor->box(FL_BORDER_BOX);
        m_bigEditor->textfont(FL_COURIER);
        m_bigEditor->textsize(FONT_SIZE);
        end();
    }

    ~ResourcesTable() { cancelEdit(); }

    void setModel(ITableModel* model)
    {
        m_model = model;
        if (m_model)
        {
            col_width_all(COL_W_NORM);
            for (int c = 0; c < m_model->columnCount(); ++c)
            {
                if (m_model->isBigColumn(c))
                {
                    col_width(c, COL_W_BIG);
                }
            }
        }
        refresh();
    }

    void refresh()
    {
        rows(m_model ? m_model->rowCount()    : 0);
        cols(m_model ? m_model->columnCount() : 0);
        redraw();
    }

    int selectedRow()
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
            fl_font(FL_HELVETICA, FONT_SIZE);
            return;

        case CONTEXT_COL_HEADER:
            fl_push_clip(X, Y, W, H);
            fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_BACKGROUND_COLOR);
            if (m_model)
            {
                fl_color(FL_FOREGROUND_COLOR);
                fl_font(FL_HELVETICA_BOLD, FONT_SIZE);
                fl_draw(m_model->columnName(C).c_str(), X, Y, W, H, FL_ALIGN_CENTER | FL_ALIGN_CLIP);
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
                    std::string data = m_model->cellData(R, C);
                    std::string display = (m_model->isBigColumn(C) && !data.empty())
                        ? std::string(data.size(), '*') : data;
                    std::fill(data.begin(), data.end(), '\0');

                    fl_color(row_selected(R) ? FL_WHITE : FL_FOREGROUND_COLOR);
                    fl_font(FL_HELVETICA, FONT_SIZE);
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
                    startEditing(R, C, cx, cy, cw, ch);
                    return 1;
                }
            }
            break;

        case FL_KEYDOWN:
            if (m_editRow >= 0) {
                if (Fl::event_key() == FL_Escape) {
                    cancelEdit();
                    return 1;
                }
                if (Fl::event_key() == FL_Enter && m_model && !m_model->isBigColumn(m_editCol)) {
                    commitEdit();
                    return 1;
                }
            }
            break;
        }

        return result;
    }

private:
    void startEditing(int row, int col, int x, int y, int w, int h)
    {
        if (m_editRow >= 0) {
            commitEdit();
        }
        m_editRow = row;
        m_editCol = col;

        bool big = m_model && m_model->isBigColumn(col);
        std::string current = m_model ? m_model->cellData(row, col) : "";

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

    void commitEdit()
    {
        if (m_editRow < 0 || !m_model) {
            return;
        }

        bool big = m_model->isBigColumn(m_editCol);
        Fl_Input_* active = big ? static_cast<Fl_Input_*>(m_bigEditor)
                                : static_cast<Fl_Input_*>(m_editor);

        std::string newValue(active->value());
        m_model->setCellData(m_editRow, m_editCol, newValue);
        std::fill(newValue.begin(), newValue.end(), '\0');

        active->value("");
        active->hide();
        m_editRow = -1;
        m_editCol = -1;
        redraw();
    }

    void cancelEdit()
    {
        if (m_editRow < 0) {
            return;
        }

        bool big = m_model && m_model->isBigColumn(m_editCol);
        Fl_Input_* active = big ? static_cast<Fl_Input_*>(m_bigEditor)
                                : static_cast<Fl_Input_*>(m_editor);
        active->value("");
        active->hide();
        m_editRow = -1;
        m_editCol = -1;
        redraw();
    }

    ITableModel*         m_model    = nullptr;
    Fl_Input*            m_editor   = nullptr;
    Fl_Multiline_Input*  m_bigEditor = nullptr;
    int                  m_editRow  = -1;
    int                  m_editCol  = -1;
};

// ─── Container widget ──────────────────────────────────────────────────────

ResourcesListWidget::ResourcesListWidget(int x, int y, int w, int h)
    : Fl_Group(x, y, w, h)
{
    begin();

    int tableH = h - BTN_GAP - BTN_H - BTN_GAP;
    m_tableWidget = new ResourcesTable(x, y, w, tableH);

    int btnY = y + tableH + BTN_GAP;
    m_btnDelete = new Fl_Button(x,             btnY, BTN_W, BTN_H, "Delete");
    m_btnDelete->callback(onDelete, this);

    m_btnAdd    = new Fl_Button(x + w - BTN_W, btnY, BTN_W, BTN_H, "Add");
    m_btnAdd->callback(onAdd, this);

    end();
    resizable(m_tableWidget);
}

void ResourcesListWidget::setModel(ITableModel* model)
{
    m_model = model;
    m_tableWidget->setModel(model);
}

void ResourcesListWidget::refresh()
{
    m_tableWidget->refresh();
}

void ResourcesListWidget::onAdd(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<ResourcesListWidget*>(ctx);
    if (self->m_model)
    {
        self->m_model->addRow();
        self->m_tableWidget->refresh();
    }
}

void ResourcesListWidget::onDelete(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<ResourcesListWidget*>(ctx);
    int row = self->m_tableWidget->selectedRow();
    if (row < 0 || !self->m_model) {
        return;
    }
    self->m_model->deleteRow(row);
    self->m_tableWidget->refresh();
}

