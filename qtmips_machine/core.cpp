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

#include "core.h"
#include "programloader.h"
#include "utils.h"

using namespace machine;

Core::Core(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data,
           unsigned int min_cache_row_size) : ex_handlers(), hw_breaks() {
    cycle_c = 0;
    this->regs = regs;
    this->mem_program = mem_program;
    this->mem_data = mem_data;
    this->ex_default_handler = new StopExceptionHandler();
    this->min_cache_row_size = min_cache_row_size;
    this->hwr_userlocal = 0xe0000000;
}

void Core::step(bool skip_break) {
    cycle_c++;
    do_step(skip_break);
}

void Core::reset() {
    cycle_c = 0;
    do_reset();
}

unsigned Core::cycles() {
    return cycle_c;
}

Registers *Core::get_regs() {
    return regs;
}

MemoryAccess *Core::get_mem_data() {
    return mem_data;
}

MemoryAccess *Core::get_mem_program() {
    return mem_program;
}


Core::hwBreak::hwBreak(std::uint32_t addr) {
    this->addr = addr;
    flags = 0;
    count = 0;
}

void Core::insert_hwbreak(std::uint32_t address) {
    hw_breaks.insert(address, new hwBreak(address));
}

void Core::remove_hwbreak(std::uint32_t address) {
    hwBreak* hwbrk = hw_breaks.take(address);
    if (hwbrk != nullptr)
        delete hwbrk;
}

bool Core::is_hwbreak(std::uint32_t address) {
    hwBreak* hwbrk = hw_breaks.value(address);
    return hwbrk != nullptr;
}

void Core::register_exception_handler(ExceptionCause excause, ExceptionHandler *exhandler)
{
    if (excause == EXCAUSE_NONE ) {
        if (ex_default_handler != nullptr)
            delete ex_default_handler;
        ex_default_handler = exhandler;
    } else {
        ExceptionHandler *old = ex_handlers.take(excause);
        delete old;
        ex_handlers.insert(excause, exhandler);
    }
}

bool Core::handle_exception(Core *core, Registers *regs, ExceptionCause excause,
                      std::uint32_t inst_addr, std::uint32_t next_addr,
                      std::uint32_t jump_branch_pc, bool in_delay_slot,
                      std::uint32_t mem_ref_addr)
{
    if (excause == EXCAUSE_HWBREAK) {
        if (in_delay_slot)
            regs->pc_abs_jmp(jump_branch_pc);
        else
            regs->pc_abs_jmp(inst_addr);
    }

    ExceptionHandler *exhandler = ex_handlers.value(excause);
    if (exhandler != nullptr)
        return exhandler->handle_exception(core, regs, excause, inst_addr,
                                           next_addr, jump_branch_pc, in_delay_slot,
                                           mem_ref_addr);
    if (ex_default_handler != nullptr)
        return ex_default_handler->handle_exception(core, regs, excause, inst_addr,
                                                    next_addr, jump_branch_pc, in_delay_slot,
                                                    mem_ref_addr);
    return false;
}

void Core::set_c0_userlocal(std::uint32_t address) {
    hwr_userlocal = address;
}

enum ExceptionCause  Core::memory_special(enum AccessControl memctl,
                       int mode, bool memread, bool memwrite,
                       std::uint32_t &towrite_val,
                       std::uint32_t rt_value, std::uint32_t mem_addr) {
    std::uint32_t mask;
    std::uint32_t shift;
    std::uint32_t temp;
    (void)mode;

    switch (memctl) {
    case AC_CACHE_OP:
        mem_data->sync();
        mem_program->sync();
        break;
    case AC_STORE_CONDITIONAL:
        if (!memwrite)
            break;
        mem_data->write_ctl(AC_WORD, mem_addr, rt_value);
        towrite_val = 1;
        break;
    case AC_LOAD_LINKED:
        if (!memread)
            break;
        towrite_val = mem_data->read_ctl(AC_WORD, mem_addr);
        break;
    case AC_WORD_RIGHT:
        if (memwrite) {
            shift = (3 - (mem_addr & 3)) << 3;
            mask = 0xffffffff << shift;
            temp = mem_data->read_ctl(AC_WORD, mem_addr & ~3);
            temp = (temp & ~mask) | (rt_value << shift);
            mem_data->write_ctl(AC_WORD, mem_addr & ~3, temp);
        } else {
            shift = (3 - (mem_addr & 3)) << 3;
            mask = 0xffffffff >> shift;
            towrite_val = mem_data->read_ctl(AC_WORD, mem_addr & ~3);
            towrite_val = (towrite_val >> shift) | (rt_value & ~mask);
        }
        break;
    case AC_WORD_LEFT:
        if (memwrite) {
            shift = (mem_addr & 3) << 3;
            mask = 0xffffffff >> shift;
            temp = mem_data->read_ctl(AC_WORD, mem_addr & ~3);
            temp = (temp & ~mask) | (rt_value >> shift);
            mem_data->write_ctl(AC_WORD, mem_addr & ~3, temp);
        } else {
            shift = (mem_addr & 3) << 3;
            mask = 0xffffffff << shift;
            towrite_val = mem_data->read_ctl(AC_WORD, mem_addr & ~3);
            towrite_val = (towrite_val << shift) | (rt_value & ~mask);
        }
        break;
    default:
        break;
    }

    return EXCAUSE_NONE;
}


