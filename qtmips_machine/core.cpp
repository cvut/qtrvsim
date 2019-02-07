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

Core::Core(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data) :
          ex_handlers() {
    cycle_c = 0;
    this->regs = regs;
    this->mem_program = mem_program;
    this->mem_data = mem_data;
    ex_default_handler = new StopExceptionHandler();
}

void Core::step() {
    cycle_c++;
    do_step();
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
                      uint32_t mem_ref_addr)
{
    ExceptionHandler *exhandler = ex_handlers.value(excause);
    if (exhandler != nullptr)
        return exhandler->handle_exception(core, regs, excause, inst_addr, next_addr, mem_ref_addr);
    if (ex_default_handler != nullptr)
        return ex_default_handler->handle_exception(core, regs, excause, inst_addr, next_addr, mem_ref_addr);
    return false;
}

struct Core::dtFetch Core::fetch() {
    std::uint32_t inst_addr = regs->read_pc();
    Instruction inst(mem_program->read_word(inst_addr));
    emit instruction_fetched(inst);
    return {
        .inst = inst,
        .inst_addr = inst_addr,
        .excause = EXCAUSE_NONE,
    };
}

struct Core::dtDecode Core::decode(const struct dtFetch &dt) {
    uint8_t rwrite;
    emit instruction_decoded(dt.inst);
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
        .memread = flags & IMF_MEMREAD,
        .memwrite = flags & IMF_MEMWRITE,
        .alusrc = flags & IMF_ALUSRC,
        .regd = regd,
        .regd31 = regd31,
        .regwrite = regwrite,
        .alu_req_rs = flags & IMF_ALU_REQ_RS,
        .alu_req_rt = flags & IMF_ALU_REQ_RT,
        .bjr_req_rs = bjr_req_rs,
        .bjr_req_rt = bjr_req_rt,
        .branch = flags & IMF_BRANCH,
        .jump = flags & IMF_JUMP,
        .bj_not = flags & IMF_BJ_NOT,
        .bgt_blez = flags & IMF_BGTZ_BLEZ,
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
    };
}

struct Core::dtExecute Core::execute(const struct dtDecode &dt) {
    emit instruction_executed(dt.inst);
    bool discard;

    // Handle conditional move (we have to change regwrite signal if conditional is not met)
    bool regwrite = dt.regwrite;

    std::uint32_t alu_sec = dt.val_rt;
    if (dt.alusrc)
        alu_sec = dt.immediate_val; // Sign or zero extend immediate value

    std::uint32_t alu_val = alu_operate(dt.aluop, dt.val_rs, alu_sec, dt.inst.shamt(), regs, discard);
    if (discard)
        regwrite = false;

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
        .excause = dt.excause,
    };
}

struct Core::dtMemory Core::memory(const struct dtExecute &dt) {
    emit instruction_memory(dt.inst);
    std::uint32_t towrite_val = dt.alu_val;
    std::uint32_t mem_addr = dt.alu_val;
    bool memread = dt.memread;
    bool memwrite = dt.memwrite;
    bool regwrite = dt.regwrite;

    if (dt.excause != EXCAUSE_NONE) {
        memread = false;
        memwrite = false;
        regwrite = false;
    } else {
        if (dt.memctl == AC_CACHE_OP)
             mem_data->sync();
        else if (memwrite)
             mem_data->write_ctl(dt.memctl, mem_addr, dt.val_rt);
        else if (memread)
             towrite_val = mem_data->read_ctl(dt.memctl, mem_addr);
    }

    emit memory_alu_value(dt.alu_val);
    emit memory_rt_value(dt.val_rt);
    emit memory_mem_value(memread ? towrite_val : 0);
    emit memory_regw_value(regwrite);
    emit memory_memtoreg_value(dt.memread);
    emit memory_memread_value(dt.memread);
    emit memory_memwrite_value(memwrite);
    emit memory_regw_num_value(dt.rwrite);

    return {
        .inst = dt.inst,
        .regwrite = regwrite,
        .rwrite = dt.rwrite,
        .towrite_val = towrite_val,
         .mem_addr = mem_addr,
        .inst_addr = dt.inst_addr,
        .excause = dt.excause,
    };
}

void Core::writeback(const struct dtMemory &dt) {
    emit instruction_writeback(dt.inst);
    emit writeback_value(dt.towrite_val);
    emit writeback_regw_value(dt.regwrite);
    emit writeback_regw_num_value(dt.rwrite);
    if (dt.regwrite)
        regs->write_gp(dt.rwrite, dt.towrite_val);
}

