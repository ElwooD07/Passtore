#include "pch.h"
#include "ResourcesTableWidget.h"
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/form.hpp>

using namespace passtore;

namespace
{
    constexpr int s_inlineRowHeight = 26;
}

ResourcesTableWidget::ResourcesTableWidget(nana::form& parent, int x, int y, int w, int h)
    : m_parent(&parent)
{
    int listboxH = h - 40;
    int btnY = y + h - 35;
    int btnDeleteX = w - 80;
    int btnAddX = w - 160;

    m_listbox = new nana::listbox(parent, nana::rectangle{ x, y, static_cast<unsigned>(w), static_cast<unsigned>(listboxH) });
    m_listbox->append("Resources");
    m_listbox->show_header(true);
    m_listbox->sortable(false);
    m_listbox->enable_single(true, true);

    // Keep single-click behavior lightweight: select/highlight only, no full refresh.
    m_listbox->events().selected([this](const nana::arg_listbox& arg)
    {
        OnListboxSelected(arg);
    });

    m_listbox->events().dbl_click([this](const nana::arg_mouse& arg)
    {
        OnListboxDoubleClick(arg);
    });

    m_listbox->events().mouse_up([this](const nana::arg_mouse& arg)
    {
        OnListboxMouseUp(arg);
    });

    m_inlineEditor = new nana::textbox(parent, nana::rectangle{ 0, 0, 0u, 0u });
    m_inlineEditor->multi_lines(false);
    m_inlineEditor->hide();

    m_inlineEditor->events().text_changed([this](const nana::arg_textbox&)
    {
        if (m_editRow >= 0)
        {
            m_inlineDirty = true;
        }
    });

    m_inlineEditor->events().key_char([this](const nana::arg_keyboard& arg)
    {
        if (m_editRow < 0)
        {
            return;
        }

        if (arg.key == '\r')
        {
            SaveInlineEdit();
            arg.ignore = true;
            return;
        }

        if (arg.key == 27)
        {
            CancelInlineEdit();
            arg.ignore = true;
        }
    });

    m_inlineEditor->events().focus([this](const nana::arg_focus& arg)
    {
        if (!arg.getting)
        {
            HandleInlineFocusLost();
        }
    });

    m_btnAdd = new nana::button(parent, nana::rectangle{ btnAddX, btnY, 75u, 30u });
    m_btnAdd->caption("Add");
    m_btnAdd->events().click([this]()
    {
        OnAdd();
    });

    m_btnDelete = new nana::button(parent, nana::rectangle{ btnDeleteX, btnY, 75u, 30u });
    m_btnDelete->caption("Delete");
    m_btnDelete->events().click([this]()
    {
        OnDelete();
    });
}

ResourcesTableWidget::~ResourcesTableWidget()
{
}

void ResourcesTableWidget::BeginShutdown()
{
    if (m_isClosing)
    {
        return;
    }

    m_isClosing = true;
    CancelInlineEdit();
}

void ResourcesTableWidget::SetModel(ITableModel* model)
{
    m_model = model;
    InitializeColumns();
    Refresh();
}

void ResourcesTableWidget::Refresh()
{
    RefreshListboxItems();
}

int ResourcesTableWidget::SelectedRow() const
{
    return m_selectedRow;
}

void ResourcesTableWidget::InitializeColumns()
{
    if (!m_model || !m_listbox)
    {
        return;
    }

    m_listbox->clear_headers();
    const int cols = m_model->ColumnCount();

    for (int c = 0; c < cols; ++c)
    {
        const unsigned width = m_model->IsBigColumn(c) ? 220u : 150u;
        m_listbox->append_header(m_model->ColumnName(c), width);

        // Non-visible columns (e.g. Password) stay in the listbox but show masked text.
        // Do NOT call column.visible(false) — that removes the column from the header entirely.
        if (m_model->IsVisibleColumn(c))
        {
            m_listbox->set_sort_compare(static_cast<nana::listbox::size_type>(c),
                [](const std::string& lhs, nana::any*, const std::string& rhs, nana::any*, bool reverse)
                {
                    return reverse ? (rhs < lhs) : (lhs < rhs);
                });
        }
        else
        {
            // Explicitly disable sorting for hidden/masked columns (e.g. Password).
            m_listbox->set_sort_compare(static_cast<nana::listbox::size_type>(c),
                [](const std::string&, nana::any*, const std::string&, nana::any*, bool)
                {
                    return false;
                });
        }
    }
}

