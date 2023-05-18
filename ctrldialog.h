#ifndef CTRLDIALOG_H
#define CTRLDIALOG_H

#include <QDialog>

namespace Ui {
class CtrlDialog;
}

class CtrlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CtrlDialog(QWidget *parent = 0);
    ~CtrlDialog();

private:
    Ui::CtrlDialog *ui;

private slots:
    void Accepted();
    void Rejected();
};

#endif // CTRLDIALOG_H
