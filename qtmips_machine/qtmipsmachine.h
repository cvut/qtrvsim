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

#ifndef QTMIPSMACHINE_H
#define QTMIPSMACHINE_H

#include <QObject>
#include <QTimer>
#include <cstdint>
#include <qtmipsexception.h>
#include <machineconfig.h>
#include <registers.h>
#include <memory.h>
#include <core.h>
#include <cache.h>
#include <physaddrspace.h>
#include <peripheral.h>

namespace machine {

class QtMipsMachine : public QObject {
    Q_OBJECT
public:
    QtMipsMachine(const MachineConfig &cc);
    ~QtMipsMachine();

    const MachineConfig &config();
    void set_speed(unsigned);

    const Registers *registers();
    const Memory *memory();
    const Cache *cache_program();
    const Cache *cache_data();
    const Core *core();
    const CoreSingle *core_singe();
    const CorePipelined *core_pipelined();

    enum Status {
        ST_READY, // Machine is ready to be started or step to be called
        ST_RUNNING, // Machine is running
        ST_BUSY, // Machine is calculating step
        ST_EXIT, // Machine exited
        ST_TRAPPED // Machine exited with failure
    };
    enum Status status();
    bool exited();

    void register_exception_handler(ExceptionCause excause, ExceptionHandler *exhandler);

public slots:
    void play();
    void pause();
    void step();
    void restart();

signals:
    void program_exit();
    void program_trap(machine::QtMipsException &e);
    void status_change(enum machine::QtMipsMachine::Status st);
    void tick(); // Time tick
    void post_tick(); // Emitted after tick to allow updates

private:
    MachineConfig mcnf;

    Registers *regs;
    Memory *mem, *mem_program_only;
    PhysAddrSpace *physaddrspace;
    Cache *cch_program, *cch_data;
    Core *cr;

    QTimer *run_t;

    std::uint32_t program_end;
    enum Status stat;
    void set_status(enum Status st);
};

}

#endif // QTMIPSMACHINE_H