void ResourcesTableWidget::RefreshListboxItems()
{
    if (m_isClosing || !m_model || !m_listbox)
    {
        return;
    }

    m_listbox->clear();
    if (m_listbox->size_categ() == 0)
    {
        m_listbox->append("Resources");
    }

    auto category = m_listbox->at(0);
    const int rowCount = m_model->RowCount();
    const int colCount = m_model->ColumnCount();

    for (int r = 0; r < rowCount; ++r)
    {
        if (colCount <= 0)
        {
            continue;
        }

        std::string firstCell = m_model->CellData(r, 0);
        if (!m_model->IsVisibleColumn(0) && !firstCell.empty())
        {
            firstCell.assign(firstCell.size(), '*');
        }

        auto item = category.append(firstCell);
        for (int c = 1; c < colCount; ++c)
        {
            std::string cellValue = m_model->CellData(r, c);
            if (!m_model->IsVisibleColumn(c) && !cellValue.empty())
            {
                cellValue.assign(cellValue.size(), '*');
            }

            item.text(static_cast<nana::listbox::size_type>(c), cellValue);
        }

        // Keep model row mapping stable even when the view is sorted.
        item.value(static_cast<int>(r));

        if (r == m_selectedRow)
        {
            item.select(true, true);
        }
    }
}

void ResourcesTableWidget::OnListboxSelected(const nana::arg_listbox& arg)
{
    if (m_isClosing)
    {
        return;
    }

    if (arg.item.empty())
    {
        return;
    }

    const int* row = arg.item.value_ptr<int>();
    if (row)
    {
        m_selectedRow = *row;
    }
}

void ResourcesTableWidget::OnListboxDoubleClick(const nana::arg_mouse& arg)
{
    if (m_isClosing || !m_model || !m_listbox)
    {
        return;
    }

    const auto disp = m_listbox->cast(arg.pos);
    if (disp.empty() || disp.is_category())
    {
        return;
    }

    const auto abs = m_listbox->index_cast(disp, true);
    auto item = m_listbox->at(abs);
    const int* rowPtr = item.value_ptr<int>();
    if (!rowPtr)
    {
        return;
    }

    const auto col = m_listbox->column_from_pos(arg.pos);
    if (col == nana::npos || col >= static_cast<nana::listbox::size_type>(m_model->ColumnCount()))
    {
        return;
    }

    const int row = *rowPtr;
    const int colIndex = static_cast<int>(col);
    if (m_model->IsBigColumn(colIndex))
    {
        BeginBigEdit(row, colIndex);
        return;
    }

    BeginInlineEdit(row, colIndex, arg.pos);
}

void ResourcesTableWidget::OnListboxMouseUp(const nana::arg_mouse& arg)
{
    if (m_isClosing || !m_model || !m_listbox)
    {
        return;
    }

    if (arg.left_button == false)
    {
        return;
    }

    const auto hit = m_listbox->cast(arg.pos);
    if (!hit.empty())
    {
        return;
    }

    const auto col = m_listbox->column_from_pos(arg.pos);
    if (col == nana::npos || col >= static_cast<nana::listbox::size_type>(m_model->ColumnCount()))
    {
        return;
    }

    const int colIndex = static_cast<int>(col);
    if (!m_model->IsVisibleColumn(colIndex))
    {
        return;
    }

    if (m_sortCol == colIndex)
    {
        m_sortReverse = !m_sortReverse;
    }
    else
    {
        m_sortCol = colIndex;
        m_sortReverse = false;
    }

    m_listbox->sort_col(col, m_sortReverse);
}

void ResourcesTableWidget::BeginInlineEdit(int row, int col, const nana::point& clickPos)
{
    if (m_isClosing || !m_model || !m_inlineEditor || !m_listbox)
    {
        return;
    }

    if (m_editRow >= 0)
    {
        HandleInlineFocusLost();
    }

    m_editRow = row;
    m_editCol = col;
    m_inlineDirty = false;

    const auto area = m_listbox->content_area();
    const int editorH = s_inlineRowHeight - 2;

    int colStart = 0;
    for (int c = 0; c < col; ++c)
    {
        const auto& prevCol = m_listbox->column_at(static_cast<nana::listbox::size_type>(c));
        if (prevCol.visible())
        {
            colStart += static_cast<int>(prevCol.width());
        }
    }

    const auto& currentCol = m_listbox->column_at(static_cast<nana::listbox::size_type>(col));
    const int colWidth = static_cast<int>(currentCol.width());

    const nana::point listPos = m_listbox->pos();
    int editorX = listPos.x + area.x + colStart + 1;
    int editorW = std::max(40, colWidth - 2);

    int relativeY = clickPos.y - area.y;
    if (relativeY < 0)
    {
        relativeY = 0;
    }
    const int rowTop = (relativeY / s_inlineRowHeight) * s_inlineRowHeight;
    int editorY = listPos.y + area.y + rowTop + 1;

    if (editorX < listPos.x + area.x + 1)
    {
        editorX = listPos.x + area.x + 1;
    }
    if (editorY < listPos.y + area.y + 1)
    {
        editorY = listPos.y + area.y + 1;
    }
    if (editorX + editorW > listPos.x + area.x + static_cast<int>(area.width))
    {
        editorX = listPos.x + area.x + static_cast<int>(area.width) - editorW - 1;
    }
    if (editorY + editorH > listPos.y + area.y + static_cast<int>(area.height))
    {
        editorY = listPos.y + area.y + static_cast<int>(area.height) - editorH - 1;
    }

    m_inlineEditor->move(nana::rectangle{
        editorX,
        editorY,
        static_cast<unsigned>(editorW),
        static_cast<unsigned>(editorH)
    });

    m_inlineEditor->caption(m_model->CellData(row, col));
    m_inlineEditor->show();
    m_inlineEditor->select(true);
    m_inlineEditor->focus();
}

