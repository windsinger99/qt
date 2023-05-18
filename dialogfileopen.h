#ifndef DIALOGFILEOPEN_H
#define DIALOGFILEOPEN_H

#include <QDialog>

namespace Ui {
class DialogFileOpen;
}

class DialogFileOpen : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFileOpen(QWidget *parent = 0);
    ~DialogFileOpen();

private:
    Ui::DialogFileOpen *ui;
};

#endif // DIALOGFILEOPEN_H