struct Core::dtFetch Core::fetch(bool skip_break) {
    enum ExceptionCause excause = EXCAUSE_NONE;
    std::uint32_t inst_addr = regs->read_pc();
    Instruction inst(mem_program->read_word(inst_addr));

    if (!skip_break) {
        hwBreak *brk = hw_breaks.value(inst_addr);
        if (brk != nullptr) {
            excause = EXCAUSE_HWBREAK;
        }
    }
    emit fetch_inst_addr_value(inst_addr);
    emit instruction_fetched(inst, inst_addr, excause);
    return {
        .inst = inst,
        .inst_addr = inst_addr,
        .excause = excause,
        .in_delay_slot = false,
    };
}

struct Core::dtDecode Core::decode(const struct dtFetch &dt) {
    uint8_t rwrite;
    enum InstructionFlags flags;
    enum AluOp alu_op;
    enum AccessControl mem_ctl;
    enum ExceptionCause excause = dt.excause;

    dt.inst.flags_alu_op_mem_ctl(flags, alu_op, mem_ctl);

    if (!(flags & IMF_SUPPORTED))
        throw QTMIPS_EXCEPTION(UnsupportedInstruction, "Instruction with following opcode is not supported", QString::number(dt.inst.opcode(), 16));

    std::uint32_t val_rs = regs->read_gp(dt.inst.rs());
    std::uint32_t val_rt = regs->read_gp(dt.inst.rt());
    std::uint32_t immediate_val;
    bool regwrite = flags & IMF_REGWRITE;
    bool regd = flags & IMF_REGD;
    bool regd31 = flags & IMF_PC_TO_R31;

    // requires rs for beq, bne, blez, bgtz, jr nad jalr
    bool bjr_req_rs = flags & IMF_BJR_REQ_RS;
    if (flags & IMF_PC8_TO_RT)
        val_rt = dt.inst_addr + 8;
    // requires rt for beq, bne
    bool bjr_req_rt = flags & IMF_BJR_REQ_RT;

    if (flags & IMF_ZERO_EXTEND)
        immediate_val = dt.inst.immediate();
    else
        immediate_val = sign_extend(dt.inst.immediate());

    if ((flags & IMF_EXCEPTION) && (excause == EXCAUSE_NONE)) {
        excause = dt.inst.encoded_exception();
    }

    emit decode_inst_addr_value(dt.inst_addr);
    emit instruction_decoded(dt.inst, dt.inst_addr, excause);
    emit decode_instruction_value(dt.inst.data());
    emit decode_reg1_value(val_rs);
    emit decode_reg2_value(val_rt);
    emit decode_immediate_value(immediate_val);
    emit decode_regw_value((bool)(flags & IMF_REGWRITE));
    emit decode_memtoreg_value((bool)(flags & IMF_MEMREAD));
    emit decode_memwrite_value((bool)(flags & IMF_MEMWRITE));
    emit decode_memread_value((bool)(flags & IMF_MEMREAD));
    emit decode_alusrc_value((bool)(flags & IMF_ALUSRC));
    emit decode_regdest_value((bool)(flags & IMF_REGD));
    emit decode_rs_num_value(dt.inst.rs());
    emit decode_rt_num_value(dt.inst.rt());
    emit decode_rd_num_value(dt.inst.rd());
    emit decode_regd31_value(regd31);

