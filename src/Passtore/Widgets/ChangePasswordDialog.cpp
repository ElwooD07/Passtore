#include "pch.h"
#include "ChangePasswordDialog.h"

using namespace passtore;

static const int s_dlgW = 340;
static const int s_dlgH = 200;
static const int s_lblW = 120;
static const int s_fldX = 130;
static const int s_fldW = s_dlgW - s_fldX - 10;
static const int s_rowH = 28;
static const int s_pad  = 10;
static const int s_btnW = 80;
static const int s_btnH = 28;

ChangePasswordDialog::ChangePasswordDialog(IResourceStorage* storage)
    : Fl_Window(s_dlgW, s_dlgH, "Change Password")
    , m_storage(storage)
{
    begin();

    int y = s_pad;

    new Fl_Box(s_pad, y, s_lblW, s_rowH, "Old password:");
    m_oldPassword = new Fl_Secret_Input(s_fldX, y, s_fldW, s_rowH);
    y += s_rowH + s_pad;

    new Fl_Box(s_pad, y, s_lblW, s_rowH, "New password:");
    m_newPassword = new Fl_Secret_Input(s_fldX, y, s_fldW, s_rowH);
    y += s_rowH + s_pad;

    new Fl_Box(s_pad, y, s_lblW, s_rowH, "Confirm:");
    m_confirmPassword = new Fl_Secret_Input(s_fldX, y, s_fldW, s_rowH);
    y += s_rowH + s_pad;

    m_statusLabel = new Fl_Box(s_pad, y, s_dlgW - s_pad * 2, s_rowH, "");
    m_statusLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    int btnY = s_dlgH - s_btnH - s_pad;
    auto* btnSave   = new Fl_Button(s_dlgW - s_btnW * 2 - s_pad * 2, btnY, s_btnW, s_btnH, "Save");
    btnSave->callback(onSave, this);

    auto* btnCancel = new Fl_Button(s_dlgW - s_btnW - s_pad, btnY, s_btnW, s_btnH, "Cancel");
    btnCancel->callback(onCancel, this);

    end();
    set_modal();
}

void ChangePasswordDialog::onSave(Fl_Widget*, void* ctx)
{
    auto* self = static_cast<ChangePasswordDialog*>(ctx);

    std::string oldPwd(self->m_oldPassword->value());
    std::string newPwd(self->m_newPassword->value());
    std::string confirm(self->m_confirmPassword->value());

    auto wipe = [&]()
    {
        std::fill(oldPwd.begin(), oldPwd.end(), '\0');
        std::fill(newPwd.begin(), newPwd.end(), '\0');
        std::fill(confirm.begin(), confirm.end(), '\0');
    };

    if (newPwd.empty())
    {
        self->m_statusLabel->copy_label("New password cannot be empty.");
        self->m_statusLabel->redraw();
        wipe();
        return;
    }

    if (newPwd != confirm)
    {
        self->m_statusLabel->copy_label("Passwords do not match.");
        self->m_statusLabel->redraw();
        wipe();
        return;
    }

    try
    {
        self->m_storage->ChangePassword(oldPwd, newPwd);
        wipe();
        self->hide();
    }
    catch (const std::exception& ex)
    {
        wipe();
        self->m_statusLabel->copy_label(ex.what());
        self->m_statusLabel->redraw();
    }
}

void ChangePasswordDialog::onCancel(Fl_Widget*, void* ctx)
{
    static_cast<ChangePasswordDialog*>(ctx)->hide();
}
