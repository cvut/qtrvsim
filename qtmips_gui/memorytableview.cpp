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

#include <QHeaderView>
#include <QFontMetrics>
#include "memorytableview.h"
#include "memorymodel.h"

MemoryTableView::MemoryTableView(QWidget *parent) : Super(parent) {

}

void MemoryTableView::resizeEvent(QResizeEvent *event) {
    Super::resizeEvent(event);
    MemoryModel *m = dynamic_cast<MemoryModel*>(model());

    if (horizontalHeader()->count() >= 2 && m != nullptr) {
        QFontMetrics fm(*m->getFont());
        int width0 = fm.width("0x00000000");
        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(0, width0);

        QString t = "";
        t.fill(QChar('0'), m->cellSizeBytes() * 2);
        int width1 = fm.width(t + " C");
        horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(1, width1);

        int w = verticalHeader()->width() + 8;
        int cells;
        width0 = columnWidth(0);
        width1 = columnWidth(1);
        w = width() - w - width0;
        if (w < width1) {
            cells = 1;
        } else {
            cells = w / width1;
        }
        if (cells != m->cellsPerRow()) {
            m->setCellsPerRow(cells);
        }
        for (int i = 1; i < m->cellsPerRow() + 1; i++) {
            horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
            horizontalHeader()->resizeSection(i, width1);
        }
    }
}
