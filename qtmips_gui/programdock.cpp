// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#include "programdock.h"

#include "hexlineedit.h"
#include "programmodel.h"
#include "programtableview.h"

#include <QComboBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

ProgramDock::ProgramDock(QWidget *parent, QSettings *settings) : Super(parent) {
    setObjectName("Program");
    setWindowTitle("Program");

    this->settings = settings;
    follow_source = (enum FollowSource)settings
                        ->value("ProgramViewFollowSource", FOLLOWSRC_FETCH)
                        .toInt();

    for (unsigned int &i : follow_addr) {
        i = 0;
    }

    QWidget *content = new QWidget();

    QComboBox *follow_inst = new QComboBox();
    follow_inst->addItem("Follow none");
    follow_inst->addItem("Follow fetch");
    follow_inst->addItem("Follow decode");
    follow_inst->addItem("Follow execute");
    follow_inst->addItem("Follow memory");
    follow_inst->addItem("Follow writeback");
    follow_inst->setCurrentIndex((int)follow_source);

    ProgramTableView *program_content = new ProgramTableView(nullptr, settings);
    // program_content->setSizePolicy();
    ProgramModel *program_model = new ProgramModel(this);
    program_content->setModel(program_model);
    program_content->verticalHeader()->hide();
    // program_content->setHorizontalHeader(program_model->);

    HexLineEdit *go_edit = new HexLineEdit(nullptr, 8, 16, "0x");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(follow_inst);
    layout->addWidget(program_content);
    layout->addWidget(go_edit);

    content->setLayout(layout);

    setWidget(content);

    connect(
        this, &ProgramDock::machine_setup, program_model, &ProgramModel::setup);
    connect(
        go_edit, &HexLineEdit::value_edit_finished, program_content,
        [program_content](uint32_t value) {
            program_content->go_to_address(value);
        });
    connect(
        program_content, &ProgramTableView::address_changed, go_edit,
        &HexLineEdit::set_value);
    connect(
        this, &ProgramDock::jump_to_pc, program_content,
        &ProgramTableView::go_to_address);
    connect(
        follow_inst, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &ProgramDock::set_follow_inst);
    connect(
        this, &ProgramDock::focus_addr, program_content,
        &ProgramTableView::focus_address);
    connect(
        this, &ProgramDock::focus_addr_with_save, program_content,
        &ProgramTableView::focus_address_with_save);
    connect(
        program_content, &QAbstractItemView::doubleClicked, program_model,
        &ProgramModel::toggle_hw_break);
    connect(
        this, &ProgramDock::stage_addr_changed, program_model,
        &ProgramModel::update_stage_addr);
    connect(
        program_model, &ProgramModel::report_error, this,
        &ProgramDock::report_error);
    connect(
        this, &ProgramDock::request_update_all, program_model,
        &ProgramModel::update_all);
}

void ProgramDock::setup(machine::QtMipsMachine *machine) {
    uint32_t pc;
    emit machine_setup(machine);
    if (machine == nullptr) {
        return;
    }
    pc = machine->registers()->read_pc();
    for (unsigned int &i : follow_addr) {
        i = pc;
    }
    update_follow_position();
}

void ProgramDock::set_follow_inst(int follow) {
    follow_source = (enum FollowSource)follow;
    settings->setValue("ProgramViewFollowSource", (int)follow_source);
    update_follow_position();
}

void ProgramDock::fetch_inst_addr(uint32_t addr) {
    if (addr != machine::STAGEADDR_NONE) {
        follow_addr[FOLLOWSRC_FETCH] = addr;
    }
    emit stage_addr_changed(ProgramModel::STAGEADDR_FETCH, addr);
    if (follow_source == FOLLOWSRC_FETCH) {
        update_follow_position();
    }
}

void ProgramDock::decode_inst_addr(uint32_t addr) {
    if (addr != machine::STAGEADDR_NONE) {
        follow_addr[FOLLOWSRC_DECODE] = addr;
    }
    emit stage_addr_changed(ProgramModel::STAGEADDR_DECODE, addr);
    if (follow_source == FOLLOWSRC_DECODE) {
        update_follow_position();
    }
}

void ProgramDock::execute_inst_addr(uint32_t addr) {
    if (addr != machine::STAGEADDR_NONE) {
        follow_addr[FOLLOWSRC_EXECUTE] = addr;
    }
    emit stage_addr_changed(ProgramModel::STAGEADDR_EXECUTE, addr);
    if (follow_source == FOLLOWSRC_EXECUTE) {
        update_follow_position();
    }
}

void ProgramDock::memory_inst_addr(uint32_t addr) {
    if (addr != machine::STAGEADDR_NONE) {
        follow_addr[FOLLOWSRC_MEMORY] = addr;
    }
    emit stage_addr_changed(ProgramModel::STAGEADDR_MEMORY, addr);
    if (follow_source == FOLLOWSRC_MEMORY) {
        update_follow_position();
    }
}

void ProgramDock::writeback_inst_addr(uint32_t addr) {
    if (addr != machine::STAGEADDR_NONE) {
        follow_addr[FOLLOWSRC_WRITEBACK] = addr;
    }
    emit stage_addr_changed(ProgramModel::STAGEADDR_WRITEBACK, addr);
    if (follow_source == FOLLOWSRC_WRITEBACK) {
        update_follow_position();
    }
}

void ProgramDock::update_follow_position() {
    if (follow_source != FOLLOWSRC_NONE) {
        emit focus_addr(follow_addr[follow_source]);
    }
}

void ProgramDock::report_error(const QString &error) {
    QMessageBox::critical(this, "QtMips Error", error);
}
