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
#include "memorydock.h"
#include "memorymodel.h"
#include "memorytableview.h"
#include "hexlineedit.h"

MemoryDock::MemoryDock(QWidget *parent, QSettings *settings) : Super(parent) {
    setObjectName("Memory");
    setWindowTitle("Memory");

    QWidget *content = new QWidget();

    QComboBox *cell_size = new QComboBox();
    cell_size->addItem("Byte", MemoryModel::CELLSIZE_BYTE);
    cell_size->addItem("Half-word", MemoryModel::CELLSIZE_HWORD);
    cell_size->addItem("Word", MemoryModel::CELLSIZE_WORD);
    cell_size->setCurrentIndex(MemoryModel::CELLSIZE_WORD);

    QComboBox *cached_access = new QComboBox();
    cached_access->addItem("Direct", 0);
    cached_access->addItem("Cached", 1);

    QTableView *memory_content = new MemoryTableView(0, settings);
    // memory_content->setSizePolicy();
    MemoryModel *memory_model = new MemoryModel(0);
    memory_content->setModel(memory_model);
    memory_content->verticalHeader()->hide();
    //memory_content->setHorizontalHeader(memory_model->);

    QLineEdit *go_edit = new HexLineEdit(0, 8, 16, "0x");

    QHBoxLayout *layout_top = new QHBoxLayout;
    layout_top->addWidget(cell_size);
    layout_top->addWidget(cached_access);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(layout_top);
    layout->addWidget(memory_content);
    layout->addWidget(go_edit);

    content->setLayout(layout);

    setWidget(content);

    connect(this, &MemoryDock::machine_setup, memory_model, &MemoryModel::setup);
    connect(cell_size, SIGNAL(currentIndexChanged(int)),
            memory_content, SLOT(set_cell_size(int)));
    connect(cached_access, SIGNAL(currentIndexChanged(int)),
            memory_model, SLOT(cached_access(int)));
    connect(go_edit, SIGNAL(value_edit_finished(std::uint32_t)),
            memory_content, SLOT(go_to_address(std::uint32_t)));
    connect(memory_content, SIGNAL(address_changed(std::uint32_t)),
            go_edit, SLOT(set_value(std::uint32_t)));
    connect(this, SIGNAL(focus_addr(std::uint32_t)),
            memory_content, SLOT(focus_address(std::uint32_t)));
}

void MemoryDock::setup(machine::QtMipsMachine *machine) {
    emit machine_setup(machine);
}
