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

#include "memorytableview.h"

#include "hinttabledelegate.h"
#include "memorymodel.h"

#include <QApplication>
#include <QClipboard>
#include <QFontMetrics>
#include <QHeaderView>
#include <QKeyEvent>
#include <QScrollBar>

MemoryTableView::MemoryTableView(QWidget *parent, QSettings *settings)
    : Super(parent) {
    setItemDelegate(new HintTableDelegate);
    connect(
        verticalScrollBar(), &QAbstractSlider::valueChanged, this,
        &MemoryTableView::adjust_scroll_pos_check);
    connect(
        this, &MemoryTableView::adjust_scroll_pos_queue, this,
        &MemoryTableView::adjust_scroll_pos_process, Qt::QueuedConnection);
    this->settings = settings;
    initial_address
        = machine::Address(settings->value("DataViewAddr0", 0).toULongLong());
    adjust_scroll_pos_in_progress = false;
    setTextElideMode(Qt::ElideNone);
}

void MemoryTableView::addr0_save_change(machine::Address val) {
    settings->setValue("DataViewAddr0", qint64(val.get_raw()));
}

void MemoryTableView::adjustColumnCount() {
    MemoryModel *m = dynamic_cast<MemoryModel *>(model());
    if (m == nullptr) {
        return;
    }

    HintTableDelegate *delegate
        = dynamic_cast<HintTableDelegate *>(itemDelegate());
    if (delegate == nullptr) {
        return;
    }

    if (horizontalHeader()->count() >= 2) {
        QModelIndex idx;
        QFontMetrics fm(*m->getFont());
        idx = m->index(0, 0);
        // int width0_dh = itemDelegate(idx)->sizeHint(viewOptions(),
        // idx).get_width() + 2;
        int width0_dh
            = delegate->sizeHintForText(viewOptions(), idx, "0x00000000").width()
              + 2;
        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(0, width0_dh);

        idx = m->index(0, 1);
        QString t = "";
        t.fill(QChar('0'), m->cellSizeBytes() * 2);
        int width1_dh
            = delegate->sizeHintForText(viewOptions(), idx, t).width() + 2;
        if (width1_dh < fm.horizontalAdvance("+99")) {
            width1_dh = fm.horizontalAdvance("+99");
        }
        horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(1, width1_dh);

        int w = verticalHeader()->width() + 4;
        unsigned int cells;
        int width0 = columnWidth(0);
        int width1 = columnWidth(1);
        w = width() - w - width0;
        if (w < width1 + 4) {
            cells = 1;
        } else {
            cells = w / (width1 + 4);
        }
        if (cells != m->cellsPerRow()) {
            m->setCellsPerRow(cells);
        }
        for (unsigned int i = 1; i < m->cellsPerRow() + 1; i++) {
            horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
            horizontalHeader()->resizeSection(i, width1);
        }
        if (!initial_address.is_null()) {
            go_to_address(initial_address);
            initial_address = machine::Address::null();
        }
    }
}

void MemoryTableView::recompute_columns() {
    adjustColumnCount();
}

void MemoryTableView::set_cell_size(int index) {
    machine::Address address;
    int row;
    bool keep_row0 = false;
    MemoryModel *m = dynamic_cast<MemoryModel *>(model());
    if (m != nullptr) {
        keep_row0 = m->get_row_address(address, rowAt(0));
        m->set_cell_size(index);
    }
    adjustColumnCount();
    if (keep_row0) {
        m->adjustRowAndOffset(row, address);
        scrollTo(m->index(row, 0), QAbstractItemView::PositionAtTop);
    }
}

void MemoryTableView::adjust_scroll_pos_check() {
    if (!adjust_scroll_pos_in_progress) {
        adjust_scroll_pos_in_progress = true;
        emit adjust_scroll_pos_queue();
    }
}

void MemoryTableView::adjust_scroll_pos_process() {
    adjust_scroll_pos_in_progress = false;
    machine::Address address;
    MemoryModel *m = dynamic_cast<MemoryModel *>(model());
    if (m == nullptr) {
        return;
    }

    QModelIndex prev_index = currentIndex();
    machine::Address row_bytes
        = machine::Address(m->cellSizeBytes() * m->cellsPerRow());
    machine::Address index0_offset = m->getIndex0Offset();

    do {
        int row = rowAt(0);
        int prev_row = row;
        if (row < m->rowCount() / 8) {
            if ((row == 0) && (index0_offset < row_bytes)
                && (!index0_offset.is_null())) {
                m->adjustRowAndOffset(row, machine::Address::null());
            } else if (index0_offset > row_bytes) {
                m->get_row_address(address, row);
                m->adjustRowAndOffset(row, address);
            } else {
                break;
            }
        } else if (row > m->rowCount() - m->rowCount() / 8) {
            m->get_row_address(address, row);
            m->adjustRowAndOffset(row, address);
        } else {
            break;
        }
        scrollTo(m->index(row, 0), QAbstractItemView::PositionAtTop);
        setCurrentIndex(
            m->index(prev_index.row() + row - prev_row, prev_index.column()));
        emit m->update_all();
    } while (false);
    m->get_row_address(address, rowAt(0));
    addr0_save_change(address);
    emit address_changed(address);
}

void MemoryTableView::resizeEvent(QResizeEvent *event) {
    MemoryModel *m = dynamic_cast<MemoryModel *>(model());
    machine::Address address;
    bool keep_row0 = false;

    if (m != nullptr) {
        if (initial_address.is_null()) {
            keep_row0 = m->get_row_address(address, rowAt(0));
        } else {
            address = initial_address;
        }
    }
    Super::resizeEvent(event);
    adjustColumnCount();
    if (keep_row0) {
        initial_address = machine::Address::null();
        go_to_address(address);
    }
}

void MemoryTableView::go_to_address(machine::Address address) {
    MemoryModel *m = dynamic_cast<MemoryModel *>(model());
    int row;
    if (m == nullptr) {
        return;
    }
    m->adjustRowAndOffset(row, address);
    scrollTo(m->index(row, 0), QAbstractItemView::PositionAtTop);
    setCurrentIndex(m->index(row, 1));
    addr0_save_change(address);
    emit m->update_all();
}

void MemoryTableView::focus_address(machine::Address address) {
    int row;
    MemoryModel *m = dynamic_cast<MemoryModel *>(model());
    if (m == nullptr) {
        return;
    }
    if (!m->get_row_for_address(row, address)) {
        go_to_address(address);
    }
    if (!m->get_row_for_address(row, address)) {
        return;
    }
    setCurrentIndex(m->index(row, 1));
}

void MemoryTableView::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Copy)) {
        QString text;
        QItemSelectionRange range = selectionModel()->selection().first();
        for (auto i = range.top(); i <= range.bottom(); ++i) {
            QStringList rowContents;
            for (auto j = range.left(); j <= range.right(); ++j) {
                rowContents << model()->index(i, j).data().toString();
            }
            text += rowContents.join("\t");
            text += "\n";
        }
        QApplication::clipboard()->setText(text);
    } else {
        Super::keyPressEvent(event);
    }
}
