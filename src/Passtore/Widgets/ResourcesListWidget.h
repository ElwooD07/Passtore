#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include "Models/ITableModel.h"

namespace passtore
{
    class ResourcesTable;  // inner Fl_Table_Row — defined in .cpp

    // Container matching the original Qt ResourcesListWidget layout:
    // table fills the area, Delete button bottom-left, Add button bottom-right.
    class ResourcesListWidget : public Fl_Group
    {
    public:
        ResourcesListWidget(int x, int y, int w, int h);

        void setModel(ITableModel* model);
        void refresh();

    private:
        static void onAdd(Fl_Widget*, void* ctx);
        static void onDelete(Fl_Widget*, void* ctx);

    private:
        ITableModel*    m_model = nullptr;
        ResourcesTable* m_tableWidget;
        Fl_Button*      m_btnDelete;
        Fl_Button*      m_btnAdd;
    };
}
