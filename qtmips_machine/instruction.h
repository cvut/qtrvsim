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

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <QObject>
#include <qstring.h>

#include "machinedefs.h"

namespace machine {

enum InstructionFlags {
    IMF_NONE       = 0,
    IMF_SUPPORTED  = 1L<<0,
    IMF_MEMWRITE   = 1L<<1,
    IMF_MEMREAD    = 1L<<2,
    IMF_ALUSRC     = 1L<<3,
    IMF_REGD       = 1L<<4,
    IMF_REGWRITE   = 1L<<5,
    IMF_ZERO_EXTEND= 1L<<6,
    IMF_PC_TO_R31  = 1L<<7,
    IMF_BJR_REQ_RS = 1L<<8,
    IMF_BJR_REQ_RT = 1L<<9,
    IMF_ALU_SHIFT = 1L<<10,
    IMF_MEM        = 1L<<11, // This instruction is memory access instruction,
    IMF_MEM_STORE  = 1L<<12,
    IMF_ALU_REQ_RS = 1L<<13,
    IMF_ALU_REQ_RT = 1L<<14,
    IMF_READ_HILO  = 1L<<15,
    IMF_WRITE_HILO = 1L<<16,
    IMF_PC8_TO_RT  = 1L<<17,
    IMF_BRANCH     = 1L<<18,
    IMF_JUMP       = 1L<<19,
    IMF_BJ_NOT     = 1L<<20,
    IMF_BGTZ_BLEZ  = 1L<<21,
    IMF_EXCEPTION  = 1L<<22,
};

class Instruction {
public:
    Instruction();
    Instruction(std::uint32_t inst);
    Instruction(std::uint8_t opcode, std::uint8_t rs, std::uint8_t rt, std::uint8_t rd, std::uint8_t shamt, std::uint8_t funct); // Type R
    Instruction(std::uint8_t opcode, std::uint8_t rs, std::uint8_t rt, std::uint16_t immediate); // Type I
    Instruction(std::uint8_t opcode, std::uint32_t address); // Type J
    Instruction(const Instruction&);

    enum Type {
        T_R,
        T_I,
        T_J,
        T_UNKNOWN
    };

    std::uint8_t opcode() const;
    std::uint8_t rs() const;
    std::uint8_t rt() const;
    std::uint8_t rd() const;
    std::uint8_t shamt() const;
    std::uint8_t funct() const;
    std::uint16_t immediate() const;
    std::uint32_t address() const;
    std::uint32_t data() const;
    enum Type type() const;
    enum InstructionFlags flags() const;
    enum AluOp alu_op() const;
    enum AccessControl mem_ctl() const;
    enum ExceptionCause encoded_exception() const;

    void flags_alu_op_mem_ctl(enum InstructionFlags &flags,
                  enum AluOp &alu_op, enum AccessControl &mem_ctl) const;

    bool is_break() const;

    bool operator==(const Instruction &c) const;
    bool operator!=(const Instruction &c) const;
    Instruction &operator=(const Instruction &c);

    QString to_str() const;

private:
    std::uint32_t dt;
};

}

Q_DECLARE_METATYPE(machine::Instruction)

#endif // INSTRUCTION_H
