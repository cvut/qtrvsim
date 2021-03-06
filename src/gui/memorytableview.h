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

#ifndef MEMORYTABLEVIEW_H
#define MEMORYTABLEVIEW_H

#include "machine/memory/address.h"

#include <QObject>
#include <QSettings>
#include <QSharedPointer>
#include <QTableView>

class MemoryTableView : public QTableView {
    Q_OBJECT

    using Super = QTableView;

public:
    MemoryTableView(QWidget *parent, QSettings *settings);

    void resizeEvent(QResizeEvent *event) override;
signals:
    void address_changed(machine::Address address);
    void adjust_scroll_pos_queue();
public slots:
    void set_cell_size(int index);
    void go_to_address(machine::Address address);
    void focus_address(machine::Address address);
    void recompute_columns();

protected:
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void adjust_scroll_pos_check();
    void adjust_scroll_pos_process();

private:
    void addr0_save_change(machine::Address val);
    void adjustColumnCount();
    QSettings *settings;

    machine::Address initial_address;
    bool adjust_scroll_pos_in_progress;
};

#endif // MEMORYTABLEVIEW_H