    if (regd31) {
        val_rt = dt.inst_addr + 8;
    }

    rwrite = regd31 ? 31: regd ? dt.inst.rd() : dt.inst.rt();

    return {
        .inst = dt.inst,
        .memread = !!(flags & IMF_MEMREAD),
        .memwrite = !!(flags & IMF_MEMWRITE),
        .alusrc = !!(flags & IMF_ALUSRC),
        .regd = regd,
        .regd31 = regd31,
        .regwrite = regwrite,
        .alu_req_rs = !!(flags & IMF_ALU_REQ_RS),
        .alu_req_rt = !!(flags & IMF_ALU_REQ_RT),
        .bjr_req_rs = bjr_req_rs,
        .bjr_req_rt = bjr_req_rt,
        .branch = !!(flags & IMF_BRANCH),
        .jump = !!(flags & IMF_JUMP),
        .bj_not = !!(flags & IMF_BJ_NOT),
        .bgt_blez = !!(flags & IMF_BGTZ_BLEZ),
        .nb_skip_ds = !!(flags & IMF_NB_SKIP_DS),
        .forward_m_d_rs = false,
        .forward_m_d_rt = false,
        .aluop = alu_op,
        .memctl = mem_ctl,
        .val_rs = val_rs,
        .val_rt = val_rt,
        .immediate_val = immediate_val,
        .rwrite = rwrite,
        .ff_rs = FORWARD_NONE,
        .ff_rt = FORWARD_NONE,
        .inst_addr = dt.inst_addr,
        .excause = excause,
        .in_delay_slot = dt.in_delay_slot,
        .stall = false,
    };
}

struct Core::dtExecute Core::execute(const struct dtDecode &dt) {
    bool discard;
    enum ExceptionCause excause = dt.excause;
    std::uint32_t alu_val = 0;

    // Handle conditional move (we have to change regwrite signal if conditional is not met)
    bool regwrite = dt.regwrite;

    std::uint32_t alu_sec = dt.val_rt;
    if (dt.alusrc)
        alu_sec = dt.immediate_val; // Sign or zero extend immediate value

    if (excause == EXCAUSE_NONE) {
        alu_val = alu_operate(dt.aluop, dt.val_rs,
                              alu_sec, dt.inst.shamt(), dt.inst.rd(), regs,
                              discard, excause);
        if (discard)
            regwrite = false;

        if (dt.aluop == ALU_OP_RDHWR) {
            switch (dt.inst.rd()) {
            case 0: // CPUNum
                alu_val = 0;
                break;
            case 1: // SYNCI_Step
                alu_val = min_cache_row_size;
                break;
            case 2: // CC
                alu_val = cycle_c;
                break;
            case 3: // CCRes
                alu_val = 1;
                break;
            case 29: // UserLocal
                alu_val = hwr_userlocal;
                break;
            default:
                alu_val = 0;
            }
        }
    }

    emit execute_inst_addr_value(dt.inst_addr);
    emit instruction_executed(dt.inst, dt.inst_addr, excause);
    emit execute_alu_value(alu_val);
    emit execute_reg1_value(dt.val_rs);
    emit execute_reg2_value(dt.val_rt);
    emit execute_reg1_ff_value(dt.ff_rs);
    emit execute_reg2_ff_value(dt.ff_rt);
    emit execute_immediate_value(dt.immediate_val);
    emit execute_regw_value(dt.regwrite);
    emit execute_memtoreg_value(dt.memread);
    emit execute_memread_value(dt.memread);
    emit execute_memwrite_value(dt.memwrite);
    emit execute_alusrc_value(dt.alusrc);
    emit execute_regdest_value(dt.regd);
    emit execute_regw_num_value(dt.rwrite);
    if (dt.stall)
        emit execute_stall_forward_value(1);
    else if (dt.ff_rs != FORWARD_NONE || dt.ff_rt != FORWARD_NONE)
        emit execute_stall_forward_value(2);
    else
        emit execute_stall_forward_value(0);

