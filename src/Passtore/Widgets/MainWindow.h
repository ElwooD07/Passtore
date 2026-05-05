#pragma once
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Button.H>
#include "Storages/IResourceStorage.h"

namespace passtore
{
    class ResourceTableModel;
    class ResourcesListWidget;

    class MainWindow : public Fl_Window
    {
    public:
        explicit MainWindow(IResourceStorage* storage);

    private:
        static void onError(void* ctx, const std::string& message);
        static void onSettings(Fl_Widget*, void* ctx);
        static void onAdd(Fl_Widget*, void* ctx);
        static void onDelete(Fl_Widget*, void* ctx);

    private:
        IResourceStorage*    m_storage;
        ResourceTableModel*  m_model;
        ResourcesListWidget* m_table;
    };
}
