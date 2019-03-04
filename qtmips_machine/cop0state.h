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

#ifndef COP0STATE_H
#define COP0STATE_H

#include <QObject>
#include <QString>
#include <cstdint>
#include <machinedefs.h>
namespace machine {

class Core;

class Cop0State : public QObject {
    Q_OBJECT
    friend class Core;
public:
    enum Cop0Registers {
        Unsupported = 0,
        UserLocal,
        BadVAddr, // Reports the address for the most recent address-related exception
        Count,    // Processor cycle count
        Compare,  // Timer interrupt control
        Status,   // Processor status and control
        Cause,    // Cause of last exception
        EPC,      // Program counter at last exception
        EBase,    // Exception vector base register
        Config,   // Configuration registers
        COP0REGS_CNT,
    };

    enum StatusReg {
        Status_IE =      0x00000001,
        Status_EXL =     0x00000002,
        Status_ERL =     0x00000004,
        Status_IntMask = 0x0000ff00,
        Status_Int0 =    0x00000100,
    };

    Cop0State(Core *core = nullptr);
    Cop0State(const Cop0State&);

    std::uint32_t read_cop0reg(enum Cop0Registers reg) const;
    std::uint32_t read_cop0reg(std::uint8_t rd, std::uint8_t sel) const; // Read coprocessor 0 register
    void write_cop0reg(enum Cop0Registers reg, std::uint32_t value);
    void write_cop0reg(std::uint8_t reg, std::uint8_t sel, std::uint32_t value); // Write coprocessor 0 register
    static QString cop0reg_name(enum Cop0Registers reg);

    bool operator ==(const Cop0State &c) const;
    bool operator !=(const Cop0State &c) const;

    void reset(); // Reset all values to zero

    bool core_interrupt_request();
    std::uint32_t exception_pc_address();

signals:
    void cop0reg_update(enum Cop0Registers reg, std::uint32_t val);

public slots:
    void set_interrupt_signal(uint irq_num, bool active);
    void set_status_exl(bool value);

protected:
    void setup_core(Core *core);
    void update_execption_cause(enum ExceptionCause excause, bool in_delay_slot);
    void update_count_and_compare_irq();

private:

    typedef std::uint32_t (Cop0State::*reg_read_t)
                  (enum Cop0Registers reg) const;

    typedef void (Cop0State::*reg_write_t)
                  (enum Cop0Registers reg, std::uint32_t value);

    struct cop0reg_desc_t {
        const char *name;
        std::uint32_t write_mask;
        std::uint32_t init_value;
        reg_read_t reg_read;
        reg_write_t reg_write;

    };

    static const cop0reg_desc_t cop0reg_desc[COP0REGS_CNT];

    std::uint32_t read_cop0reg_default(enum Cop0Registers reg) const;
    void write_cop0reg_default(enum Cop0Registers reg, std::uint32_t value);
    void write_cop0reg_count_compare(enum Cop0Registers reg, std::uint32_t value);
    void write_cop0reg_user_local(enum Cop0Registers reg, std::uint32_t value);
    Core *core;
    std::uint32_t cop0reg[COP0REGS_CNT]; // coprocessor 0 registers
    std::uint32_t last_core_cycles;
};

}

Q_DECLARE_METATYPE(machine::Cop0State)
Q_DECLARE_METATYPE(machine::Cop0State::Cop0Registers)

#endif // COP0STATE_H