    return {
        .inst = dt.inst,
        .memread = dt.memread,
        .memwrite = dt.memwrite,
        .regwrite = regwrite,
        .memctl = dt.memctl,
        .val_rt = dt.val_rt,
        .rwrite = dt.rwrite,
        .alu_val = alu_val,
        .inst_addr = dt.inst_addr,
        .excause = excause,
        .in_delay_slot = dt.in_delay_slot,
    };
}

struct Core::dtMemory Core::memory(const struct dtExecute &dt) {
    std::uint32_t towrite_val = dt.alu_val;
    std::uint32_t mem_addr = dt.alu_val;
    enum ExceptionCause excause = dt.excause;
    bool memread = dt.memread;
    bool memwrite = dt.memwrite;
    bool regwrite = dt.regwrite;

    if (excause == EXCAUSE_NONE) {
        if (dt.memctl > AC_LAST_REGULAR) {
            excause = memory_special(dt.memctl, dt.inst.rt(), memread, memwrite,
                                 towrite_val, dt.val_rt, mem_addr);
        } else {
            if (memwrite)
                mem_data->write_ctl(dt.memctl, mem_addr, dt.val_rt);
            if (memread)
                towrite_val = mem_data->read_ctl(dt.memctl, mem_addr);
        }
    }

    if (dt.excause != EXCAUSE_NONE) {
        memread = false;
        memwrite = false;
        regwrite = false;
    }

    emit memory_inst_addr_value(dt.inst_addr);
    emit instruction_memory(dt.inst, dt.inst_addr, dt.excause);
    emit memory_alu_value(dt.alu_val);
    emit memory_rt_value(dt.val_rt);
    emit memory_mem_value(memread ? towrite_val : 0);
    emit memory_regw_value(regwrite);
    emit memory_memtoreg_value(dt.memread);
    emit memory_memread_value(dt.memread);
    emit memory_memwrite_value(memwrite);
    emit memory_regw_num_value(dt.rwrite);
    emit memory_excause_value(excause);

    return {
        .inst = dt.inst,
        .memtoreg = memread,
        .regwrite = regwrite,
        .rwrite = dt.rwrite,
        .towrite_val = towrite_val,
        .mem_addr = mem_addr,
        .inst_addr = dt.inst_addr,
        .excause = dt.excause,
        .in_delay_slot = dt.in_delay_slot,
    };
}

void Core::writeback(const struct dtMemory &dt) {
    emit writeback_inst_addr_value(dt.inst_addr);
    emit instruction_writeback(dt.inst, dt.inst_addr, dt.excause);
    emit writeback_value(dt.towrite_val);
    emit writeback_regw_value(dt.regwrite);
    emit writeback_regw_num_value(dt.rwrite);
    if (dt.regwrite)
        regs->write_gp(dt.rwrite, dt.towrite_val);
}

bool Core::handle_pc(const struct dtDecode &dt) {
    bool branch = false;
    emit instruction_program_counter(dt.inst, dt.inst_addr, EXCAUSE_NONE);

    if (dt.jump) {
        if (!dt.bjr_req_rs) {
            regs->pc_abs_jmp_28(dt.inst.address() << 2);
            emit fetch_jump_value(true);
            emit fetch_jump_reg_value(false);
        } else {
            regs->pc_abs_jmp(dt.val_rs);
            emit fetch_jump_value(false);
            emit fetch_jump_reg_value(true);
        }
        emit fetch_branch_value(false);
        return true;
    }

    if (dt.branch) {
        if (dt.bjr_req_rt) {
            branch = dt.val_rs == dt.val_rt;
        } else if (!dt.bgt_blez) {
            branch = (std::int32_t)dt.val_rs < 0;
        } else {
            branch = (std::int32_t)dt.val_rs <= 0;
        }

        if (dt.bj_not)
            branch = !branch;
    }

    emit fetch_jump_value(false);
    emit fetch_jump_reg_value(false);
    emit fetch_branch_value(branch);

    if (branch)
        regs->pc_jmp((std::int32_t)(((dt.inst.immediate() & 0x8000) ? 0xFFFF0000 : 0) | (dt.inst.immediate() << 2)));
    else
        regs->pc_inc();
    return branch;
}

void Core::dtFetchInit(struct dtFetch &dt) {
    dt.inst = Instruction(0x00);
    dt.excause = EXCAUSE_NONE;
    dt.in_delay_slot = false;
}

