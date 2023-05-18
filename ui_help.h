/********************************************************************************
** Form generated from reading UI file 'help.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HELP_H
#define UI_HELP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CtrlDialog
{
public:
    QPushButton *pushButton;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_5;

    void setupUi(QDialog *CtrlDialog)
    {
        if (CtrlDialog->objectName().isEmpty())
            CtrlDialog->setObjectName(QStringLiteral("CtrlDialog"));
        CtrlDialog->resize(282, 259);
        pushButton = new QPushButton(CtrlDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(90, 200, 93, 28));
        layoutWidget = new QWidget(CtrlDialog);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(40, 30, 214, 151));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);


        verticalLayout_2->addLayout(verticalLayout);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_2->addWidget(label_3);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout_2->addWidget(label_4);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        verticalLayout_2->addWidget(label_6);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        verticalLayout_2->addWidget(label_7);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout_2->addWidget(label_5);


        retranslateUi(CtrlDialog);

        QMetaObject::connectSlotsByName(CtrlDialog);
    } // setupUi

    void retranslateUi(QDialog *CtrlDialog)
    {
        CtrlDialog->setWindowTitle(QApplication::translate("CtrlDialog", "Dialog", Q_NULLPTR));
        pushButton->setText(QApplication::translate("CtrlDialog", "OK", Q_NULLPTR));
        label->setText(QApplication::translate("CtrlDialog", "'O' : Open USB", Q_NULLPTR));
        label_2->setText(QApplication::translate("CtrlDialog", "'Z' : Open Captured File", Q_NULLPTR));
        label_3->setText(QApplication::translate("CtrlDialog", "'B' : Brush, Pen&Marker Mode", Q_NULLPTR));
        label_4->setText(QApplication::translate("CtrlDialog", "'W' : Capture Start/Stop", Q_NULLPTR));
        label_6->setText(QApplication::translate("CtrlDialog", "'P' : Play/Pause", Q_NULLPTR));
        label_7->setText(QApplication::translate("CtrlDialog", "'1' : One Step", Q_NULLPTR));
        label_5->setText(QApplication::translate("CtrlDialog", "'ESC' : Exit", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CtrlDialog: public Ui_CtrlDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HELP_H