void Core::handle_pc(const struct dtDecode &dt) {
    bool branch = false;
    emit instruction_program_counter(dt.inst);

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
        return;
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
}

void Core::dtFetchInit(struct dtFetch &dt) {
    dt.inst = Instruction(0x00);
    dt.excause = EXCAUSE_NONE;
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
}

void Core::dtMemoryInit(struct dtMemory &dt) {
    dt.inst = Instruction(0x00);
    dt.regwrite = false;
    dt.rwrite = false;
    dt.towrite_val = 0;
    dt.mem_addr = 0;
    dt.excause = EXCAUSE_NONE;
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

void CoreSingle::do_step() {
    struct dtFetch f = fetch();
    struct dtDecode d = decode(f);
    struct dtExecute e = execute(d);
    struct dtMemory m = memory(e);
    writeback(m);

    if (jmp_delay_decode != nullptr) {
        handle_pc(*jmp_delay_decode);
        *jmp_delay_decode = d; // Copy current decode
    } else
        handle_pc(d);

    if (m.excause != EXCAUSE_NONE) {
        if (jmp_delay_decode != nullptr)
            dtDecodeInit(*jmp_delay_decode);
        handle_exception(this, regs, m.excause, m.inst_addr, regs->read_pc(), m.mem_addr);
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

void CorePipelined::do_step() {
    bool stall = false;
    bool excpt_in_progress = false;

    // Process stages
    writeback(dt_m);
    dt_m = memory(dt_e);
    dt_e = execute(dt_d);
    dt_d = decode(dt_f);

    // Resolve exceptions
    excpt_in_progress = dt_m.excause != EXCAUSE_NONE;
    if (excpt_in_progress)
        dtExecuteInit(dt_e);
    excpt_in_progress = excpt_in_progress || dt_e.excause != EXCAUSE_NONE;
    if (excpt_in_progress)
        dtDecodeInit(dt_d);
    excpt_in_progress = excpt_in_progress || dt_e.excause != EXCAUSE_NONE;
    if (excpt_in_progress) {
        dtFetchInit(dt_f);
        if (dt_m.excause != EXCAUSE_NONE) {
            regs->pc_abs_jmp(dt_e.inst_addr);
            handle_exception(this, regs, dt_m.excause, dt_m.inst_addr, dt_e.inst_addr, dt_m.mem_addr);
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
                    dt_d.ff_rs = FORWARD_FROM_M;
                }
                if (dt_d.alu_req_rt && dt_m.rwrite == dt_d.inst.rt()) {
                    dt_d.val_rt = dt_m.towrite_val;
                    dt_d.ff_rt = FORWARD_FROM_M;
                }
            } else
                stall = true;
        }
        if (HAZARD(dt_e)) {
            // Hazard with instruction in execute stage
            if (hazard_unit == MachineConfig::HU_STALL_FORWARD) {
                if (dt_e.memread) // TODO extend by branch instructions
                    stall = true;
                else {
                    // Forward result value
                    if (dt_d.alu_req_rs && dt_e.rwrite == dt_d.inst.rs()) {
                        dt_d.val_rs = dt_e.alu_val;
                        dt_d.ff_rs = FORWARD_FROM_W;
                     }
                    if (dt_d.alu_req_rt && dt_e.rwrite == dt_d.inst.rt()) {
                        dt_d.val_rt = dt_e.alu_val;
                        dt_d.ff_rt = FORWARD_FROM_W;
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
        } else {
            if (hazard_unit != MachineConfig::HU_STALL_FORWARD) {
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

    // Now process program counter (loop connections from decode stage)
    if (!stall) {
        dt_f = fetch();
        handle_pc(dt_d);
    } else {
        // Run fetch stage on empty
        fetch();
        // clear decode latch (insert nope to execute stage)
        dtDecodeInit(dt_d);
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
     ExceptionCause excause, std::uint32_t inst_addr, std::uint32_t next_addr,
     std::uint32_t mem_ref_addr) {

#if 0
    printf("Exception cause %d instruction PC 0x%08lx next PC 0x%08lx registers PC 0x%08lx mem ref 0x%08lx\n",
           excause, (unsigned long)inst_addr, (unsigned long)next_addr,
           (unsigned long)regs->read_pc(), (unsigned long)mem_ref_addr);
#else
    (void)excause; (void)inst_addr; (void)next_addr; (void)mem_ref_addr;
#endif
    emit core->stop_on_exception_reached();
    return true;
};
