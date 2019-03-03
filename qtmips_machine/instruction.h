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
#include <QString>

#include "machinedefs.h"

namespace machine {

enum InstructionFlags {
    IMF_NONE       = 0,
    IMF_SUPPORTED  = 1L<<0, /**< Instruction is supported */
    IMF_MEMWRITE   = 1L<<1, /**< Write to the memory when memory stage is reached */
    IMF_MEMREAD    = 1L<<2, /**< Read from the memory when memory stage is reached */
    IMF_ALUSRC     = 1L<<3, /**< The second ALU source is immediate operand */
    IMF_REGD       = 1L<<4, /**< RD field specifies register to be updated, if not
                                 set and REGWRITE = 1, then destination reg in RT */
    IMF_REGWRITE   = 1L<<5, /**< Instruction result (ALU or memory) is written to
                                 register file */
    IMF_ZERO_EXTEND= 1L<<6, /**< Immediate operand is zero extended, else sign */
    IMF_PC_TO_R31  = 1L<<7, /**< PC value will be stored to register R31/RA */
    IMF_BJR_REQ_RS = 1L<<8, /**< Branch or jump operation reguires RS value */
    IMF_BJR_REQ_RT = 1L<<9, /**< Branch or jump operation requires RT value */
    IMF_ALU_SHIFT  = 1L<<10, /**< Operation is shift of RT by RS or SHAMT */
    IMF_MEM        = 1L<<11, /**< Instruction is memory access instruction */
    IMF_ALU_REQ_RS = 1L<<12, /**< Execution phase/ALU requires RS value */
    IMF_ALU_REQ_RT = 1L<<13, /**< Execution phase/ALU/mem requires RT value */
    IMF_READ_HILO  = 1L<<14, /**< Operation reads value from  HI or LO registers */
    IMF_WRITE_HILO = 1L<<15, /**< Operation writes value to HI and/or LO registers */
    IMF_PC8_TO_RT  = 1L<<16, /**< PC value will be stored in RT specified register */
    IMF_BRANCH     = 1L<<17, /**< Operation is conditional or unconditional branch
                                  or branch and link when PC_TO_R31 is set */
    IMF_JUMP       = 1L<<18, /**< Jump operation - J, JAL, JR or JALR */
    IMF_BJ_NOT     = 1L<<19, /**< Negate condition for branch instructiion */
    IMF_BGTZ_BLEZ  = 1L<<20, /**< BGTZ/BLEZ, else BEGT/BLTZ or BEQ, BNE when RT */
    IMF_NB_SKIP_DS = 1L<<21, /**< Skip instruction in delay slot if branch not taken */
    IMF_EXCEPTION  = 1L<<22, /**< Instruction causes synchronous exception */
    IMF_STOP_IF    = 1L<<23, /**< Stop instruction fetch until instruction processed */
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
    std::uint8_t cop0sel() const;
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

    QString to_str(std::int32_t inst_addr = 0) const;

    static Instruction from_string(QString str, bool *pok = nullptr);
private:
    std::uint32_t dt;
};

}

Q_DECLARE_METATYPE(machine::Instruction)

#endif // INSTRUCTION_H
