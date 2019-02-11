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

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTableView>
#include <QComboBox>
#include <QHeaderView>
#include "programdock.h"
#include "programmodel.h"
#include "programtableview.h"
#include "hexlineedit.h"



ProgramDock::ProgramDock(QWidget *parent, QSettings *settings) : Super(parent) {
    setObjectName("Program");
    setWindowTitle("Program");

    this->settings = settings;
    follow_source = (enum FollowSource)
            settings->value("ProgramViewFollowSource", 0).toInt();

    for (int i = 0; i < FOLLOWSRC_COUNT; i++)
        follow_addr[i] = 0;

    QWidget *content = new QWidget();

    QComboBox *follow_inst = new QComboBox();
    follow_inst->addItem("Follow none");
    follow_inst->addItem("Follow fetch");
    follow_inst->addItem("Follow decode");
    follow_inst->addItem("Follow execute");
    follow_inst->addItem("Follow memory");
    follow_inst->addItem("Follow writeback");
    follow_inst->setCurrentIndex((int)follow_source);

    QTableView *program_content = new ProgramTableView(0, settings);
    // program_content->setSizePolicy();
    ProgramModel *program_model = new ProgramModel(0);
    program_content->setModel(program_model);
    program_content->verticalHeader()->hide();
    //program_content->setHorizontalHeader(program_model->);

    QLineEdit *go_edit = new HexLineEdit(0, 8, 16, "0x");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(follow_inst);
    layout->addWidget(program_content);
    layout->addWidget(go_edit);

    content->setLayout(layout);

    setWidget(content);

    connect(this, &ProgramDock::machine_setup, program_model, &ProgramModel::setup);
    connect(go_edit, SIGNAL(value_edit_finished(std::uint32_t)),
            program_content, SLOT(go_to_address(std::uint32_t)));
    connect(program_content, SIGNAL(address_changed(std::uint32_t)),
            go_edit, SLOT(set_value(std::uint32_t)));
    connect(this, SIGNAL(jump_to_pc(std::uint32_t)),
            program_content, SLOT(go_to_address(std::uint32_t)));
    connect(follow_inst, SIGNAL(currentIndexChanged(int)),
            this, SLOT(set_follow_inst(int)));
    connect(this, SIGNAL(focus_addr(std::uint32_t)),
            program_content, SLOT(focus_address(std::uint32_t)));
}

void ProgramDock::setup(machine::QtMipsMachine *machine) {
    std::uint32_t pc;
    emit machine_setup(machine);
    if (machine == nullptr)
        return;
    pc = machine->registers()->read_pc();
    for (int i = 0; i < FOLLOWSRC_COUNT; i++)
        follow_addr[i] = pc;
    update_follow_position();
}

void ProgramDock::set_follow_inst(int follow) {
    follow_source = (enum FollowSource)follow;
    settings->setValue("ProgramViewFollowSource", (int)follow_source);
    update_follow_position();
}

void ProgramDock::fetch_inst_addr(std::uint32_t addr) {
    follow_addr[FOLLOWSRC_FETCH] = addr;
    if (follow_source == FOLLOWSRC_FETCH)
        update_follow_position();
}

void ProgramDock::decode_inst_addr(std::uint32_t addr) {
    follow_addr[FOLLOWSRC_DECODE] = addr;
    if (follow_source == FOLLOWSRC_DECODE)
        update_follow_position();
}

void ProgramDock::execute_inst_addr(std::uint32_t addr) {
    follow_addr[FOLLOWSRC_EXECUTE] = addr;
    if (follow_source == FOLLOWSRC_EXECUTE)
        update_follow_position();
}

void ProgramDock::memory_inst_addr(std::uint32_t addr) {
    follow_addr[FOLLOWSRC_MEMORY] = addr;
    if (follow_source == FOLLOWSRC_MEMORY)
        update_follow_position();
}

void ProgramDock::writeback_inst_addr(std::uint32_t addr) {
    follow_addr[FOLLOWSRC_WRITEBACK] = addr;
    if (follow_source == FOLLOWSRC_WRITEBACK)
        update_follow_position();
}

void ProgramDock::update_follow_position() {
    if (follow_source != FOLLOWSRC_NONE)
        emit focus_addr(follow_addr[follow_source]);
}
