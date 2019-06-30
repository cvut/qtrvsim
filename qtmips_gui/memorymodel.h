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

#ifndef MEMORYMODEL_H
#define MEMORYMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include "qtmipsmachine.h"

class MemoryModel : public QAbstractTableModel
{
    Q_OBJECT

    using Super = QAbstractTableModel;
public:
    enum MemoryCellSize {
        CELLSIZE_BYTE,
        CELLSIZE_HWORD,
        CELLSIZE_WORD,
    };

    MemoryModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role) override;
    bool adjustRowAndOffset(int &row, std::uint32_t address);
    void update_all();

    void setCellsPerRow(unsigned int cells);

    inline unsigned int cellsPerRow() {
        return cells_per_row;
    }

    inline const QFont *getFont() const {
        return &data_font;
    }

    inline std::uint32_t getIndex0Offset() const {
        return index0_offset;
    }

    inline unsigned int cellSizeBytes() const {
        switch (cell_size) {
        case CELLSIZE_BYTE:
            return 1;
        case CELLSIZE_HWORD:
            return 2;
        case CELLSIZE_WORD:
            return 4;
        }
        return 0;
    }
    inline bool get_row_address(std::uint32_t &address, int row) const {
        address = index0_offset + (row * cells_per_row * cellSizeBytes());
        return address >= index0_offset;
    }
    inline bool get_row_for_address(int &row, std::uint32_t address) const {
        if (address < index0_offset) {
            row = -1;
            return false;
        }
        row = (address - index0_offset) / (cells_per_row * cellSizeBytes());
        if ((address - index0_offset > 0x80000000) || row > rowCount()) {
            row = rowCount();
            return false;
        }
        return true;
    }

public slots:
    void setup(machine::QtMipsMachine *machine);
    void set_cell_size(int index);
    void check_for_updates();
    void cached_access(int cached);

signals:
    void cell_size_changed();
    void setup_done();

private:
    const machine::MemoryAccess *mem_access() const;
    machine::MemoryAccess *mem_access_rw() const;
    enum MemoryCellSize cell_size;
    unsigned int cells_per_row;
    std::uint32_t index0_offset;
    QFont data_font;
    machine::QtMipsMachine *machine;
    std::uint32_t memory_change_counter;
    std::uint32_t cache_data_change_counter;
    int access_through_cache;
};


#endif // MEMORYMODEL_H
