/********************************************************************************
** Form generated from reading UI file 'NewDialogCache.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWDIALOGCACHE_H
#define UI_NEWDIALOGCACHE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NewDialogCache
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *enabled;
    QFormLayout *formLayout;
    QLabel *label_2;
    QSpinBox *number_of_sets;
    QLabel *label_3;
    QSpinBox *block_size;
    QLabel *label_5;
    QSpinBox *degree_of_associativity;
    QLabel *label_6;
    QComboBox *replacement_policy;
    QLabel *label_writeback;
    QComboBox *writeback_policy;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *NewDialogCache)
    {
        if (NewDialogCache->objectName().isEmpty())
            NewDialogCache->setObjectName(QString::fromUtf8("NewDialogCache"));
        NewDialogCache->resize(435, 204);
        verticalLayout = new QVBoxLayout(NewDialogCache);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        enabled = new QGroupBox(NewDialogCache);
        enabled->setObjectName(QString::fromUtf8("enabled"));
        enabled->setEnabled(true);
        enabled->setCheckable(true);
        formLayout = new QFormLayout(enabled);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_2 = new QLabel(enabled);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);

        number_of_sets = new QSpinBox(enabled);
        number_of_sets->setObjectName(QString::fromUtf8("number_of_sets"));
        number_of_sets->setMinimum(1);
        number_of_sets->setMaximum(1024);

        formLayout->setWidget(0, QFormLayout::FieldRole, number_of_sets);

        label_3 = new QLabel(enabled);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        block_size = new QSpinBox(enabled);
        block_size->setObjectName(QString::fromUtf8("block_size"));
        block_size->setMinimum(1);

        formLayout->setWidget(1, QFormLayout::FieldRole, block_size);

        label_5 = new QLabel(enabled);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        degree_of_associativity = new QSpinBox(enabled);
        degree_of_associativity->setObjectName(QString::fromUtf8("degree_of_associativity"));
        degree_of_associativity->setMinimum(1);

        formLayout->setWidget(2, QFormLayout::FieldRole, degree_of_associativity);

        label_6 = new QLabel(enabled);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_6);

        replacement_policy = new QComboBox(enabled);
        replacement_policy->addItem(QString());
        replacement_policy->addItem(QString());
        replacement_policy->addItem(QString());
        replacement_policy->addItem(QString());
        replacement_policy->setObjectName(QString::fromUtf8("replacement_policy"));

        formLayout->setWidget(3, QFormLayout::FieldRole, replacement_policy);

        label_writeback = new QLabel(enabled);
        label_writeback->setObjectName(QString::fromUtf8("label_writeback"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_writeback);

        writeback_policy = new QComboBox(enabled);
        writeback_policy->addItem(QString());
        writeback_policy->addItem(QString());
        writeback_policy->addItem(QString());
        writeback_policy->setObjectName(QString::fromUtf8("writeback_policy"));

        formLayout->setWidget(4, QFormLayout::FieldRole, writeback_policy);


        verticalLayout->addWidget(enabled);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(NewDialogCache);

        QMetaObject::connectSlotsByName(NewDialogCache);
    } // setupUi

    void retranslateUi(QWidget *NewDialogCache)
    {
        NewDialogCache->setWindowTitle(QCoreApplication::translate("NewDialogCache", "Form", nullptr));
        enabled->setTitle(QCoreApplication::translate("NewDialogCache", "Enable cache", nullptr));
        label_2->setText(QCoreApplication::translate("NewDialogCache", "Number of sets:", nullptr));
        label_3->setText(QCoreApplication::translate("NewDialogCache", "Block size:", nullptr));
        label_5->setText(QCoreApplication::translate("NewDialogCache", "Degree of associativity:", nullptr));
        label_6->setText(QCoreApplication::translate("NewDialogCache", "Replacement policy:", nullptr));
        replacement_policy->setItemText(0, QCoreApplication::translate("NewDialogCache", "Random", nullptr));
        replacement_policy->setItemText(1, QCoreApplication::translate("NewDialogCache", "Least Recently Used (LRU)", nullptr));
        replacement_policy->setItemText(2, QCoreApplication::translate("NewDialogCache", "Least Frequently Used (LFU)", nullptr));
        replacement_policy->setItemText(3, QCoreApplication::translate("NewDialogCache", "Pseudo Least Recently Used (PLRU)", nullptr));

        label_writeback->setText(QCoreApplication::translate("NewDialogCache", "Writeback policy:", nullptr));
        writeback_policy->setItemText(0, QCoreApplication::translate("NewDialogCache", "Write through - noallocate", nullptr));
        writeback_policy->setItemText(1, QCoreApplication::translate("NewDialogCache", "Write through - write allocate", nullptr));
        writeback_policy->setItemText(2, QCoreApplication::translate("NewDialogCache", "Write back", nullptr));

    } // retranslateUi

};

namespace Ui {
    class NewDialogCache: public Ui_NewDialogCache {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWDIALOGCACHE_H
