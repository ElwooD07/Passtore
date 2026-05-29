#pragma once
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include "Storages/IResourceStorage.h"

namespace passtore
{
    /* Modal dialog for changing the database master password. */
    class ChangePasswordDialog : public Fl_Window
    {
    public:
        explicit ChangePasswordDialog(IResourceStorage* storage);

    private:
        static void onSave(Fl_Widget*, void* ctx);
        static void onCancel(Fl_Widget*, void* ctx);

    private:
        IResourceStorage* m_storage = nullptr;
        Fl_Secret_Input* m_oldPassword = nullptr;
        Fl_Secret_Input* m_newPassword = nullptr;
        Fl_Secret_Input* m_confirmPassword = nullptr;
        Fl_Box* m_statusLabel = nullptr;
    };
}
