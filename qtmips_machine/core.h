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

#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <qtmipsexception.h>
#include <machineconfig.h>
#include <registers.h>
#include <cop0state.h>
#include <memory.h>
#include <instruction.h>
#include <alu.h>

namespace machine {

class Core;

class ExceptionHandler : public QObject {
    Q_OBJECT
public:
    virtual bool handle_exception(Core *core, Registers *regs,
                          ExceptionCause excause, std::uint32_t inst_addr,
                          std::uint32_t next_addr, std::uint32_t jump_branch_pc,
                          bool in_delay_slot, std::uint32_t mem_ref_addr) =  0;
};

class StopExceptionHandler : public ExceptionHandler {
    Q_OBJECT
public:
    bool handle_exception(Core *core, Registers *regs,
                          ExceptionCause excause, std::uint32_t inst_addr,
                          std::uint32_t next_addr, std::uint32_t jump_branch_pc,
                          bool in_delay_slot, std::uint32_t mem_ref_addr);
};

class Core : public QObject {
    Q_OBJECT
public:
    Core(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data,
         unsigned int min_cache_row_size = 1, Cop0State *cop0state = nullptr);

    void step(bool skip_break = false); // Do single step
    void reset(); // Reset core (only core, memory and registers has to be reseted separately)

    unsigned cycles(); // Returns number of executed cycles

    Registers *get_regs();
    Cop0State *get_cop0state();
    MemoryAccess *get_mem_data();
    MemoryAccess *get_mem_program();
    void register_exception_handler(ExceptionCause excause, ExceptionHandler *exhandler);
    void insert_hwbreak(std::uint32_t address);
    void remove_hwbreak(std::uint32_t address);
    bool is_hwbreak(std::uint32_t address);
    void set_stop_on_exception(enum ExceptionCause excause, bool value);
    bool get_stop_on_exception(enum ExceptionCause excause) const;
    void set_step_over_exception(enum ExceptionCause excause, bool value);
    bool get_step_over_exception(enum ExceptionCause excause) const;

    void set_c0_userlocal(std::uint32_t address);

    enum ForwardFrom {
        FORWARD_NONE   = 0b00,
        FORWARD_FROM_W = 0b01,
        FORWARD_FROM_M = 0b10,
    };

signals:
    void instruction_fetched(const machine::Instruction &inst, std::uint32_t inst_addr, ExceptionCause excause);
    void instruction_decoded(const machine::Instruction &inst, std::uint32_t inst_addr, ExceptionCause excause);
    void instruction_executed(const machine::Instruction &inst, std::uint32_t inst_addr, ExceptionCause excause);
    void instruction_memory(const machine::Instruction &inst, std::uint32_t inst_addr, ExceptionCause excause);
    void instruction_writeback(const machine::Instruction &inst, std::uint32_t inst_addr, ExceptionCause excause);
    void instruction_program_counter(const machine::Instruction &inst, std::uint32_t inst_addr, ExceptionCause excause);