void Core::dtDecodeInit(struct dtDecode &dt) {
    dt.inst = Instruction(0x00);
    dt.memread = false;
    dt.memwrite = false;
    dt.alusrc = false;
    dt.regd = false;
    dt.regwrite = false;
    dt.bjr_req_rs = false; // requires rs for beq, bne, blez, bgtz, jr nad jalr
    dt.bjr_req_rt = false; // requires rt for beq, bne
    dt.jump = false;
    dt.bj_not = false;
    dt.bgt_blez = false;
    dt.nb_skip_ds = false;
    dt.forward_m_d_rs = false;
    dt.forward_m_d_rt = false;
    dt.aluop = ALU_OP_SLL;
    dt.memctl = AC_NONE;
    dt.val_rs = 0;
    dt.val_rt = 0;
    dt.rwrite = 0;
    dt.immediate_val = 0;
    dt.ff_rs = FORWARD_NONE;
    dt.ff_rt = FORWARD_NONE;
    dt.excause = EXCAUSE_NONE;
    dt.in_delay_slot = false;
    dt.stall = false;
}

void Core::dtExecuteInit(struct dtExecute &dt) {
    dt.inst = Instruction(0x00);
    dt.memread = false;
    dt.memwrite = false;
    dt.regwrite = false;
    dt.memctl = AC_NONE;
    dt.val_rt = 0;
    dt.rwrite = 0;
    dt.alu_val = 0;
    dt.excause = EXCAUSE_NONE;
    dt.in_delay_slot = false;
}

void Core::dtMemoryInit(struct dtMemory &dt) {
    dt.inst = Instruction(0x00);
    dt.memtoreg = false;
    dt.regwrite = false;
    dt.rwrite = false;
    dt.towrite_val = 0;
    dt.mem_addr = 0;
    dt.excause = EXCAUSE_NONE;
    dt.in_delay_slot = false;
}

CoreSingle::CoreSingle(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data, bool jmp_delay_slot) : \
    Core(regs, mem_program, mem_data) {
    if (jmp_delay_slot)
        jmp_delay_decode = new struct Core::dtDecode();
    else
        jmp_delay_decode = nullptr;
    reset();
}

CoreSingle::~CoreSingle() {
    if (jmp_delay_decode != nullptr)
        delete jmp_delay_decode;
}

void CoreSingle::do_step(bool skip_break) {
    bool in_delay_slot = false;
    std::uint32_t jump_branch_pc;

    struct dtFetch f = fetch(skip_break);
    struct dtDecode d = decode(f);

    // Handle PC before instruction following jump leaves decode stage
    if (jmp_delay_decode != nullptr) {
        in_delay_slot = handle_pc(*jmp_delay_decode);
        if (jmp_delay_decode->nb_skip_ds && !in_delay_slot) {
            // Discard processing of instruction in delay slot
            // for BEQL, BNEL, BLEZL, BGTZL, BLTZL, BGEZL, BLTZALL, BGEZALL
            dtDecodeInit(d);
        }
        jump_branch_pc = jmp_delay_decode->inst_addr;
        *jmp_delay_decode = d; // Copy current decode
    } else {
        handle_pc(d);
        jump_branch_pc = d.inst_addr;
    }

    struct dtExecute e = execute(d);
    struct dtMemory m = memory(e);
    writeback(m);

    if (m.excause != EXCAUSE_NONE) {
        if (jmp_delay_decode != nullptr)
            dtDecodeInit(*jmp_delay_decode);
        handle_exception(this, regs, m.excause, m.inst_addr, regs->read_pc(),
                         jump_branch_pc, in_delay_slot, m.mem_addr);
        return;
    }
}

void CoreSingle::do_reset() {
    if (jmp_delay_decode != nullptr) {
        Core::dtDecodeInit(*jmp_delay_decode);
        jmp_delay_decode->inst_addr = 0;
    }
}

CorePipelined::CorePipelined(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data, enum MachineConfig::HazardUnit hazard_unit) : \
    Core(regs, mem_program, mem_data) {
    this->hazard_unit = hazard_unit;
    reset();
}

