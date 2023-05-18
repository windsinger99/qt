/********************************************************************************
** Form generated from reading UI file 'ctrldialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CTRLDIALOG_H
#define UI_CTRLDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_CtrlDialog
{
public:
    QDialogButtonBox *buttonBox;
    QRadioButton *radioButton_text;
    QRadioButton *radioButton_graphic;
    QRadioButton *radioButton_write;
    QRadioButton *radioButton_read;
    QRadioButton *radioButton_normal;
    QCheckBox *checkBox_inverse;
    QGroupBox *groupBox;
    QGroupBox *groupBox_2;
    QTextEdit *textEdit_vid;
    QLabel *label;
    QLabel *label_2;
    QTextEdit *textEdit_vid_2;

    void setupUi(QDialog *CtrlDialog)
    {
        if (CtrlDialog->objectName().isEmpty())
            CtrlDialog->setObjectName(QStringLiteral("CtrlDialog"));
        CtrlDialog->resize(341, 292);
        buttonBox = new QDialogButtonBox(CtrlDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(0, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        radioButton_text = new QRadioButton(CtrlDialog);
        radioButton_text->setObjectName(QStringLiteral("radioButton_text"));
        radioButton_text->setGeometry(QRect(50, 50, 71, 19));
        radioButton_graphic = new QRadioButton(CtrlDialog);
        radioButton_graphic->setObjectName(QStringLiteral("radioButton_graphic"));
        radioButton_graphic->setGeometry(QRect(120, 50, 81, 19));
        radioButton_write = new QRadioButton(CtrlDialog);
        radioButton_write->setObjectName(QStringLiteral("radioButton_write"));
        radioButton_write->setGeometry(QRect(50, 130, 81, 19));
        radioButton_read = new QRadioButton(CtrlDialog);
        radioButton_read->setObjectName(QStringLiteral("radioButton_read"));
        radioButton_read->setGeometry(QRect(130, 130, 81, 19));
        radioButton_normal = new QRadioButton(CtrlDialog);
        radioButton_normal->setObjectName(QStringLiteral("radioButton_normal"));
        radioButton_normal->setGeometry(QRect(210, 130, 81, 19));
        checkBox_inverse = new QCheckBox(CtrlDialog);
        checkBox_inverse->setObjectName(QStringLiteral("checkBox_inverse"));
        checkBox_inverse->setGeometry(QRect(220, 50, 96, 19));
        groupBox = new QGroupBox(CtrlDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(30, 20, 281, 71));
        groupBox_2 = new QGroupBox(CtrlDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(30, 100, 281, 71));
        textEdit_vid = new QTextEdit(CtrlDialog);
        textEdit_vid->setObjectName(QStringLiteral("textEdit_vid"));
        textEdit_vid->setGeometry(QRect(55, 183, 104, 31));
        label = new QLabel(CtrlDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 190, 31, 16));
        label_2 = new QLabel(CtrlDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(170, 190, 31, 16));
        textEdit_vid_2 = new QTextEdit(CtrlDialog);
        textEdit_vid_2->setObjectName(QStringLiteral("textEdit_vid_2"));
        textEdit_vid_2->setGeometry(QRect(200, 180, 104, 31));

        retranslateUi(CtrlDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), CtrlDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CtrlDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(CtrlDialog);
    } // setupUi

    void retranslateUi(QDialog *CtrlDialog)
    {
        CtrlDialog->setWindowTitle(QApplication::translate("CtrlDialog", "Dialog", Q_NULLPTR));
        radioButton_text->setText(QApplication::translate("CtrlDialog", "Text", Q_NULLPTR));
        radioButton_graphic->setText(QApplication::translate("CtrlDialog", "Graphic", Q_NULLPTR));
        radioButton_write->setText(QApplication::translate("CtrlDialog", "Write", Q_NULLPTR));
        radioButton_read->setText(QApplication::translate("CtrlDialog", "Read", Q_NULLPTR));
        radioButton_normal->setText(QApplication::translate("CtrlDialog", "Normal", Q_NULLPTR));
        checkBox_inverse->setText(QApplication::translate("CtrlDialog", "Inverse", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("CtrlDialog", "Display", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("CtrlDialog", "File", Q_NULLPTR));
        label->setText(QApplication::translate("CtrlDialog", "Vid", Q_NULLPTR));
        label_2->setText(QApplication::translate("CtrlDialog", "Pid", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CtrlDialog: public Ui_CtrlDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CTRLDIALOG_H