void ResourcesTableWidget::SaveInlineEdit()
{
    if (m_isClosing || m_editRow < 0 || m_editCol < 0 || !m_model || !m_inlineEditor)
    {
        return;
    }

    const std::string value = m_inlineEditor->caption();
    m_model->SetCellData(m_editRow, m_editCol, value);

    m_inlineEditor->hide();
    m_editRow = -1;
    m_editCol = -1;
    m_inlineDirty = false;
    RefreshListboxItems();
}

void ResourcesTableWidget::CancelInlineEdit()
{
    if (!m_inlineEditor)
    {
        return;
    }

    if (m_editRow < 0)
    {
        return;
    }

    m_inlineEditor->hide();
    m_editRow = -1;
    m_editCol = -1;
    m_inlineDirty = false;
}

void ResourcesTableWidget::HandleInlineFocusLost()
{
    if (!m_inlineEditor || !m_inlineEditor->visible())
    {
        return;
    }

    if (m_isClosing)
    {
        CancelInlineEdit();
        return;
    }

    if (m_editRow < 0)
    {
        return;
    }

    if (!m_inlineDirty)
    {
        CancelInlineEdit();
        return;
    }

    nana::msgbox prompt(*m_parent, "Unsaved changes", nana::msgbox::yes_no_cancel);
    prompt.icon(nana::msgbox::icon_question);
    prompt << "Save changes?";
    const auto pick = prompt.show();

    if (pick == nana::msgbox::pick_yes)
    {
        SaveInlineEdit();
    }
    else if (pick == nana::msgbox::pick_no)
    {
        CancelInlineEdit();
    }
    else
    {
        if (!m_isClosing && m_parent && m_parent->visible())
        {
            m_inlineEditor->focus();
        }
    }
}

void ResourcesTableWidget::BeginBigEdit(int row, int col)
{
    if (m_isClosing || !m_model || !m_parent)
    {
        return;
    }

    nana::form popup(*m_parent, nana::size{ 560, 320 });
    popup.caption("Edit Value");

    nana::textbox editor(popup, nana::rectangle{ 12, 12, 536u, 252u });
    editor.multi_lines(true);
    editor.caption(m_model->CellData(row, col));

    bool saveRequested = false;
    std::string savedValue;

    nana::button btnCancel(popup, nana::rectangle{ 360, 274, 90u, 32u });
    btnCancel.caption("Cancel");
    btnCancel.events().click([&popup]()
    {
        popup.close();
    });

    nana::button btnSave(popup, nana::rectangle{ 458, 274, 90u, 32u });
    btnSave.caption("Save");
    btnSave.events().click([&popup, &saveRequested, &savedValue, &editor]()
    {
        savedValue = editor.caption();
        saveRequested = true;
        popup.close();
    });

    m_bigEditRow = row;
    m_bigEditCol = col;

    popup.show();
    editor.focus();
    popup.modality();

    if (saveRequested)
    {
        m_model->SetCellData(row, col, savedValue);
        RefreshListboxItems();
    }

    m_bigEditRow = -1;
    m_bigEditCol = -1;
}

void ResourcesTableWidget::OnAdd()
{
    if (m_isClosing || !m_model || !m_listbox)
    {
        return;
    }

    const int row = m_model->AddRow();
    if (row < 0)
    {
        return;
    }

    m_selectedRow = row;
    Refresh();

    if (m_addCallback)
    {
        m_addCallback();
    }
}

void ResourcesTableWidget::OnDelete()
{
    if (m_isClosing || m_selectedRow < 0 || !m_model || !m_listbox)
    {
        return;
    }

    nana::msgbox prompt(*m_parent, "Delete entry", nana::msgbox::yes_no);
    prompt.icon(nana::msgbox::icon_question);
    prompt << "Delete selected entry?";
    if (prompt.show() != nana::msgbox::pick_yes)
    {
        return;
    }

    if (!m_model->DeleteRow(m_selectedRow))
    {
        return;
    }

    m_selectedRow = -1;
    Refresh();

    if (m_deleteCallback)
    {
        m_deleteCallback();
    }
}
