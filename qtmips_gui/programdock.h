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

#ifndef PROGRAMDOCK_H
#define PROGRAMDOCK_H

#include <QDockWidget>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include "qtmipsmachine.h"
#include "memoryview.h"

class ProgramView : public MemoryView {
    Q_OBJECT
public:
    ProgramView(QWidget *parent, QSettings *settings);

    void setup(machine::QtMipsMachine*);

    void jump_to_pc(std::uint32_t);

protected:
    QList<QWidget*> row_widget(std::uint32_t address, QWidget *parent);

    void addr0_save_change(std::uint32_t val);

private slots:
    void cb_single_changed(int index);
    void cb_pipelined_changed(int index);

private:
    QComboBox *cb_single;
    QComboBox *cb_pipelined;
    QSettings *settings;
};

class ProgramDock : public QDockWidget {
    Q_OBJECT
public:
    ProgramDock(QWidget *parent, QSettings *settings);

    void setup(machine::QtMipsMachine *machine);

public slots:
    void jump_to_pc(std::uint32_t);

private:
    ProgramView *view;
};

#endif // PROGRAMDOCK_H