void CorePipelined::do_step(bool skip_break) {
    bool stall = false;
    bool branch_stall = false;
    bool excpt_in_progress = false;
    std::uint32_t jump_branch_pc = dt_m.inst_addr;

    // Process stages
    writeback(dt_m);
    dt_m = memory(dt_e);
    dt_e = execute(dt_d);
    dt_d = decode(dt_f);

    // Resolve exceptions
    excpt_in_progress = dt_m.excause != EXCAUSE_NONE;
    if (excpt_in_progress) {
        dtExecuteInit(dt_e);
        emit instruction_executed(dt_e.inst, dt_e.inst_addr, dt_e.excause);
    }
    excpt_in_progress = excpt_in_progress || dt_e.excause != EXCAUSE_NONE;
    if (excpt_in_progress) {
        dtDecodeInit(dt_d);
        emit instruction_decoded(dt_d.inst, dt_d.inst_addr, dt_d.excause);
    }
    excpt_in_progress = excpt_in_progress || dt_e.excause != EXCAUSE_NONE;
    if (excpt_in_progress) {
        dtFetchInit(dt_f);
        emit instruction_fetched(dt_f.inst, dt_f.inst_addr, dt_f.excause);
        if (dt_m.excause != EXCAUSE_NONE) {
            regs->pc_abs_jmp(dt_e.inst_addr);
            handle_exception(this, regs, dt_m.excause, dt_m.inst_addr,
                             dt_e.inst_addr, jump_branch_pc,
                             dt_m.in_delay_slot, dt_m.mem_addr);
        }
        return;
    }

    dt_d.ff_rs = FORWARD_NONE;
    dt_d.ff_rt = FORWARD_NONE;

    if (hazard_unit != MachineConfig::HU_NONE) {
        // Note: We make exception with $0 as that has no effect when written and is used in nop instruction

#define HAZARD(STAGE) ( \
            (STAGE).regwrite && (STAGE).rwrite != 0 && \
            ((dt_d.alu_req_rs && (STAGE).rwrite == dt_d.inst.rs()) ||  \
             (dt_d.alu_req_rt && (STAGE).rwrite == dt_d.inst.rt())) \
        ) // Note: We make exception with $0 as that has no effect and is used in nop instruction

        // Write back stage combinatoricly propagates written instruction to decode stage so nothing has to be done for that stage
        if (HAZARD(dt_m)) {
            // Hazard with instruction in memory stage
            if (hazard_unit == MachineConfig::HU_STALL_FORWARD) {
                // Forward result value
                if (dt_d.alu_req_rs && dt_m.rwrite == dt_d.inst.rs()) {
                    dt_d.val_rs = dt_m.towrite_val;
                    dt_d.ff_rs = FORWARD_FROM_W;
                }
                if (dt_d.alu_req_rt && dt_m.rwrite == dt_d.inst.rt()) {
                    dt_d.val_rt = dt_m.towrite_val;
                    dt_d.ff_rt = FORWARD_FROM_W;
                }
            } else
                stall = true;
        }
        if (HAZARD(dt_e)) {
            // Hazard with instruction in execute stage
            if (hazard_unit == MachineConfig::HU_STALL_FORWARD) {
                if (dt_e.memread)
                    stall = true;
                else {
                    // Forward result value
                    if (dt_d.alu_req_rs && dt_e.rwrite == dt_d.inst.rs()) {
                        dt_d.val_rs = dt_e.alu_val;
                        dt_d.ff_rs = FORWARD_FROM_M;
                     }
                    if (dt_d.alu_req_rt && dt_e.rwrite == dt_d.inst.rt()) {
                        dt_d.val_rt = dt_e.alu_val;
                        dt_d.ff_rt = FORWARD_FROM_M;
                    }
                }
            } else
                stall = true;
        }
#undef HAZARD
        if (dt_e.rwrite != 0 && dt_e.regwrite &&
            ((dt_d.bjr_req_rs && dt_d.inst.rs() == dt_e.rwrite) ||
             (dt_d.bjr_req_rt && dt_d.inst.rt() == dt_e.rwrite))) {
            stall = true;
            branch_stall = true;
        } else {
            if (hazard_unit != MachineConfig::HU_STALL_FORWARD || dt_m.memtoreg) {
                if (dt_m.rwrite != 0 && dt_m.regwrite &&
                    ((dt_d.bjr_req_rs && dt_d.inst.rs() == dt_m.rwrite) ||
                     (dt_d.bjr_req_rt && dt_d.inst.rt() == dt_m.rwrite)))
                    stall = true;
            } else {
                if (dt_m.rwrite != 0 && dt_m.regwrite &&
                    dt_d.bjr_req_rs && dt_d.inst.rs() == dt_m.rwrite) {
                    dt_d.val_rs = dt_m.towrite_val;
                    dt_d.forward_m_d_rs = true;
                }
                if (dt_m.rwrite != 0 && dt_m.regwrite &&
                    dt_d.bjr_req_rt && dt_d.inst.rt() == dt_m.rwrite) {
                    dt_d.val_rt = dt_m.towrite_val;
                    dt_d.forward_m_d_rt = true;
                }
            }
        }
        emit forward_m_d_rs_value(dt_d.forward_m_d_rs);
        emit forward_m_d_rt_value(dt_d.forward_m_d_rt);
    }
    emit branch_forward_value((dt_d.forward_m_d_rs || dt_d.forward_m_d_rt)? 2: branch_stall);
