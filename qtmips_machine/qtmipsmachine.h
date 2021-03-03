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

#include "core.h"
#include "machineconfig.h"
#include "memory/backend/lcddisplay.h"
#include "memory/backend/peripheral.h"
#include "memory/backend/peripspiled.h"
#include "memory/backend/serialport.h"
#include "memory/cache/cache.h"
#include "memory/memory_bus.h"
#include "qtmipsexception.h"
#include "registers.h"
#include "symboltable.h"

#include <QObject>
#include <QTimer>
#include <cstdint>

namespace machine {

class QtMipsMachine : public QObject {
    Q_OBJECT
public:
    explicit QtMipsMachine(
        MachineConfig config,
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
    const MemoryDataBus *memory_data_bus();
    MemoryDataBus *memory_data_bus_rw();
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
    bool memory_bus_insert_range(
        BackendMemory *mem_acces,
        Address start_addr,
        Address last_addr,
        bool move_ownership);

    void insert_hwbreak(Address address);
    void remove_hwbreak(Address address);
    bool is_hwbreak(Address address);
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
    MachineConfig machine_config;

    Registers *regs = nullptr;
    Memory *mem = nullptr;
    /**
     * Memory with loaded program only.
     * It is not used for execution, only for quick
     * simulation reset without repeated ELF file loading.
     */
    Memory *mem_program_only = nullptr;
    MemoryDataBus *data_bus = nullptr;
    SerialPort *ser_port = nullptr;
    PeripSpiLed *perip_spi_led = nullptr;
    LcdDisplay *perip_lcd_display = nullptr;
    Cache *cch_program = nullptr;
    Cache *cch_data = nullptr;
    Cop0State *cop0st = nullptr;
    Core *cr = nullptr;

    QTimer *run_t = nullptr;
    unsigned int time_chunk = { 0 };

    SymbolTable *symtab = nullptr;
    Address program_end = 0xffff0000_addr;
    enum Status stat = ST_READY;
    void set_status(enum Status st);
    void setup_serial_port();
    void setup_perip_spi_led();
    void setup_lcd_display();
};

} // namespace machine

#endif // QTMIPSMACHINE_H
