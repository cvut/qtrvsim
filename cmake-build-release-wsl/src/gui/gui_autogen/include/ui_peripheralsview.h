/********************************************************************************
** Form generated from reading UI file 'peripheralsview.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PERIPHERALSVIEW_H
#define UI_PERIPHERALSVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PeripheralsView
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_7;
    QLabel *label;
    QLabel *labelRgb1;
    QLineEdit *lineEditRgb1;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout_6;
    QVBoxLayout *verticalLayout_8;
    QLabel *label_2;
    QLabel *labelRgb2;
    QLineEdit *lineEditRgb2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *checkRed;
    QDial *dialRed;
    QLabel *label_3;
    QSpinBox *spinRed;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *checkGreen;
    QDial *dialGreen;
    QLabel *label_4;
    QSpinBox *spinGreen;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *checkBlue;
    QDial *dialBlue;
    QLabel *label_5;
    QSpinBox *spinBlue;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_9;
    QLabel *label_6;
    QLineEdit *lineEditHex;
    QSpacerItem *horizontalSpacer_4;
    QVBoxLayout *verticalLayout_10;
    QLabel *label_7;
    QLineEdit *lineEditDec;
    QVBoxLayout *verticalLayout_11;
    QLabel *label_8;
    QLineEdit *lineEditBin;

    void setupUi(QWidget *PeripheralsView)
    {
        if (PeripheralsView->objectName().isEmpty())
            PeripheralsView->setObjectName(QString::fromUtf8("PeripheralsView"));
        PeripheralsView->resize(573, 484);
        gridLayout = new QGridLayout(PeripheralsView);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        label = new QLabel(PeripheralsView);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_7->addWidget(label);

        labelRgb1 = new QLabel(PeripheralsView);
        labelRgb1->setObjectName(QString::fromUtf8("labelRgb1"));
        labelRgb1->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        verticalLayout_7->addWidget(labelRgb1);

        lineEditRgb1 = new QLineEdit(PeripheralsView);
        lineEditRgb1->setObjectName(QString::fromUtf8("lineEditRgb1"));
        lineEditRgb1->setReadOnly(true);

        verticalLayout_7->addWidget(lineEditRgb1);


        verticalLayout_5->addLayout(verticalLayout_7);


        horizontalLayout_3->addLayout(verticalLayout_5);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        label_2 = new QLabel(PeripheralsView);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout_8->addWidget(label_2);

        labelRgb2 = new QLabel(PeripheralsView);
        labelRgb2->setObjectName(QString::fromUtf8("labelRgb2"));
        labelRgb2->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        verticalLayout_8->addWidget(labelRgb2);

        lineEditRgb2 = new QLineEdit(PeripheralsView);
        lineEditRgb2->setObjectName(QString::fromUtf8("lineEditRgb2"));
        lineEditRgb2->setReadOnly(true);

        verticalLayout_8->addWidget(lineEditRgb2);


        verticalLayout_6->addLayout(verticalLayout_8);


        horizontalLayout_3->addLayout(verticalLayout_6);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        checkRed = new QCheckBox(PeripheralsView);
        checkRed->setObjectName(QString::fromUtf8("checkRed"));

        verticalLayout_2->addWidget(checkRed);

        dialRed = new QDial(PeripheralsView);
        dialRed->setObjectName(QString::fromUtf8("dialRed"));
        dialRed->setMaximum(255);
        dialRed->setWrapping(true);

        verticalLayout_2->addWidget(dialRed);

        label_3 = new QLabel(PeripheralsView);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_3);

        spinRed = new QSpinBox(PeripheralsView);
        spinRed->setObjectName(QString::fromUtf8("spinRed"));
        spinRed->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        spinRed->setMaximum(255);

        verticalLayout_2->addWidget(spinRed);


        horizontalLayout->addLayout(verticalLayout_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        checkGreen = new QCheckBox(PeripheralsView);
        checkGreen->setObjectName(QString::fromUtf8("checkGreen"));

        verticalLayout_3->addWidget(checkGreen);

        dialGreen = new QDial(PeripheralsView);
        dialGreen->setObjectName(QString::fromUtf8("dialGreen"));
        dialGreen->setMaximum(255);
        dialGreen->setWrapping(true);

        verticalLayout_3->addWidget(dialGreen);

        label_4 = new QLabel(PeripheralsView);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_4);

        spinGreen = new QSpinBox(PeripheralsView);
        spinGreen->setObjectName(QString::fromUtf8("spinGreen"));
        spinGreen->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        spinGreen->setMaximum(255);

        verticalLayout_3->addWidget(spinGreen);


        horizontalLayout->addLayout(verticalLayout_3);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        checkBlue = new QCheckBox(PeripheralsView);
        checkBlue->setObjectName(QString::fromUtf8("checkBlue"));

        verticalLayout_4->addWidget(checkBlue);

        dialBlue = new QDial(PeripheralsView);
        dialBlue->setObjectName(QString::fromUtf8("dialBlue"));
        dialBlue->setMaximum(255);
        dialBlue->setWrapping(true);

        verticalLayout_4->addWidget(dialBlue);

        label_5 = new QLabel(PeripheralsView);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(label_5);

        spinBlue = new QSpinBox(PeripheralsView);
        spinBlue->setObjectName(QString::fromUtf8("spinBlue"));
        spinBlue->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        spinBlue->setMaximum(255);

        verticalLayout_4->addWidget(spinBlue);


        horizontalLayout->addLayout(verticalLayout_4);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        label_6 = new QLabel(PeripheralsView);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        verticalLayout_9->addWidget(label_6);

        lineEditHex = new QLineEdit(PeripheralsView);
        lineEditHex->setObjectName(QString::fromUtf8("lineEditHex"));
        lineEditHex->setAlignment(Qt::AlignCenter);
        lineEditHex->setReadOnly(true);

        verticalLayout_9->addWidget(lineEditHex);


        horizontalLayout_2->addLayout(verticalLayout_9);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        label_7 = new QLabel(PeripheralsView);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        verticalLayout_10->addWidget(label_7);

        lineEditDec = new QLineEdit(PeripheralsView);
        lineEditDec->setObjectName(QString::fromUtf8("lineEditDec"));
        lineEditDec->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditDec->setReadOnly(true);

        verticalLayout_10->addWidget(lineEditDec);


        horizontalLayout_2->addLayout(verticalLayout_10);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));

        horizontalLayout_2->addLayout(verticalLayout_11);


        verticalLayout->addLayout(horizontalLayout_2);

        label_8 = new QLabel(PeripheralsView);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);

        verticalLayout->addWidget(label_8);

        lineEditBin = new QLineEdit(PeripheralsView);
        lineEditBin->setObjectName(QString::fromUtf8("lineEditBin"));
        lineEditBin->setAlignment(Qt::AlignCenter);
        lineEditBin->setReadOnly(true);

        verticalLayout->addWidget(lineEditBin);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(PeripheralsView);

        QMetaObject::connectSlotsByName(PeripheralsView);
    } // setupUi

    void retranslateUi(QWidget *PeripheralsView)
    {
        PeripheralsView->setWindowTitle(QCoreApplication::translate("PeripheralsView", "Form", nullptr));
        label->setText(QCoreApplication::translate("PeripheralsView", "LED RGB 1", nullptr));
        labelRgb1->setText(QString());
        label_2->setText(QCoreApplication::translate("PeripheralsView", "LED RGB 2", nullptr));
        labelRgb2->setText(QString());
        checkRed->setText(QString());
        label_3->setText(QCoreApplication::translate("PeripheralsView", "Red Knob", nullptr));
        checkGreen->setText(QString());
        label_4->setText(QCoreApplication::translate("PeripheralsView", "Green Knob", nullptr));
        checkBlue->setText(QString());
        label_5->setText(QCoreApplication::translate("PeripheralsView", "Blue Knob", nullptr));
        label_6->setText(QCoreApplication::translate("PeripheralsView", "Word hexadecimal", nullptr));
        lineEditHex->setText(QString());
        label_7->setText(QCoreApplication::translate("PeripheralsView", "Word decimal", nullptr));
        label_8->setText(QCoreApplication::translate("PeripheralsView", "Word binary", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PeripheralsView: public Ui_PeripheralsView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PERIPHERALSVIEW_H