#if 0
    if (stall)
        printf("STALL\n");
    else if(dt_d.forward_m_d_rs || dt_d.forward_m_d_rt)
        printf("f_m_d_rs %d f_m_d_rt %d\n", (int)dt_d.forward_m_d_rs, (int)dt_d.forward_m_d_rt);
    printf("D: %s inst.type %d dt_d.inst.rs [%d] dt_d.inst.rt [%d] dt_d.ff_rs %d dt_d.ff_rt %d E: regwrite %d inst.type %d  rwrite [%d] M: regwrite %d inst.type %d rwrite [%d] \n",
            dt_d.inst.to_str().toLocal8Bit().data(),
            dt_d.inst.type(), dt_d.inst.rs(), dt_d.inst.rt(), dt_d.ff_rs, dt_d.ff_rt,
            dt_e.regwrite, dt_e.inst.type(), dt_e.rwrite,
            dt_m.regwrite,  dt_m.inst.type(), dt_m.rwrite);
#endif
#if 0
    printf("PC 0x%08lx\n", (unsigned long)dt_f.inst_addr);
#endif

    emit hu_stall_value(stall);

    // Now process program counter (loop connections from decode stage)
    if (!stall) {
        dt_d.stall = false;
        dt_f = fetch(skip_break);
        if (handle_pc(dt_d)) {
            dt_f.in_delay_slot = true;
        } else {
            if (dt_d.nb_skip_ds) {
                dtFetchInit(dt_f);
                emit instruction_fetched(dt_f.inst, dt_f.inst_addr, dt_f.excause);
            }
        }
    } else {
        // Run fetch stage on empty
        fetch(skip_break);
        // clear decode latch (insert nope to execute stage)
        dtDecodeInit(dt_d);
        dt_d.stall = true;
        // emit instruction_decoded(dt_d.inst, dt_d.inst_addr, dt_d.excause);
    }
}

void CorePipelined::do_reset() {
    dtFetchInit(dt_f);
    dt_f.inst_addr = 0;
    dtDecodeInit(dt_d);
    dt_d.inst_addr = 0;
    dtExecuteInit(dt_e);
    dt_e.inst_addr = 0;
    dtMemoryInit(dt_m);
    dt_m.inst_addr = 0;
}

bool StopExceptionHandler::handle_exception(Core *core, Registers *regs,
                            ExceptionCause excause, std::uint32_t inst_addr,
                            std::uint32_t next_addr, std::uint32_t jump_branch_pc,
                            bool in_delay_slot, std::uint32_t mem_ref_addr) {
#if 0
    printf("Exception cause %d instruction PC 0x%08lx next PC 0x%08lx jump branch PC 0x%08lx "
           "in_delay_slot %d registers PC 0x%08lx mem ref 0x%08lx\n",
           excause, (unsigned long)inst_addr, (unsigned long)next_addr,
           (unsigned long)jump_branch_pc, (int)in_delay_slot,
           (unsigned long)regs->read_pc(), (unsigned long)mem_ref_addr);
#else
    (void)excause; (void)inst_addr; (void)next_addr; (void)mem_ref_addr; (void)regs;
    (void)jump_branch_pc; (void)in_delay_slot;
#endif
    emit core->stop_on_exception_reached();
    return true;
};
