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

#include <QBrush>

#include "programmodel.h"

ProgramModel::ProgramModel(QObject *parent)
    : Super(parent), data_font("Monospace") {
    index0_offset = 0;
    data_font.setStyleHint(QFont::TypeWriter);
    machine = nullptr;
    memory_change_counter = 0;
    cache_program_change_counter = 0;
}

int ProgramModel::rowCount(const QModelIndex & /*parent*/) const {
   return 750;
}

int ProgramModel::columnCount(const QModelIndex & /*parent*/) const {
    return 4;
}

QVariant ProgramModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {
        if(role == Qt::DisplayRole) {
            switch (section) {
            case 0:
                return tr("Bp");
            case 1:
                return tr("Address");
            case 2:
                return tr("Code");
            case 3:
                return tr("Instruction");
            default:
                return tr("");
            }
        }
    }
    return Super::headerData(section, orientation, role);
}

QVariant ProgramModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole)
    {
        QString s, t;
        std::uint32_t address;
        if (!get_row_address(address, index.row()))
            return QString("");

        if (index.column() == 1) {
            t = QString::number(address, 16);
            s.fill('0', 8 - t.count());
            return "0x" + s + t.toUpper();
        }

        if (machine == nullptr)
            return QString(" ");
        if (machine->memory() == nullptr)
            return QString(" ");

        machine::Instruction inst(machine->memory()->read_word(address));

        switch (index.column()) {
        case 0:
            if (machine->is_hwbreak(address))
                return QString("B");
            else
                return QString(" ");
        case 2:
            t = QString::number(inst.data(), 16);
            s.fill('0', 8 - t.count());
            return s + t.toUpper();
        case 3: return inst.to_str(address);
        default:  return tr("");
        }
    }
    if (role == Qt::BackgroundRole) {
        std::uint32_t address;
        if (!get_row_address(address, index.row()) ||
            machine == nullptr)
            return QVariant();
        if (index.column() == 2 && machine->cache_program() != nullptr) {
            machine::LocationStatus loc_stat;
            loc_stat = machine->cache_program()->location_status(address);
            if (loc_stat & machine::LOCSTAT_CACHED) {
                QBrush bgd(Qt::lightGray);
                return bgd;
            }
        } else if (index.column() == 0 && machine->is_hwbreak(address)) {
            QBrush bgd(Qt::red);
            return bgd;
        }
        return QVariant();
    }
    if (role==Qt::FontRole)
            return data_font;
    return QVariant();
}

void ProgramModel::setup(machine::QtMipsMachine *machine) {
    this->machine = machine;
    if (machine != nullptr)
        connect(machine, SIGNAL(post_tick()), this, SLOT(check_for_updates()));
    emit update_all();
}

void ProgramModel::update_all() {
    if (machine != nullptr && machine->memory() != nullptr) {
        memory_change_counter = machine->memory()->get_change_counter();
        if (machine->cache_program() != nullptr)
            cache_program_change_counter = machine->cache_program()->get_change_counter();
    }
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void ProgramModel::check_for_updates() {
    bool need_update = false;
    if (machine == nullptr)
        return;
    if (machine->memory() == nullptr)
        return;

    if (memory_change_counter != machine->memory()->get_change_counter())
        need_update = true;
    if (machine->cache_data() != nullptr) {
        if (cache_program_change_counter != machine->cache_program()->get_change_counter())
            need_update = true;
    }
    if (!need_update)
        return;
    update_all();
}

bool ProgramModel::adjustRowAndOffset(int &row, int optimal_row, std::uint32_t address) {
    if (optimal_row < rowCount() / 8)
        optimal_row = rowCount() / 8;
    if (optimal_row >= rowCount() - rowCount() / 8)
        optimal_row = rowCount() - rowCount() / 8;
    row = rowCount() / 2;
    address -= address % cellSizeBytes();
    std::uint32_t row_bytes = cellSizeBytes();
    std::uint32_t diff = row * row_bytes;
    if (diff > address) {
        row = address / row_bytes;
        if (row == 0) {
            index0_offset = 0;
        } else {
           index0_offset = address - row * row_bytes;
        }
    } else {
        index0_offset = address - diff;
    }
    return get_row_for_address(row, address);
}

void ProgramModel::toggle_hw_break(const QModelIndex & index) {
    std::uint32_t address;
    if (index.column() != 0 || machine == nullptr)
        return;

    if (!get_row_address(address, index.row()))
        return;

    if (machine->is_hwbreak(address))
        machine->remove_hwbreak(address);
    else
        machine->insert_hwbreak(address);
    update_all();
}

Qt::ItemFlags ProgramModel::flags(const QModelIndex &index) const {
    if (index.column() != 2 && index.column() != 3)
        return QAbstractTableModel::flags(index);
    else
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool ProgramModel::setData(const QModelIndex & index, const QVariant & value, int role) {
    if (role == Qt::EditRole)
    {
        bool ok;
        std::uint32_t address;
        std::uint32_t data;
        machine::MemoryAccess *mem;
        if (!get_row_address(address, index.row()))
            return false;
        if (index.column() == 0 || machine == nullptr)
            return false;
        mem = machine->memory_rw();
        if (mem == nullptr)
            return false;
        switch (index.column()) {
        case 2:
            data = value.toString().toULong(&ok, 16);
            if (!ok)
                return false;
            mem->write_word(address, data);
            break;
        case 3:
            data = machine::Instruction::from_string(value.toString(), &ok, address).data();
            if (!ok)
                return false;
            mem->write_word(address, data);
            break;
        default:
            return false;
        }
    }
    return true;
}
