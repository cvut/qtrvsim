/********************************************************************************
** Form generated from reading UI file 'NewDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWDIALOG_H
#define UI_NEWDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "gui/dialogs/new/bigslider.h"

QT_BEGIN_NAMESPACE

class Ui_NewDialog
{
public:
    QVBoxLayout *verticalLayout_1;
    QHBoxLayout *horizontalLayout_4;
    QTreeWidget *page_select_tree;
    QVBoxLayout *verticalLayout;
    QLabel *config_page_title;
    QStackedWidget *config_pages;
    QWidget *tab_preset_load;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *preset_box;
    QVBoxLayout *verticalLayout_6;
    QRadioButton *preset_no_pipeline;
    QRadioButton *preset_no_pipeline_cache;
    QRadioButton *preset_pipelined_bare;
    QRadioButton *preset_pipelined;
    QRadioButton *preset_custom;
    QCheckBox *reset_at_compile;
    QSpacerItem *verticalSpacer_3;
    QFrame *line_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QLineEdit *elf_file;
    QPushButton *pushButton_browse;
    QWidget *tab_core;
    QVBoxLayout *verticalLayout_8;
    QGridLayout *gridLayoutIsa;
    QCheckBox *pipelined;
    QCheckBox *xlen_64bit;
    QCheckBox *isa_atomic;
    QCheckBox *delay_slot;
    QCheckBox *isa_multiply;
    QGroupBox *hazard_unit;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *hazard_stall;
    QRadioButton *hazard_stall_forward;
    QSpacerItem *verticalSpacer_1;
    QWidget *tab_branch_predictor;
    QVBoxLayout *verticalLayout_10;
    QGroupBox *group_bp;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *layout_bp_type;
    QLabel *text_bp_type;
    QComboBox *select_bp_type;
    QHBoxLayout *layout_bp_init_state;
    QLabel *text_bp_init_state;
    QComboBox *select_bp_init_state;
    QGroupBox *group_bp_btb;
    QGridLayout *gridLayout;
    QFrame *line_bp_btb;
    QLabel *text_bp_btb_addr_bits_number;
    QLabel *text_bp_btb_addr_bits;
    QSlider *slider_bp_btb_addr_bits;
    QHBoxLayout *layout_bp_btb_status;
    QLabel *text_bp_btb_entries;
    QLabel *text_bp_btb_entries_number;
    QSpacerItem *spacer_bp_btb_status;
    QFrame *line_bp_btb_status;
    QLabel *text_bp_btb_bits;
    QLabel *text_bp_btb_bits_number;
    QGroupBox *group_bp_bht;
    QGridLayout *gridLayout_2;
    QLabel *text_bp_bht_bhr_bits_number;
    QLabel *text_bp_bht_addr_bits_number;
    QSlider *slider_bp_bht_bhr_bits;
    QSlider *slider_bp_bht_addr_bits;
    QLabel *text_bp_bht_bhr_bits;
    QFrame *line_bp_bht;
    QLabel *text_bp_bht_addr_bits;
    QHBoxLayout *layout_bp_bht_status;
    QLabel *text_bp_bht_entries;
    QLabel *text_bp_bht_entries_number;
    QSpacerItem *spacer_bp_bht_status;
    QFrame *line_bp_bht_status;
    QLabel *text_bp_bht_bits;
    QLabel *text_bp_bht_bits_number;
    QSpacerItem *verticalSpacer_5;
    QWidget *tab_virtual_memory;
    QGroupBox *group_vm;
    QRadioButton *radioButton_bare;
    QRadioButton *radioButton_sv32;
    QLabel *label_mode;
    QLineEdit *lineEdit_kernel_base;
    BigSlider *horizontalSlider_kernel_base;
    QLabel *label_kernel_base;
    QLineEdit *lineEdit_root_ppn;
    QLabel *label_root_ppn;
    BigSlider *horizontalSlider_root_ppn;
    QWidget *tab_memory;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *mem_protec_write;
    QCheckBox *mem_protec_exec;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_read;
    QSpinBox *mem_time_read;
    QLabel *label_write;
    QSpinBox *mem_time_write;
    QLabel *label_burts_enable;
    QCheckBox *mem_enable_burst;
    QLabel *label_burts;
    QSpinBox *mem_time_burst;
    QLabel *label_level2;
    QSpinBox *mem_time_level2;
    QSpacerItem *verticalSpacer_2;
    QWidget *tab_cache_program;
    QWidget *tab_cache_data;
    QWidget *tab_cache_level2;
    QWidget *tab_os_emulation;
    QVBoxLayout *verticalLayout_7;
    QCheckBox *osemu_enable;
    QCheckBox *osemu_known_syscall_stop;
    QCheckBox *osemu_unknown_syscall_stop;
    QCheckBox *osemu_interrupt_stop;
    QCheckBox *osemu_exception_stop;
    QHBoxLayout *horizontalLayout;
    QLabel *label_fs_root;
    QLineEdit *osemu_fs_root;
    QPushButton *osemu_fs_root_browse;
    QSpacerItem *verticalSpacer_4;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_example;
    QPushButton *pushButton_start_empty;
    QPushButton *pushButton_load;
    QPushButton *pushButton_cancel;

    void setupUi(QDialog *NewDialog)
    {
        if (NewDialog->objectName().isEmpty())
            NewDialog->setObjectName(QString::fromUtf8("NewDialog"));
        NewDialog->resize(745, 472);
        NewDialog->setModal(true);
        verticalLayout_1 = new QVBoxLayout(NewDialog);
        verticalLayout_1->setObjectName(QString::fromUtf8("verticalLayout_1"));
        verticalLayout_1->setContentsMargins(0, 0, 0, -1);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        page_select_tree = new QTreeWidget(NewDialog);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("Config Pages"));
        page_select_tree->setHeaderItem(__qtreewidgetitem);
        page_select_tree->setObjectName(QString::fromUtf8("page_select_tree"));
        page_select_tree->setMinimumSize(QSize(135, 0));

        horizontalLayout_4->addWidget(page_select_tree);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 4, 0, -1);
        config_page_title = new QLabel(NewDialog);
        config_page_title->setObjectName(QString::fromUtf8("config_page_title"));

        verticalLayout->addWidget(config_page_title);

        config_pages = new QStackedWidget(NewDialog);
        config_pages->setObjectName(QString::fromUtf8("config_pages"));
        tab_preset_load = new QWidget();
        tab_preset_load->setObjectName(QString::fromUtf8("tab_preset_load"));
        verticalLayout_5 = new QVBoxLayout(tab_preset_load);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        preset_box = new QGroupBox(tab_preset_load);
        preset_box->setObjectName(QString::fromUtf8("preset_box"));
        verticalLayout_6 = new QVBoxLayout(preset_box);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        preset_no_pipeline = new QRadioButton(preset_box);
        preset_no_pipeline->setObjectName(QString::fromUtf8("preset_no_pipeline"));

        verticalLayout_6->addWidget(preset_no_pipeline);

        preset_no_pipeline_cache = new QRadioButton(preset_box);
        preset_no_pipeline_cache->setObjectName(QString::fromUtf8("preset_no_pipeline_cache"));

        verticalLayout_6->addWidget(preset_no_pipeline_cache);

        preset_pipelined_bare = new QRadioButton(preset_box);
        preset_pipelined_bare->setObjectName(QString::fromUtf8("preset_pipelined_bare"));

        verticalLayout_6->addWidget(preset_pipelined_bare);

        preset_pipelined = new QRadioButton(preset_box);
        preset_pipelined->setObjectName(QString::fromUtf8("preset_pipelined"));

        verticalLayout_6->addWidget(preset_pipelined);

        preset_custom = new QRadioButton(preset_box);
        preset_custom->setObjectName(QString::fromUtf8("preset_custom"));

        verticalLayout_6->addWidget(preset_custom);


        verticalLayout_5->addWidget(preset_box);

        reset_at_compile = new QCheckBox(tab_preset_load);
        reset_at_compile->setObjectName(QString::fromUtf8("reset_at_compile"));

        verticalLayout_5->addWidget(reset_at_compile);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_3);

        line_2 = new QFrame(tab_preset_load);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_5->addWidget(line_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label = new QLabel(tab_preset_load);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_3->addWidget(label);

        elf_file = new QLineEdit(tab_preset_load);
        elf_file->setObjectName(QString::fromUtf8("elf_file"));

        horizontalLayout_3->addWidget(elf_file);

        pushButton_browse = new QPushButton(tab_preset_load);
        pushButton_browse->setObjectName(QString::fromUtf8("pushButton_browse"));

        horizontalLayout_3->addWidget(pushButton_browse);


        verticalLayout_5->addLayout(horizontalLayout_3);

        config_pages->addWidget(tab_preset_load);
        tab_core = new QWidget();
        tab_core->setObjectName(QString::fromUtf8("tab_core"));
        verticalLayout_8 = new QVBoxLayout(tab_core);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        gridLayoutIsa = new QGridLayout();
        gridLayoutIsa->setObjectName(QString::fromUtf8("gridLayoutIsa"));
        pipelined = new QCheckBox(tab_core);
        pipelined->setObjectName(QString::fromUtf8("pipelined"));

        gridLayoutIsa->addWidget(pipelined, 0, 0, 1, 1);

        xlen_64bit = new QCheckBox(tab_core);
        xlen_64bit->setObjectName(QString::fromUtf8("xlen_64bit"));

        gridLayoutIsa->addWidget(xlen_64bit, 0, 1, 1, 1);

        isa_atomic = new QCheckBox(tab_core);
        isa_atomic->setObjectName(QString::fromUtf8("isa_atomic"));

        gridLayoutIsa->addWidget(isa_atomic, 0, 2, 1, 1);

        delay_slot = new QCheckBox(tab_core);
        delay_slot->setObjectName(QString::fromUtf8("delay_slot"));

        gridLayoutIsa->addWidget(delay_slot, 1, 0, 1, 1);

        isa_multiply = new QCheckBox(tab_core);
        isa_multiply->setObjectName(QString::fromUtf8("isa_multiply"));

        gridLayoutIsa->addWidget(isa_multiply, 1, 2, 1, 1);


        verticalLayout_8->addLayout(gridLayoutIsa);

        hazard_unit = new QGroupBox(tab_core);
        hazard_unit->setObjectName(QString::fromUtf8("hazard_unit"));
        hazard_unit->setCheckable(true);
        hazard_unit->setChecked(false);
        verticalLayout_3 = new QVBoxLayout(hazard_unit);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        hazard_stall = new QRadioButton(hazard_unit);
        hazard_stall->setObjectName(QString::fromUtf8("hazard_stall"));

        verticalLayout_3->addWidget(hazard_stall);

        hazard_stall_forward = new QRadioButton(hazard_unit);
        hazard_stall_forward->setObjectName(QString::fromUtf8("hazard_stall_forward"));
        hazard_stall_forward->setChecked(true);

        verticalLayout_3->addWidget(hazard_stall_forward);


        verticalLayout_8->addWidget(hazard_unit);

        verticalSpacer_1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_8->addItem(verticalSpacer_1);

        config_pages->addWidget(tab_core);
        tab_branch_predictor = new QWidget();
        tab_branch_predictor->setObjectName(QString::fromUtf8("tab_branch_predictor"));
        verticalLayout_10 = new QVBoxLayout(tab_branch_predictor);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        group_bp = new QGroupBox(tab_branch_predictor);
        group_bp->setObjectName(QString::fromUtf8("group_bp"));
        group_bp->setEnabled(true);
        group_bp->setCheckable(true);
        group_bp->setChecked(true);
        verticalLayout_9 = new QVBoxLayout(group_bp);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        layout_bp_type = new QHBoxLayout();
        layout_bp_type->setObjectName(QString::fromUtf8("layout_bp_type"));
        layout_bp_type->setContentsMargins(-1, 0, -1, -1);
        text_bp_type = new QLabel(group_bp);
        text_bp_type->setObjectName(QString::fromUtf8("text_bp_type"));

        layout_bp_type->addWidget(text_bp_type);

        select_bp_type = new QComboBox(group_bp);
        select_bp_type->setObjectName(QString::fromUtf8("select_bp_type"));

        layout_bp_type->addWidget(select_bp_type);


        verticalLayout_9->addLayout(layout_bp_type);

        layout_bp_init_state = new QHBoxLayout();
        layout_bp_init_state->setObjectName(QString::fromUtf8("layout_bp_init_state"));
        layout_bp_init_state->setContentsMargins(-1, 0, -1, -1);
        text_bp_init_state = new QLabel(group_bp);
        text_bp_init_state->setObjectName(QString::fromUtf8("text_bp_init_state"));

        layout_bp_init_state->addWidget(text_bp_init_state);

        select_bp_init_state = new QComboBox(group_bp);
        select_bp_init_state->setObjectName(QString::fromUtf8("select_bp_init_state"));

        layout_bp_init_state->addWidget(select_bp_init_state);


        verticalLayout_9->addLayout(layout_bp_init_state);

        group_bp_btb = new QGroupBox(group_bp);
        group_bp_btb->setObjectName(QString::fromUtf8("group_bp_btb"));
        group_bp_btb->setFlat(false);
        gridLayout = new QGridLayout(group_bp_btb);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        line_bp_btb = new QFrame(group_bp_btb);
        line_bp_btb->setObjectName(QString::fromUtf8("line_bp_btb"));
        line_bp_btb->setFrameShape(QFrame::HLine);
        line_bp_btb->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line_bp_btb, 1, 0, 1, 3);

        text_bp_btb_addr_bits_number = new QLabel(group_bp_btb);
        text_bp_btb_addr_bits_number->setObjectName(QString::fromUtf8("text_bp_btb_addr_bits_number"));
        text_bp_btb_addr_bits_number->setMinimumSize(QSize(40, 0));
        text_bp_btb_addr_bits_number->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(text_bp_btb_addr_bits_number, 0, 2, 1, 1);

        text_bp_btb_addr_bits = new QLabel(group_bp_btb);
        text_bp_btb_addr_bits->setObjectName(QString::fromUtf8("text_bp_btb_addr_bits"));

        gridLayout->addWidget(text_bp_btb_addr_bits, 0, 0, 1, 1);

        slider_bp_btb_addr_bits = new QSlider(group_bp_btb);
        slider_bp_btb_addr_bits->setObjectName(QString::fromUtf8("slider_bp_btb_addr_bits"));
        slider_bp_btb_addr_bits->setMaximum(8);
        slider_bp_btb_addr_bits->setOrientation(Qt::Horizontal);
        slider_bp_btb_addr_bits->setInvertedAppearance(false);
        slider_bp_btb_addr_bits->setTickPosition(QSlider::TicksBothSides);

        gridLayout->addWidget(slider_bp_btb_addr_bits, 0, 1, 1, 1);

        layout_bp_btb_status = new QHBoxLayout();
        layout_bp_btb_status->setObjectName(QString::fromUtf8("layout_bp_btb_status"));
        text_bp_btb_entries = new QLabel(group_bp_btb);
        text_bp_btb_entries->setObjectName(QString::fromUtf8("text_bp_btb_entries"));
        text_bp_btb_entries->setMinimumSize(QSize(140, 0));

        layout_bp_btb_status->addWidget(text_bp_btb_entries);

        text_bp_btb_entries_number = new QLabel(group_bp_btb);
        text_bp_btb_entries_number->setObjectName(QString::fromUtf8("text_bp_btb_entries_number"));
        text_bp_btb_entries_number->setMinimumSize(QSize(40, 0));
        text_bp_btb_entries_number->setAlignment(Qt::AlignCenter);

        layout_bp_btb_status->addWidget(text_bp_btb_entries_number);

        spacer_bp_btb_status = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        layout_bp_btb_status->addItem(spacer_bp_btb_status);

        line_bp_btb_status = new QFrame(group_bp_btb);
        line_bp_btb_status->setObjectName(QString::fromUtf8("line_bp_btb_status"));
        line_bp_btb_status->setFrameShape(QFrame::VLine);
        line_bp_btb_status->setFrameShadow(QFrame::Sunken);

        layout_bp_btb_status->addWidget(line_bp_btb_status);

        text_bp_btb_bits = new QLabel(group_bp_btb);
        text_bp_btb_bits->setObjectName(QString::fromUtf8("text_bp_btb_bits"));
        text_bp_btb_bits->setMinimumSize(QSize(140, 0));

        layout_bp_btb_status->addWidget(text_bp_btb_bits);

        text_bp_btb_bits_number = new QLabel(group_bp_btb);
        text_bp_btb_bits_number->setObjectName(QString::fromUtf8("text_bp_btb_bits_number"));
        text_bp_btb_bits_number->setMinimumSize(QSize(40, 0));
        text_bp_btb_bits_number->setAlignment(Qt::AlignCenter);

        layout_bp_btb_status->addWidget(text_bp_btb_bits_number);


        gridLayout->addLayout(layout_bp_btb_status, 3, 0, 1, 3);


        verticalLayout_9->addWidget(group_bp_btb);

        group_bp_bht = new QGroupBox(group_bp);
        group_bp_bht->setObjectName(QString::fromUtf8("group_bp_bht"));
        gridLayout_2 = new QGridLayout(group_bp_bht);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        text_bp_bht_bhr_bits_number = new QLabel(group_bp_bht);
        text_bp_bht_bhr_bits_number->setObjectName(QString::fromUtf8("text_bp_bht_bhr_bits_number"));
        text_bp_bht_bhr_bits_number->setMinimumSize(QSize(40, 0));
        text_bp_bht_bhr_bits_number->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(text_bp_bht_bhr_bits_number, 0, 2, 1, 1);

        text_bp_bht_addr_bits_number = new QLabel(group_bp_bht);
        text_bp_bht_addr_bits_number->setObjectName(QString::fromUtf8("text_bp_bht_addr_bits_number"));
        text_bp_bht_addr_bits_number->setMinimumSize(QSize(40, 0));
        text_bp_bht_addr_bits_number->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(text_bp_bht_addr_bits_number, 1, 2, 1, 1);

        slider_bp_bht_bhr_bits = new QSlider(group_bp_bht);
        slider_bp_bht_bhr_bits->setObjectName(QString::fromUtf8("slider_bp_bht_bhr_bits"));
        slider_bp_bht_bhr_bits->setMaximum(8);
        slider_bp_bht_bhr_bits->setOrientation(Qt::Horizontal);
        slider_bp_bht_bhr_bits->setTickPosition(QSlider::TicksBothSides);

        gridLayout_2->addWidget(slider_bp_bht_bhr_bits, 0, 1, 1, 1);

        slider_bp_bht_addr_bits = new QSlider(group_bp_bht);
        slider_bp_bht_addr_bits->setObjectName(QString::fromUtf8("slider_bp_bht_addr_bits"));
        slider_bp_bht_addr_bits->setMaximum(8);
        slider_bp_bht_addr_bits->setOrientation(Qt::Horizontal);
        slider_bp_bht_addr_bits->setTickPosition(QSlider::TicksBothSides);

        gridLayout_2->addWidget(slider_bp_bht_addr_bits, 1, 1, 1, 1);

        text_bp_bht_bhr_bits = new QLabel(group_bp_bht);
        text_bp_bht_bhr_bits->setObjectName(QString::fromUtf8("text_bp_bht_bhr_bits"));
        text_bp_bht_bhr_bits->setMinimumSize(QSize(140, 0));

        gridLayout_2->addWidget(text_bp_bht_bhr_bits, 0, 0, 1, 1);

        line_bp_bht = new QFrame(group_bp_bht);
        line_bp_bht->setObjectName(QString::fromUtf8("line_bp_bht"));
        line_bp_bht->setFrameShape(QFrame::HLine);
        line_bp_bht->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(line_bp_bht, 2, 0, 1, 3);

        text_bp_bht_addr_bits = new QLabel(group_bp_bht);
        text_bp_bht_addr_bits->setObjectName(QString::fromUtf8("text_bp_bht_addr_bits"));
        text_bp_bht_addr_bits->setMinimumSize(QSize(140, 0));

        gridLayout_2->addWidget(text_bp_bht_addr_bits, 1, 0, 1, 1);

        layout_bp_bht_status = new QHBoxLayout();
        layout_bp_bht_status->setObjectName(QString::fromUtf8("layout_bp_bht_status"));
        text_bp_bht_entries = new QLabel(group_bp_bht);
        text_bp_bht_entries->setObjectName(QString::fromUtf8("text_bp_bht_entries"));
        text_bp_bht_entries->setMinimumSize(QSize(140, 0));

        layout_bp_bht_status->addWidget(text_bp_bht_entries);

        text_bp_bht_entries_number = new QLabel(group_bp_bht);
        text_bp_bht_entries_number->setObjectName(QString::fromUtf8("text_bp_bht_entries_number"));
        text_bp_bht_entries_number->setMinimumSize(QSize(40, 0));
        text_bp_bht_entries_number->setAlignment(Qt::AlignCenter);

        layout_bp_bht_status->addWidget(text_bp_bht_entries_number);

        spacer_bp_bht_status = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        layout_bp_bht_status->addItem(spacer_bp_bht_status);

        line_bp_bht_status = new QFrame(group_bp_bht);
        line_bp_bht_status->setObjectName(QString::fromUtf8("line_bp_bht_status"));
        line_bp_bht_status->setFrameShape(QFrame::VLine);
        line_bp_bht_status->setFrameShadow(QFrame::Sunken);

        layout_bp_bht_status->addWidget(line_bp_bht_status);

        text_bp_bht_bits = new QLabel(group_bp_bht);
        text_bp_bht_bits->setObjectName(QString::fromUtf8("text_bp_bht_bits"));
        text_bp_bht_bits->setMinimumSize(QSize(140, 0));

        layout_bp_bht_status->addWidget(text_bp_bht_bits);

        text_bp_bht_bits_number = new QLabel(group_bp_bht);
        text_bp_bht_bits_number->setObjectName(QString::fromUtf8("text_bp_bht_bits_number"));
        text_bp_bht_bits_number->setMinimumSize(QSize(40, 0));
        text_bp_bht_bits_number->setAlignment(Qt::AlignCenter);

        layout_bp_bht_status->addWidget(text_bp_bht_bits_number);


        gridLayout_2->addLayout(layout_bp_bht_status, 3, 0, 1, 3);


        verticalLayout_9->addWidget(group_bp_bht);


        verticalLayout_10->addWidget(group_bp);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_10->addItem(verticalSpacer_5);

        config_pages->addWidget(tab_branch_predictor);
        tab_virtual_memory = new QWidget();
        tab_virtual_memory->setObjectName(QString::fromUtf8("tab_virtual_memory"));
        group_vm = new QGroupBox(tab_virtual_memory);
        group_vm->setObjectName(QString::fromUtf8("group_vm"));
        group_vm->setEnabled(true);
        group_vm->setGeometry(QRect(10, 10, 471, 181));
        group_vm->setCheckable(true);
        group_vm->setChecked(true);
        radioButton_bare = new QRadioButton(group_vm);
        radioButton_bare->setObjectName(QString::fromUtf8("radioButton_bare"));
        radioButton_bare->setGeometry(QRect(10, 50, 99, 20));
        radioButton_sv32 = new QRadioButton(group_vm);
        radioButton_sv32->setObjectName(QString::fromUtf8("radioButton_sv32"));
        radioButton_sv32->setGeometry(QRect(10, 70, 99, 20));
        radioButton_sv32->setChecked(true);
        label_mode = new QLabel(group_vm);
        label_mode->setObjectName(QString::fromUtf8("label_mode"));
        label_mode->setGeometry(QRect(10, 30, 57, 14));
        lineEdit_kernel_base = new QLineEdit(group_vm);
        lineEdit_kernel_base->setObjectName(QString::fromUtf8("lineEdit_kernel_base"));
        lineEdit_kernel_base->setGeometry(QRect(360, 100, 91, 22));
        horizontalSlider_kernel_base = new BigSlider(group_vm);
        horizontalSlider_kernel_base->setObjectName(QString::fromUtf8("horizontalSlider_kernel_base"));
        horizontalSlider_kernel_base->setGeometry(QRect(200, 100, 131, 21));
        horizontalSlider_kernel_base->setOrientation(Qt::Horizontal);
        horizontalSlider_kernel_base->setTickPosition(QSlider::TicksBothSides);
        label_kernel_base = new QLabel(group_vm);
        label_kernel_base->setObjectName(QString::fromUtf8("label_kernel_base"));
        label_kernel_base->setGeometry(QRect(10, 100, 171, 20));
        lineEdit_root_ppn = new QLineEdit(group_vm);
        lineEdit_root_ppn->setObjectName(QString::fromUtf8("lineEdit_root_ppn"));
        lineEdit_root_ppn->setGeometry(QRect(360, 140, 91, 22));
        label_root_ppn = new QLabel(group_vm);
        label_root_ppn->setObjectName(QString::fromUtf8("label_root_ppn"));
        label_root_ppn->setGeometry(QRect(10, 140, 171, 20));
        horizontalSlider_root_ppn = new BigSlider(group_vm);
        horizontalSlider_root_ppn->setObjectName(QString::fromUtf8("horizontalSlider_root_ppn"));
        horizontalSlider_root_ppn->setGeometry(QRect(200, 140, 131, 21));
        horizontalSlider_root_ppn->setOrientation(Qt::Horizontal);
        horizontalSlider_root_ppn->setTickPosition(QSlider::TicksBothSides);
        config_pages->addWidget(tab_virtual_memory);
        tab_memory = new QWidget();
        tab_memory->setObjectName(QString::fromUtf8("tab_memory"));
        verticalLayout_4 = new QVBoxLayout(tab_memory);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        mem_protec_write = new QCheckBox(tab_memory);
        mem_protec_write->setObjectName(QString::fromUtf8("mem_protec_write"));

        verticalLayout_4->addWidget(mem_protec_write);

        mem_protec_exec = new QCheckBox(tab_memory);
        mem_protec_exec->setObjectName(QString::fromUtf8("mem_protec_exec"));

        verticalLayout_4->addWidget(mem_protec_exec);

        groupBox = new QGroupBox(tab_memory);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_read = new QLabel(groupBox);
        label_read->setObjectName(QString::fromUtf8("label_read"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_read);

        mem_time_read = new QSpinBox(groupBox);
        mem_time_read->setObjectName(QString::fromUtf8("mem_time_read"));
        mem_time_read->setMinimum(1);
        mem_time_read->setMaximum(999999999);

        formLayout->setWidget(0, QFormLayout::FieldRole, mem_time_read);

        label_write = new QLabel(groupBox);
        label_write->setObjectName(QString::fromUtf8("label_write"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_write);

        mem_time_write = new QSpinBox(groupBox);
        mem_time_write->setObjectName(QString::fromUtf8("mem_time_write"));
        mem_time_write->setMinimum(1);
        mem_time_write->setMaximum(999999999);

        formLayout->setWidget(1, QFormLayout::FieldRole, mem_time_write);

        label_burts_enable = new QLabel(groupBox);
        label_burts_enable->setObjectName(QString::fromUtf8("label_burts_enable"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_burts_enable);

        mem_enable_burst = new QCheckBox(groupBox);
        mem_enable_burst->setObjectName(QString::fromUtf8("mem_enable_burst"));
        mem_enable_burst->setChecked(false);

        formLayout->setWidget(2, QFormLayout::FieldRole, mem_enable_burst);

        label_burts = new QLabel(groupBox);
        label_burts->setObjectName(QString::fromUtf8("label_burts"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_burts);

        mem_time_burst = new QSpinBox(groupBox);
        mem_time_burst->setObjectName(QString::fromUtf8("mem_time_burst"));
        mem_time_burst->setMinimum(0);
        mem_time_burst->setMaximum(999999999);

        formLayout->setWidget(3, QFormLayout::FieldRole, mem_time_burst);

        label_level2 = new QLabel(groupBox);
        label_level2->setObjectName(QString::fromUtf8("label_level2"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_level2);

        mem_time_level2 = new QSpinBox(groupBox);
        mem_time_level2->setObjectName(QString::fromUtf8("mem_time_level2"));
        mem_time_level2->setMinimum(0);
        mem_time_level2->setMaximum(999999999);

        formLayout->setWidget(4, QFormLayout::FieldRole, mem_time_level2);


        verticalLayout_4->addWidget(groupBox);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_2);

        config_pages->addWidget(tab_memory);
        tab_cache_program = new QWidget();
        tab_cache_program->setObjectName(QString::fromUtf8("tab_cache_program"));
        config_pages->addWidget(tab_cache_program);
        tab_cache_data = new QWidget();
        tab_cache_data->setObjectName(QString::fromUtf8("tab_cache_data"));
        config_pages->addWidget(tab_cache_data);
        tab_cache_level2 = new QWidget();
        tab_cache_level2->setObjectName(QString::fromUtf8("tab_cache_level2"));
        config_pages->addWidget(tab_cache_level2);
        tab_os_emulation = new QWidget();
        tab_os_emulation->setObjectName(QString::fromUtf8("tab_os_emulation"));
        verticalLayout_7 = new QVBoxLayout(tab_os_emulation);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        osemu_enable = new QCheckBox(tab_os_emulation);
        osemu_enable->setObjectName(QString::fromUtf8("osemu_enable"));
        osemu_enable->setChecked(true);

        verticalLayout_7->addWidget(osemu_enable);

        osemu_known_syscall_stop = new QCheckBox(tab_os_emulation);
        osemu_known_syscall_stop->setObjectName(QString::fromUtf8("osemu_known_syscall_stop"));
        osemu_known_syscall_stop->setChecked(true);

        verticalLayout_7->addWidget(osemu_known_syscall_stop);

        osemu_unknown_syscall_stop = new QCheckBox(tab_os_emulation);
        osemu_unknown_syscall_stop->setObjectName(QString::fromUtf8("osemu_unknown_syscall_stop"));
        osemu_unknown_syscall_stop->setChecked(true);

        verticalLayout_7->addWidget(osemu_unknown_syscall_stop);

        osemu_interrupt_stop = new QCheckBox(tab_os_emulation);
        osemu_interrupt_stop->setObjectName(QString::fromUtf8("osemu_interrupt_stop"));
        osemu_interrupt_stop->setChecked(true);

        verticalLayout_7->addWidget(osemu_interrupt_stop);

        osemu_exception_stop = new QCheckBox(tab_os_emulation);
        osemu_exception_stop->setObjectName(QString::fromUtf8("osemu_exception_stop"));
        osemu_exception_stop->setChecked(true);

        verticalLayout_7->addWidget(osemu_exception_stop);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_fs_root = new QLabel(tab_os_emulation);
        label_fs_root->setObjectName(QString::fromUtf8("label_fs_root"));

        horizontalLayout->addWidget(label_fs_root);

        osemu_fs_root = new QLineEdit(tab_os_emulation);
        osemu_fs_root->setObjectName(QString::fromUtf8("osemu_fs_root"));

        horizontalLayout->addWidget(osemu_fs_root);

        osemu_fs_root_browse = new QPushButton(tab_os_emulation);
        osemu_fs_root_browse->setObjectName(QString::fromUtf8("osemu_fs_root_browse"));

        horizontalLayout->addWidget(osemu_fs_root_browse);


        verticalLayout_7->addLayout(horizontalLayout);

        verticalSpacer_4 = new QSpacerItem(21, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_4);

        config_pages->addWidget(tab_os_emulation);

        verticalLayout->addWidget(config_pages);


        horizontalLayout_4->addLayout(verticalLayout);


        verticalLayout_1->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(9, 0, 9, 0);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton_example = new QPushButton(NewDialog);
        pushButton_example->setObjectName(QString::fromUtf8("pushButton_example"));

        horizontalLayout_2->addWidget(pushButton_example);

        pushButton_start_empty = new QPushButton(NewDialog);
        pushButton_start_empty->setObjectName(QString::fromUtf8("pushButton_start_empty"));

        horizontalLayout_2->addWidget(pushButton_start_empty);

        pushButton_load = new QPushButton(NewDialog);
        pushButton_load->setObjectName(QString::fromUtf8("pushButton_load"));

        horizontalLayout_2->addWidget(pushButton_load);

        pushButton_cancel = new QPushButton(NewDialog);
        pushButton_cancel->setObjectName(QString::fromUtf8("pushButton_cancel"));

        horizontalLayout_2->addWidget(pushButton_cancel);


        verticalLayout_1->addLayout(horizontalLayout_2);


        retranslateUi(NewDialog);

        config_pages->setCurrentIndex(3);
        pushButton_load->setDefault(true);


        QMetaObject::connectSlotsByName(NewDialog);
    } // setupUi

    void retranslateUi(QDialog *NewDialog)
    {
        NewDialog->setWindowTitle(QCoreApplication::translate("NewDialog", "Dialog", nullptr));
        config_page_title->setText(QCoreApplication::translate("NewDialog", "Page Name", nullptr));
#if QT_CONFIG(accessibility)
        tab_preset_load->setAccessibleName(QCoreApplication::translate("NewDialog", "Presets and ELF File", nullptr));
#endif // QT_CONFIG(accessibility)
        preset_box->setTitle(QCoreApplication::translate("NewDialog", "Preset", nullptr));
        preset_no_pipeline->setText(QCoreApplication::translate("NewDialog", "No pipeline no cache", nullptr));
        preset_no_pipeline_cache->setText(QCoreApplication::translate("NewDialog", "No pipeline with cache", nullptr));
        preset_pipelined_bare->setText(QCoreApplication::translate("NewDialog", "Pipelined without hazard unit and without cache", nullptr));
        preset_pipelined->setText(QCoreApplication::translate("NewDialog", "Pipelined with hazard unit and cache", nullptr));
        preset_custom->setText(QCoreApplication::translate("NewDialog", "Custom", nullptr));
        reset_at_compile->setText(QCoreApplication::translate("NewDialog", "Reset at compile time (reload after make)", nullptr));
        label->setText(QCoreApplication::translate("NewDialog", "Elf executable: ", nullptr));
        pushButton_browse->setText(QCoreApplication::translate("NewDialog", "Browse", nullptr));
#if QT_CONFIG(accessibility)
        tab_core->setAccessibleName(QCoreApplication::translate("NewDialog", "Core ISA and Hazards", nullptr));
#endif // QT_CONFIG(accessibility)
        pipelined->setText(QCoreApplication::translate("NewDialog", "Pipelined", nullptr));
        xlen_64bit->setText(QCoreApplication::translate("NewDialog", "XLEN 64-bit", nullptr));
        isa_atomic->setText(QCoreApplication::translate("NewDialog", "Atomic (A)", nullptr));
        delay_slot->setText(QCoreApplication::translate("NewDialog", "Delay slot", nullptr));
        isa_multiply->setText(QCoreApplication::translate("NewDialog", "Multiply (M)", nullptr));
        hazard_unit->setTitle(QCoreApplication::translate("NewDialog", "Hazard unit", nullptr));
        hazard_stall->setText(QCoreApplication::translate("NewDialog", "Stall when hazard is detected", nullptr));
        hazard_stall_forward->setText(QCoreApplication::translate("NewDialog", "Stall or forward when hazard is detected", nullptr));
#if QT_CONFIG(accessibility)
        tab_branch_predictor->setAccessibleName(QCoreApplication::translate("NewDialog", "Branch Predictor", nullptr));
#endif // QT_CONFIG(accessibility)
        group_bp->setTitle(QCoreApplication::translate("NewDialog", "Branch Predictor", nullptr));
        text_bp_type->setText(QCoreApplication::translate("NewDialog", "Predictor type:", nullptr));
        text_bp_init_state->setText(QCoreApplication::translate("NewDialog", "Initial state:", nullptr));
        group_bp_btb->setTitle(QCoreApplication::translate("NewDialog", "Branch Target Buffer (BTB) ", nullptr));
        text_bp_btb_addr_bits_number->setText(QCoreApplication::translate("NewDialog", "0", nullptr));
#if QT_CONFIG(tooltip)
        text_bp_btb_addr_bits->setToolTip(QCoreApplication::translate("NewDialog", "PC - Program Counter register", nullptr));
#endif // QT_CONFIG(tooltip)
        text_bp_btb_addr_bits->setText(QCoreApplication::translate("NewDialog", "Bits from PC address: ", nullptr));
        text_bp_btb_entries->setText(QCoreApplication::translate("NewDialog", "Number of entries:", nullptr));
        text_bp_btb_entries_number->setText(QCoreApplication::translate("NewDialog", "1", nullptr));
        text_bp_btb_bits->setText(QCoreApplication::translate("NewDialog", "Number of bits:", nullptr));
        text_bp_btb_bits_number->setText(QCoreApplication::translate("NewDialog", "0", nullptr));
        group_bp_bht->setTitle(QCoreApplication::translate("NewDialog", "Branch History Table (BHT)", nullptr));
        text_bp_bht_bhr_bits_number->setText(QCoreApplication::translate("NewDialog", "0", nullptr));
        text_bp_bht_addr_bits_number->setText(QCoreApplication::translate("NewDialog", "0", nullptr));
#if QT_CONFIG(tooltip)
        text_bp_bht_bhr_bits->setToolTip(QCoreApplication::translate("NewDialog", "BHR - Branch History Register", nullptr));
#endif // QT_CONFIG(tooltip)
        text_bp_bht_bhr_bits->setText(QCoreApplication::translate("NewDialog", "Bits in BHR:", nullptr));
#if QT_CONFIG(tooltip)
        text_bp_bht_addr_bits->setToolTip(QCoreApplication::translate("NewDialog", "PC - Program Counter register", nullptr));
#endif // QT_CONFIG(tooltip)
        text_bp_bht_addr_bits->setText(QCoreApplication::translate("NewDialog", "Bits from PC address: ", nullptr));
        text_bp_bht_entries->setText(QCoreApplication::translate("NewDialog", "Number of entries:", nullptr));
        text_bp_bht_entries_number->setText(QCoreApplication::translate("NewDialog", "1", nullptr));
        text_bp_bht_bits->setText(QCoreApplication::translate("NewDialog", "Number of bits:", nullptr));
        text_bp_bht_bits_number->setText(QCoreApplication::translate("NewDialog", "0", nullptr));
#if QT_CONFIG(accessibility)
        tab_virtual_memory->setAccessibleName(QCoreApplication::translate("NewDialog", "Virtual Memory", nullptr));
#endif // QT_CONFIG(accessibility)
        group_vm->setTitle(QCoreApplication::translate("NewDialog", "Virtual Memory", nullptr));
        radioButton_bare->setText(QCoreApplication::translate("NewDialog", "Bare", nullptr));
        radioButton_sv32->setText(QCoreApplication::translate("NewDialog", "Sv32", nullptr));
        label_mode->setText(QCoreApplication::translate("NewDialog", "MODE", nullptr));
        lineEdit_kernel_base->setText(QCoreApplication::translate("NewDialog", "0x80000000", nullptr));
        label_kernel_base->setText(QCoreApplication::translate("NewDialog", "Kernel Virtual Base Address", nullptr));
        lineEdit_root_ppn->setText(QCoreApplication::translate("NewDialog", "0x80000000", nullptr));
        label_root_ppn->setText(QCoreApplication::translate("NewDialog", "Page\342\200\221Table Base (PPN)", nullptr));
#if QT_CONFIG(accessibility)
        tab_memory->setAccessibleName(QCoreApplication::translate("NewDialog", "Memory Timings", nullptr));
#endif // QT_CONFIG(accessibility)
        mem_protec_write->setText(QCoreApplication::translate("NewDialog", "Program memory write protection", nullptr));
        mem_protec_exec->setText(QCoreApplication::translate("NewDialog", "Data memory executable protection", nullptr));
        groupBox->setTitle(QCoreApplication::translate("NewDialog", "Access time (in cycles)", nullptr));
        label_read->setText(QCoreApplication::translate("NewDialog", "Read:", nullptr));
        label_write->setText(QCoreApplication::translate("NewDialog", "Write:", nullptr));
        label_burts_enable->setText(QCoreApplication::translate("NewDialog", "Burst enable:", nullptr));
        label_burts->setText(QCoreApplication::translate("NewDialog", "Burst:", nullptr));
        label_level2->setText(QCoreApplication::translate("NewDialog", "L2 Access:", nullptr));
#if QT_CONFIG(accessibility)
        tab_cache_program->setAccessibleName(QCoreApplication::translate("NewDialog", "L1 Program Cache", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        tab_cache_data->setAccessibleName(QCoreApplication::translate("NewDialog", "L1 Data Cache", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        tab_cache_level2->setAccessibleName(QCoreApplication::translate("NewDialog", "L2 Cache", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        tab_os_emulation->setAccessibleName(QCoreApplication::translate("NewDialog", "System Emulation and IRQ", nullptr));
#endif // QT_CONFIG(accessibility)
        osemu_enable->setText(QCoreApplication::translate("NewDialog", "Enable emulation of operating system services", nullptr));
        osemu_known_syscall_stop->setText(QCoreApplication::translate("NewDialog", "Stop on known system call", nullptr));
        osemu_unknown_syscall_stop->setText(QCoreApplication::translate("NewDialog", "Stop on unknown system call", nullptr));
        osemu_interrupt_stop->setText(QCoreApplication::translate("NewDialog", "Stop on interrupt entry", nullptr));
        osemu_exception_stop->setText(QCoreApplication::translate("NewDialog", "Stop and step over exceptions (overflow, etc.)", nullptr));
        label_fs_root->setText(QCoreApplication::translate("NewDialog", "Filesystem root:", nullptr));
        osemu_fs_root_browse->setText(QCoreApplication::translate("NewDialog", "Browse", nullptr));
        pushButton_example->setText(QCoreApplication::translate("NewDialog", "Example", nullptr));
        pushButton_start_empty->setText(QCoreApplication::translate("NewDialog", "Start empty", nullptr));
        pushButton_load->setText(QCoreApplication::translate("NewDialog", "Load machine", nullptr));
        pushButton_cancel->setText(QCoreApplication::translate("NewDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewDialog: public Ui_NewDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWDIALOG_H
