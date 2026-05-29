#include "pch.h"
#include "ChangePasswordDialog.h"

using namespace passtore;

static const int DLG_W  = 340;
static const int DLG_H  = 200;
static const int LBL_W  = 120;
static const int FLD_X  = 130;
static const int FLD_W  = DLG_W - FLD_X - 10;
static const int ROW_H  = 28;
static const int PAD    = 10;
static const int BTN_W  = 80;
static const int BTN_H  = 28;

ChangePasswordDialog::ChangePasswordDialog(IResourceStorage* storage)
    : Fl_Window(DLG_W, DLG_H, "Change Password")
    , m_storage(storage)
{
    begin();

    int y = PAD;

    new Fl_Box(PAD, y, LBL_W, ROW_H, "Old password:");
    m_oldPassword = new Fl_Secret_Input(FLD_X, y, FLD_W, ROW_H);
    y += ROW_H + PAD;

    new Fl_Box(PAD, y, LBL_W, ROW_H, "New password:");
    m_newPassword = new Fl_Secret_Input(FLD_X, y, FLD_W, ROW_H);
    y += ROW_H + PAD;

    new Fl_Box(PAD, y, LBL_W, ROW_H, "Confirm:");
    m_confirmPassword = new Fl_Secret_Input(FLD_X, y, FLD_W, ROW_H);
    y += ROW_H + PAD;

    m_statusLabel = new Fl_Box(PAD, y, DLG_W - PAD * 2, ROW_H, "");
    m_statusLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    int btnY = DLG_H - BTN_H - PAD;
    auto* btnSave   = new Fl_Button(DLG_W - BTN_W * 2 - PAD * 2, btnY, BTN_W, BTN_H, "Save");
    btnSave->callback(onSave, this);

    auto* btnCancel = new Fl_Button(DLG_W - BTN_W - PAD, btnY, BTN_W, BTN_H, "Cancel");
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
