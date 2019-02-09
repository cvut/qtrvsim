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

#include "memorymodel.h"

MemoryModel::MemoryModel(QObject *parent)
    :QAbstractTableModel(parent) {
    cell_size = CELLSIZE_WORD;
    cells_per_row = 1;
    index0_offset = 0;
    updateHeaderLabels();
}

void MemoryModel::updateHeaderLabels() {
    setHeaderData(0, Qt::Horizontal, QString("Address"));
    for (unsigned int i = 0; i < cells_per_row; i++) {
        std::uint32_t addr = i * cellSizeBytes();
        setHeaderData(i + 1, Qt::Horizontal, QString("+0x") +
                      QString::number(addr, 16));
    }
}

int MemoryModel::rowCount(const QModelIndex & /*parent*/) const {
   std::uint64_t rows = (0x10000000 + cells_per_row - 1) / cells_per_row;
   return rows;
}

int MemoryModel::columnCount(const QModelIndex & /*parent*/) const {
    return 2;
}

QVariant MemoryModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole)
    {
       return QString("Row%1, Column%2")
                   .arg(index.row() + 1)
                   .arg(index.column() +1);
    }
    return QVariant();
}
