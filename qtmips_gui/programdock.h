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
#include <QLabel>
#include <QComboBox>
#include "peripheralsview.h"
#include "qtmipsmachine.h"

class ProgramDock : public QDockWidget  {
    Q_OBJECT

    using Super = QDockWidget;

public:
    ProgramDock(QWidget *parent, QSettings *settings);

    void setup(machine::QtMipsMachine *machine);

signals:
    void machine_setup(machine::QtMipsMachine *machine);
    void jump_to_pc(std::uint32_t);
    void focus_addr(std::uint32_t);
    void focus_addr_with_save(std::uint32_t);
    void stage_addr_changed(uint stage, std::uint32_t addr);
public slots:
    void set_follow_inst(int);
    void fetch_inst_addr(std::uint32_t addr);
    void decode_inst_addr(std::uint32_t addr);
    void execute_inst_addr(std::uint32_t addr);
    void memory_inst_addr(std::uint32_t addr);
    void writeback_inst_addr(std::uint32_t addr);
    void report_error(QString error);
private:
    enum FollowSource {
        FOLLOWSRC_NONE,
        FOLLOWSRC_FETCH,
        FOLLOWSRC_DECODE,
        FOLLOWSRC_EXECUTE,
        FOLLOWSRC_MEMORY,
        FOLLOWSRC_WRITEBACK,
        FOLLOWSRC_COUNT,
    };

    void update_follow_position();
    enum FollowSource follow_source;
    std::uint32_t follow_addr[FOLLOWSRC_COUNT];
    QSettings *settings;
};

#endif // PROGRAMDOCK_H
