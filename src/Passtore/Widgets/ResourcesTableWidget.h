#pragma once
#include <vector>
#include <functional>
#include <nana/gui/widgets/textbox.hpp>
#include "Models/ITableModel.h"

namespace passtore
{
    /* Nana-based resource list widget with Add/Delete buttons. */
    class ResourcesTableWidget
    {
    public:
        using DeleteCallback = std::function<void()>;
        using AddCallback = std::function<void()>;

        ResourcesTableWidget(nana::form& parent, int x, int y, int w, int h);
        ~ResourcesTableWidget();

        void SetModel(ITableModel* model);
        void Refresh();
        int SelectedRow() const;
        void BeginShutdown();
        void SetDeleteCallback(DeleteCallback cb) { m_deleteCallback = cb; }
        void SetAddCallback(AddCallback cb) { m_addCallback = cb; }

    private:
        void InitializeColumns();
        void RefreshListboxItems();
        void OnListboxSelected(const nana::arg_listbox& arg);
        void OnListboxDoubleClick(const nana::arg_mouse& arg);
        void OnListboxMouseUp(const nana::arg_mouse& arg);
        void BeginInlineEdit(int row, int col, const nana::point& clickPos);
        void SaveInlineEdit();
        void CancelInlineEdit();
        void HandleInlineFocusLost();
        void BeginBigEdit(int row, int col);
        void OnAdd();
        void OnDelete();

    private:
        ITableModel* m_model = nullptr;
        nana::form* m_parent = nullptr;
        nana::listbox* m_listbox = nullptr;
        nana::textbox* m_inlineEditor = nullptr;
        nana::button* m_btnDelete = nullptr;
        nana::button* m_btnAdd = nullptr;
        int m_editRow = -1;
        int m_editCol = -1;
        bool m_inlineDirty = false;
        int m_bigEditRow = -1;
        int m_bigEditCol = -1;
        bool m_isClosing = false;
        DeleteCallback m_deleteCallback;
        AddCallback m_addCallback;
        int m_selectedRow = -1;
        int m_sortCol = -1;
        bool m_sortReverse = false;
    };
}
