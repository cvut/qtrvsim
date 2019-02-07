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

#include "memorydock.h"

DataView::DataView(QWidget *parent, QSettings *settings) : MemoryView(parent, settings->value("DataViewAddr0", 0).toULongLong()) {
    this->settings = settings;
}

QList<QWidget*> DataView::row_widget(std::uint32_t address, QWidget *parent) {
    QList<QWidget*> widgs;
    QLabel *l;

    QFont f;
    f.setStyleHint(QFont::Monospace);

    l = new QLabel(QString("0x") + QString("%1").arg(address, 8, 16, QChar('0')).toUpper(), parent);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    l->setFont(f);
    widgs.append(l);

    l = new QLabel(parent);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    l->setFont(f);
    if (memory != nullptr) {
        machine::LocationStatus loc_stat = machine::LOCSTAT_NONE;
        QString val;
        val = QString("0x") + QString("%1").arg(memory->read_word(address), 8, 16, QChar('0')).toUpper();
        if (cache_data != nullptr) {
            loc_stat = cache_data->location_status(address);
            if (loc_stat & machine::LOCSTAT_DIRTY)
                val += " D";
            else if (loc_stat & machine::LOCSTAT_CACHED)
                val += " C";
        }
        l->setText(val);
    }
    widgs.append(l);

    return widgs;
}

void DataView::addr0_save_change(std::uint32_t val) {
    settings->setValue("DataViewAddr0", val);
}

MemoryDock::MemoryDock(QWidget *parent, QSettings *settings) : QDockWidget(parent) {
    view = new DataView(this, settings);
    setWidget(view);

    setObjectName("Memory");
    setWindowTitle("Memory");
}

void MemoryDock::setup(machine::QtMipsMachine *machine) {
    view->setup(machine);
}