    void fetch_inst_addr_value(std::uint32_t);
    void fetch_jump_reg_value(std::uint32_t);
    void fetch_jump_value(std::uint32_t);
    void fetch_branch_value(std::uint32_t);
    void decode_inst_addr_value(std::uint32_t);
    void decode_instruction_value(std::uint32_t);
    void decode_reg1_value(std::uint32_t);
    void decode_reg2_value(std::uint32_t);
    void decode_immediate_value(std::uint32_t);
    void decode_regw_value(std::uint32_t);
    void decode_memtoreg_value(std::uint32_t);
    void decode_memwrite_value(std::uint32_t);
    void decode_memread_value(std::uint32_t);
    void decode_alusrc_value(std::uint32_t);
    void decode_regdest_value(std::uint32_t);
    void decode_rs_num_value(std::uint32_t);
    void decode_rt_num_value(std::uint32_t);
    void decode_rd_num_value(std::uint32_t);
    void decode_regd31_value(std::uint32_t);
    void forward_m_d_rs_value(std::uint32_t);
    void forward_m_d_rt_value(std::uint32_t);
    void execute_inst_addr_value(std::uint32_t);
    void execute_alu_value(std::uint32_t);
    void execute_reg1_value(std::uint32_t);
    void execute_reg2_value(std::uint32_t);
    void execute_reg1_ff_value(std::uint32_t);
    void execute_reg2_ff_value(std::uint32_t);
    void execute_immediate_value(std::uint32_t);
    void execute_regw_value(std::uint32_t);
    void execute_memtoreg_value(std::uint32_t);
    void execute_memwrite_value(std::uint32_t);
    void execute_memread_value(std::uint32_t);
    void execute_alusrc_value(std::uint32_t);
    void execute_regdest_value(std::uint32_t);
    void execute_regw_num_value(std::uint32_t);
    void execute_stall_forward_value(std::uint32_t);
    void execute_rs_num_value(std::uint32_t);
    void execute_rt_num_value(std::uint32_t);
    void execute_rd_num_value(std::uint32_t);
    void memory_inst_addr_value(std::uint32_t);
    void memory_alu_value(std::uint32_t);
    void memory_rt_value(std::uint32_t);
    void memory_mem_value(std::uint32_t);
    void memory_regw_value(std::uint32_t);
    void memory_memtoreg_value(std::uint32_t);
    void memory_memwrite_value(std::uint32_t);
    void memory_memread_value(std::uint32_t);
    void memory_regw_num_value(std::uint32_t);
    void memory_excause_value(std::uint32_t);
    void writeback_inst_addr_value(std::uint32_t);
    void writeback_value(std::uint32_t);
    void writeback_regw_value(std::uint32_t);
    void writeback_regw_num_value(std::uint32_t);

    void hu_stall_value(std::uint32_t);
    void branch_forward_value(std::uint32_t);

    void stop_on_exception_reached();

protected:
    virtual void do_step(bool skip_break = false) = 0;
    virtual void do_reset() = 0;

    bool handle_exception(Core *core, Registers *regs,
                     ExceptionCause excause, std::uint32_t inst_addr,
                     std::uint32_t next_addr, std::uint32_t jump_branch_pc,
                     bool in_delay_slot, std::uint32_t mem_ref_addr);

    Registers *regs;
    Cop0State *cop0state;
    MemoryAccess *mem_data, *mem_program;
    QMap<ExceptionCause, ExceptionHandler *> ex_handlers;
    ExceptionHandler *ex_default_handler;

