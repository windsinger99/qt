#include "ctrldialog.h"
#include "ui_ctrldialog.h"
#include "mainwindow.h"
#include <QPushButton>

CtrlDialog::CtrlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CtrlDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(Accepted()));
}

CtrlDialog::~CtrlDialog()
{
    delete ui;
}

void CtrlDialog::Accepted()
{
    //usb_check_n_open(); //FIXME
}

void CtrlDialog::Rejected()
{
}

