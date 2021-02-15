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

#include "cache.h"
#include "core.h"
#include "lcddisplay.h"
#include "machineconfig.h"
#include "peripheral.h"
#include "peripspiled.h"
#include "physaddrspace.h"
#include "qtmipsexception.h"
#include "registers.h"
#include "serialport.h"
#include "symboltable.h"

#include <QObject>
#include <QTimer>

namespace machine {

class QtMipsMachine : public QObject {
    Q_OBJECT
public:
    QtMipsMachine(
        const MachineConfig &cc,
        bool load_symtab = false,
        bool load_executable = true);
    ~QtMipsMachine() override;

    const MachineConfig &config();
    void set_speed(unsigned int ips, unsigned int time_chunk = 0);

    const Registers *registers();
    const Cop0State *cop0state();
    const Memory *memory();
    Memory *memory_rw();
    const Cache *cache_program();
    const Cache *cache_data();
    Cache *cache_data_rw();
    void cache_sync();
    const PhysAddrSpace *physical_address_space();
    PhysAddrSpace *physical_address_space_rw();
    SerialPort *serial_port();
    PeripSpiLed *peripheral_spi_led();
    LcdDisplay *peripheral_lcd_display();
    const SymbolTable *symbol_table(bool create = false);
    SymbolTable *symbol_table_rw(bool create = false);
    void set_symbol(
        const QString &name,
        uint32_t value,
        uint32_t size,
        unsigned char info = 0,
        unsigned char other = 0);
    const Core *core();
    const CoreSingle *core_singe();
    const CorePipelined *core_pipelined();
    bool executable_loaded() const;

    enum Status {
        ST_READY,   // Machine is ready to be started or step to be called
        ST_RUNNING, // Machine is running
        ST_BUSY,    // Machine is calculating step
        ST_EXIT,    // Machine exited
        ST_TRAPPED  // Machine exited with failure
    };
    enum Status status();
    bool exited();

    void register_exception_handler(
        ExceptionCause excause,
        ExceptionHandler *exhandler);
    bool addressapce_insert_range(
        MemoryAccess *mem_acces,
        uint32_t start_addr,
        uint32_t last_addr,
        bool move_ownership);

    void insert_hwbreak(uint32_t address);
    void remove_hwbreak(uint32_t address);
    bool is_hwbreak(uint32_t address);
    void set_stop_on_exception(enum ExceptionCause excause, bool value);
    bool get_stop_on_exception(enum ExceptionCause excause) const;
    void set_step_over_exception(enum ExceptionCause excause, bool value);
    bool get_step_over_exception(enum ExceptionCause excause) const;
    enum ExceptionCause get_exception_cause() const;

public slots:
    void play();
    void pause();
    void step();
    void restart();

signals:
    void program_exit();
    void program_trap(machine::QtMipsException &e);
    void status_change(enum machine::QtMipsMachine::Status st);
    void tick();      // Time tick
    void post_tick(); // Emitted after tick to allow updates
    void set_interrupt_signal(uint irq_num, bool active);

private slots:
    void step_timer();

private:
    void step_internal(bool skip_break = false);
    MachineConfig mcnf;

    Registers *regs;
    Memory *mem, *mem_program_only;
    PhysAddrSpace *physaddrspace;
    SerialPort *ser_port;
    PeripSpiLed *perip_spi_led;
    LcdDisplay *perip_lcd_display;
    Cache *cch_program, *cch_data;
    Cop0State *cop0st;
    Core *cr;

    QTimer *run_t;
    unsigned int time_chunk {};

    SymbolTable *symtab;
    uint32_t program_end;
    enum Status stat;
    void set_status(enum Status st);
};

} // namespace machine

#endif // QTMIPSMACHINE_H
