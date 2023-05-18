#include "dialogfileopen.h"
#include "ui_dialogfileopen.h"

DialogFileOpen::DialogFileOpen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFileOpen)
{
    ui->setupUi(this);
}

DialogFileOpen::~DialogFileOpen()
{
    delete ui;
}