    struct dtFetch {
        Instruction inst; // Loaded instruction
        uint32_t inst_addr; // Address of instruction
        enum ExceptionCause excause;
        bool in_delay_slot;
    };
    struct dtDecode {
        Instruction inst;
        bool memread; // If memory should be read
        bool memwrite; // If memory should write input
        bool alusrc; // If second value to alu is immediate value (rt used otherwise)
        bool regd; // If rd is used (otherwise rt is used for write target)
        bool regd31; // Use R31 as destionation for JAL
        bool regwrite; // If output should be written back to register (which one depends on regd)
        bool alu_req_rs; // requires rs value for ALU
        bool alu_req_rt; // requires rt value for ALU or SW
        bool bjr_req_rs; // requires rs for beq, bne, blez, bgtz, jr nad jalr
        bool bjr_req_rt; // requires rt for beq, bne
        bool branch;     // branch instruction
        bool jump;       // jump
        bool bj_not;     // negate branch condition
        bool bgt_blez;   // BGTZ/BLEZ instead of BGEZ/BLTZ
        bool nb_skip_ds; // Skip delay slot if branch is not taken
        bool forward_m_d_rs; // forwarding required for beq, bne, blez, bgtz, jr nad jalr
        bool forward_m_d_rt; // forwarding required for beq, bne
        enum AluOp aluop; // Decoded ALU operation
        enum AccessControl memctl; // Decoded memory access type
        std::uint8_t num_rs; // Number of the register s
        std::uint8_t num_rt; // Number of the register t
        std::uint8_t num_rd; // Number of the register d
        std::uint32_t val_rs; // Value from register rs
        std::uint32_t val_rt; // Value from register rt
        std::uint32_t immediate_val; // zero or sign-extended immediate value
        std::uint8_t rwrite; // Writeback register (multiplexed between rt and rd according to regd)
        ForwardFrom ff_rs;
        ForwardFrom ff_rt;
        uint32_t inst_addr; // Address of instruction
        enum ExceptionCause excause;
        bool in_delay_slot;
        bool stall;
        bool stop_if;
    };
    struct dtExecute {
        Instruction inst;
        bool memread;
        bool memwrite;
        bool regwrite;
        enum AccessControl memctl;
        std::uint32_t val_rt;
        std::uint8_t rwrite; // Writeback register (multiplexed between rt and rd according to regd)
        std::uint32_t alu_val; // Result of ALU execution
        uint32_t inst_addr; // Address of instruction
        enum ExceptionCause excause;
        bool in_delay_slot;
        bool stop_if;
    };
    struct dtMemory {
        Instruction inst;
        bool memtoreg;
        bool regwrite;
        std::uint8_t rwrite;
        std::uint32_t towrite_val;
        std::uint32_t mem_addr; // Address used to access memory
        uint32_t inst_addr; // Address of instruction
        enum ExceptionCause excause;
        bool in_delay_slot;
        bool stop_if;
    };

    struct dtFetch fetch(bool skip_break = false);
    struct dtDecode decode(const struct dtFetch&);
    struct dtExecute execute(const struct dtDecode&);
    struct dtMemory memory(const struct dtExecute&);
    void writeback(const struct dtMemory&);
    bool handle_pc(const struct dtDecode&, std::int32_t rel_adj = 0);

    enum ExceptionCause memory_special(enum AccessControl memctl,
                           int mode, bool memread, bool memwrite,
                           std::uint32_t &towrite_val,
                           std::uint32_t rt_value, std::uint32_t mem_addr);

    // Initialize structures to NOPE instruction
    void dtFetchInit(struct dtFetch &dt);
    void dtDecodeInit(struct dtDecode &dt);
    void dtExecuteInit(struct dtExecute &dt);
    void dtMemoryInit(struct dtMemory &dt);

private:
    struct hwBreak{
        hwBreak(std::uint32_t addr);
        std::uint32_t addr;
        unsigned int flags;
        unsigned int count;
    };
    unsigned int cycle_c;
    unsigned int min_cache_row_size;
    std::uint32_t hwr_userlocal;
    QMap<std::uint32_t, hwBreak *> hw_breaks;
    bool stop_on_exception[EXCAUSE_COUNT];
    bool step_over_exception[EXCAUSE_COUNT];
};

class CoreSingle : public Core {
public:
    CoreSingle(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data, bool jmp_delay_slot,
               unsigned int min_cache_row_size = 1, Cop0State *cop0state = nullptr);
    ~CoreSingle();

protected:
    void do_step(bool skip_break = false);
    void do_reset();

private:
    struct Core::dtDecode *jmp_delay_decode;
};

class CorePipelined : public Core {
public:
    CorePipelined(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data,
                  enum MachineConfig::HazardUnit hazard_unit = MachineConfig::HU_STALL_FORWARD,
                  unsigned int min_cache_row_size = 1, Cop0State *cop0state = nullptr);

protected:
    void do_step(bool skip_break = false);
    void do_reset();

private:
    struct Core::dtFetch dt_f;
    struct Core::dtDecode dt_d;
    struct Core::dtExecute dt_e;
    struct Core::dtMemory dt_m;

    enum MachineConfig::HazardUnit hazard_unit;
};

}

#endif // CORE_H
