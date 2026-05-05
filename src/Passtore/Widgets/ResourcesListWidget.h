#pragma once
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include "Models/ITableModel.h"

namespace passtore
{
    // Fl_Table_Row subclass. Renders ITableModel cells with blur support for sensitive columns.
    // Inline editing: double-click a cell to edit; Enter commits, Escape cancels.
    class ResourcesListWidget : public Fl_Table_Row
    {
    public:
        ResourcesListWidget(int x, int y, int w, int h, ITableModel* model = nullptr);
        ~ResourcesListWidget();

        void setModel(ITableModel* model);
        void refresh();

    protected:
        void draw_cell(TableContext ctx, int R, int C, int X, int Y, int W, int H) override;
        int handle(int event) override;

    private:
        void startEditing(int row, int col, int x, int y, int w, int h);
        void commitEdit();
        void cancelEdit();

    private:
        ITableModel* m_model = nullptr;
        Fl_Input* m_editor = nullptr;
        Fl_Multiline_Input* m_bigEditor = nullptr;
        int m_editRow = -1;
        int m_editCol = -1;
    };
}
