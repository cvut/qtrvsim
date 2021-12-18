#include "core.h"

#include "common/logging.h"
#include "execute/alu.h"
#include "utils.h"

LOG_CATEGORY("machine.core");

using namespace machine;

Core::Core(
    Registers *regs,
    Predictor *predictor,
    FrontendMemory *mem_program,
    FrontendMemory *mem_data,
    Cop0State *cop0state,
    Xlen xlen)
    : if_id(state.pipeline.fetch.final)
    , id_ex(state.pipeline.decode.final)
    , ex_mem(state.pipeline.execute.final)
    , mem_wb(state.pipeline.memory.final)
    , xlen(xlen)
    , regs(regs)
    , cop0state(cop0state)
    , predictor(predictor)
    , mem_data(mem_data)
    , mem_program(mem_program)
    , ex_handlers()
    , ex_default_handler(new StopExceptionHandler()) {
    if (cop0state != nullptr) { cop0state->setup_core(this); }
    stop_on_exception.fill(true);
    step_over_exception.fill(true);
    step_over_exception[EXCAUSE_INT] = false;
}

void Core::step(bool skip_break) {
    state.cycle_count++;
    do_step(skip_break);
    emit step_done(state);
}

void Core::reset() {
    state.cycle_count = 0;
    state.stall_count = 0;
    do_reset();
}

unsigned Core::get_cycle_count() const {
    return state.cycle_count;
}

unsigned Core::get_stall_count() const {
    return state.stall_count;
}

Registers *Core::get_regs() const {
    return regs;
}

Cop0State *Core::get_cop0state() const {
    return cop0state;
}

FrontendMemory *Core::get_mem_data() const {
    return mem_data;
}

FrontendMemory *Core::get_mem_program() const {
    return mem_program;
}

Predictor *Core::get_predictor() const {
    return predictor;
}

const CoreState &Core::get_state() const {
    return state;
}

void Core::insert_hwbreak(Address address) {
    hw_breaks.insert(address, new hwBreak(address));
}

void Core::remove_hwbreak(Address address) {
    hwBreak *hwbrk = hw_breaks.take(address);
    delete hwbrk;
}

bool Core::is_hwbreak(Address address) const {
    hwBreak *hwbrk = hw_breaks.value(address);
    return hwbrk != nullptr;
}

void Core::set_stop_on_exception(enum ExceptionCause excause, bool value) {
    stop_on_exception[excause] = value;
}

bool Core::get_stop_on_exception(enum ExceptionCause excause) const {
    return stop_on_exception[excause];
}

void Core::set_step_over_exception(enum ExceptionCause excause, bool value) {
    step_over_exception[excause] = value;
}

bool Core::get_step_over_exception(enum ExceptionCause excause) const {
    return step_over_exception[excause];
}

void Core::register_exception_handler(ExceptionCause excause, ExceptionHandler *exhandler) {
    if (excause == EXCAUSE_NONE) {
        ex_default_handler.reset(exhandler);
    } else {
        ExceptionHandler *old = ex_handlers.take(excause);
        delete old;
        ex_handlers.insert(excause, exhandler);
    }
}

bool Core::handle_exception(
    ExceptionCause excause,
    Instruction inst,
    Address inst_addr,
    Address next_addr,
    Address jump_branch_pc,
    Address mem_ref_addr) {
    if (excause == EXCAUSE_UNKNOWN) {
        throw SIMULATOR_EXCEPTION(
            UnsupportedInstruction, "Instruction with following encoding is not supported",
            QString::number(inst.data(), 16));
    }

    if (excause == EXCAUSE_HWBREAK) { regs->write_pc(inst_addr); }

    if (cop0state != nullptr) {
        cop0state->write_cop0reg(Cop0State::EPC, inst_addr.get_raw());
        cop0state->update_execption_cause(excause);
        if (cop0state->read_cop0reg(Cop0State::EBase) != 0 && !get_step_over_exception(excause)) {
            cop0state->set_status_exl(true);
            regs->write_pc(cop0state->exception_pc_address());
        }
    }

    bool ret = false;
    ExceptionHandler *exhandler = ex_handlers.value(excause, ex_default_handler.data());
    if (exhandler != nullptr) {
        ret = exhandler->handle_exception(
            this, regs, excause, inst_addr, next_addr, jump_branch_pc, mem_ref_addr);
    }

    if (get_stop_on_exception(excause)) { emit stop_on_exception_reached(); }

    return ret;
}

void Core::set_c0_userlocal(uint32_t address) {
    if (cop0state != nullptr) {
        if (address != cop0state->read_cop0reg(Cop0State::UserLocal)) {
            cop0state->write_cop0reg(Cop0State::UserLocal, address);
        }
    }
}

enum ExceptionCause Core::memory_special(
    enum AccessControl memctl,
    int mode,
    bool memread,
    bool memwrite,
    RegisterValue &towrite_val,
    RegisterValue rt_value,
    Address mem_addr) {
    Q_UNUSED(mode)

    switch (memctl) {
    case AC_CACHE_OP:
        mem_data->sync();
        mem_program->sync();
        break;
    case AC_STORE_CONDITIONAL:
        if (!memwrite) { break; }
        mem_data->write_u32(mem_addr, rt_value.as_u32());
        towrite_val = 1;
        break;
    case AC_LOAD_LINKED:
        if (!memread) { break; }
        towrite_val = mem_data->read_u32(mem_addr);
        break;
    default: break;
    }

    return EXCAUSE_NONE;
}

FetchState Core::fetch(bool skip_break) {
    enum ExceptionCause excause = EXCAUSE_NONE;
    Address inst_addr = Address(regs->read_pc());
    Instruction inst(mem_program->read_u32(inst_addr));

    if (!skip_break) {
        hwBreak *brk = hw_breaks.value(inst_addr);
        if (brk != nullptr) { excause = EXCAUSE_HWBREAK; }
    }
    if (cop0state != nullptr && excause == EXCAUSE_NONE) {
        if (cop0state->core_interrupt_request()) { excause = EXCAUSE_INT; }
    }

    return { FetchInternalState { .fetched_value = inst.data() }, FetchInterstage {
                                                                      .inst = inst,
                                                                      .inst_addr = inst_addr,
                                                                      .excause = excause,
                                                                      .is_valid = true,
                                                                  } };
}

DecodeState Core::decode(const FetchInterstage &dt) {
    enum InstructionFlags flags;
    enum AluOp alu_op;
    enum AccessControl mem_ctl;
    enum ExceptionCause excause = dt.excause;

    dt.inst.flags_alu_op_mem_ctl(flags, alu_op, mem_ctl);

    if (!(flags & IMF_SUPPORTED)) { excause = EXCAUSE_UNKNOWN; }

    uint8_t num_rs = dt.inst.rs();
    uint8_t num_rt = dt.inst.rt();
    uint8_t num_rd = dt.inst.rd();
    RegisterValue val_rs = regs->read_gp(num_rs);
    RegisterValue val_rt = regs->read_gp(num_rt);
    int32_t immediate_val = dt.inst.immediate();
    bool regwrite = flags & IMF_REGWRITE;

    if ((flags & IMF_EXCEPTION) && (excause == EXCAUSE_NONE)) {
        if (flags & IMF_EBREAK) {
            excause = EXCAUSE_BREAK;
        } else if (flags & IMF_ECALL) {
            excause = EXCAUSE_SYSCALL;
        }
    }

    return { DecodeInternalState {
                 .alu_op_num = static_cast<unsigned>(alu_op),
                 .excause_num = static_cast<unsigned>(excause),
                 .inst_bus = dt.inst.data(),
             },
             DecodeInterstage { .inst = dt.inst,
                                .memread = bool(flags & IMF_MEMREAD),
                                .memwrite = bool(flags & IMF_MEMWRITE),
                                .alusrc = bool(flags & IMF_ALUSRC),
                                .regwrite = regwrite,
                                .alu_req_rs = bool(flags & IMF_ALU_REQ_RS),
                                .alu_req_rt = bool(flags & IMF_ALU_REQ_RT),
                                .branch = bool(flags & IMF_BRANCH),
                                .jump = bool(flags & IMF_JUMP),
                                .bj_not = bool(flags & IMF_BJ_NOT),
                                .aluop = alu_op,
                                .memctl = mem_ctl,
                                .num_rs = num_rs,
                                .num_rt = num_rt,
                                .num_rd = num_rd,
                                .val_rs = val_rs,
                                .val_rs_orig = val_rs,
                                .val_rt = val_rt,
                                .val_rt_orig = val_rt,
                                .immediate_val = immediate_val,
                                .ff_rs = FORWARD_NONE,
                                .ff_rt = FORWARD_NONE,
                                .inst_addr = dt.inst_addr,
                                .excause = excause,
                                .stall = false,
                                .is_valid = dt.is_valid,
                                .alu_mod = bool(flags & IMF_ALU_MOD),
                                .alu_pc = bool(flags & IMF_PC_TO_ALU) } };
}

ExecuteState Core::execute(const DecodeInterstage &dt) {
    enum ExceptionCause excause = dt.excause;
    RegisterValue alu_fst
        = dt.alu_pc ? RegisterValue(dt.inst_addr.get_raw()) : (dt.alu_req_rs ? dt.val_rs : 0);
    RegisterValue alu_sec = dt.alusrc ? dt.immediate_val : dt.val_rt;

    RegisterValue alu_val = 0;
    if (excause == EXCAUSE_NONE) {
        alu_val = alu_combined_operate(
            { .alu_op = dt.aluop }, AluComponent::ALU, true, dt.alu_mod, alu_fst, alu_sec);
    }
    Address target = 0_addr;
    if (dt.branch) target = dt.inst_addr + dt.immediate_val.as_i64();

    const unsigned stall_status = [&]() {
        if (dt.stall) {
            return 1;
        } else if (dt.ff_rs != FORWARD_NONE || dt.ff_rt != FORWARD_NONE) {
            return 2;
        } else {
            return 0;
        }
    }();

    return { ExecuteInternalState {
                 .alu_src = dt.alusrc,
                 .branch = dt.branch,
                 .alu_pc = dt.alu_pc,
                 .alu_src1 = dt.val_rs,
                 .alu_src2 = alu_sec,
                 .immediate = dt.immediate_val,
                 .rs = dt.val_rs_orig,
                 .rt = dt.val_rt_orig,
                 .stall_status = stall_status,
                 .alu_op_num = static_cast<unsigned>(dt.aluop),
                 .forward_from_rs1_num = static_cast<unsigned>(dt.ff_rs),
                 .forward_from_rs2_num = static_cast<unsigned>(dt.ff_rt),
                 .excause_num = static_cast<unsigned>(dt.excause),
             },
             ExecuteInterstage {
                 .inst = dt.inst,
                 .memread = dt.memread,
                 .memwrite = dt.memwrite,
                 .regwrite = dt.regwrite,
                 .is_valid = dt.is_valid,
                 .branch = dt.branch,
                 // In the diagram `branch_taken` is computed in memory/compute_next_pc, but here it
                 // can be done on one place only.
                 .branch_taken = dt.branch && (!dt.bj_not ^ (alu_val != 0)),
                 .jump = dt.jump,
                 .bj_not = dt.bj_not,
                 .alu_zero = alu_val == 0,
                 .memctl = dt.memctl,
                 .val_rt = dt.val_rt,
                 .num_rd = dt.num_rd,
                 .alu_val = alu_val,
                 .inst_addr = dt.inst_addr,
                 .excause = excause,
                 .branch_target = target,
             } };
}

MemoryState Core::memory(const ExecuteInterstage &dt) {
    RegisterValue towrite_val = dt.alu_val;
    Address mem_addr = Address(get_xlen_from_reg(dt.alu_val));
    bool memread = dt.memread;
    bool memwrite = dt.memwrite;
    bool regwrite = dt.regwrite;

    enum ExceptionCause excause = dt.excause;
    if (excause == EXCAUSE_NONE) {
        if (is_special_access(dt.memctl)) {
            excause = memory_special(
                dt.memctl, dt.inst.rt(), memread, memwrite, towrite_val, dt.val_rt, mem_addr);
        } else if (is_regular_access(dt.memctl)) {
            if (memwrite) { mem_data->write_ctl(dt.memctl, mem_addr, dt.val_rt); }
            if (memread) { towrite_val = mem_data->read_ctl(dt.memctl, mem_addr); }
        } else {
            Q_ASSERT(dt.memctl == AC_NONE);
            // AC_NONE is memory NOP
        }
    }

    if (dt.excause != EXCAUSE_NONE) {
        memread = false;
        memwrite = false;
        regwrite = false;
    }

    return { MemoryInternalState {
                 .memwrite = memwrite,
                 .memread = memread,
                 .branch = dt.branch,
                 .jump = dt.jump,
                 .branch_or_jump = dt.branch_taken || dt.jump,
                 .mem_read_val = towrite_val,
                 .mem_write_val = dt.val_rt,
                 .excause_num = static_cast<unsigned>(excause),
                 .mem_addr = dt.alu_val,
             },
             MemoryInterstage {
                 .inst = dt.inst,
                 .memtoreg = memread,
                 .regwrite = regwrite,
                 .num_rd = dt.num_rd,
                 .towrite_val = towrite_val,
                 .mem_addr = mem_addr,
                 .inst_addr = dt.inst_addr,
                 .next_pc = compute_next_pc(dt),
                 .excause = dt.excause,
                 .is_valid = dt.is_valid,
             } };
}

WritebackState Core::writeback(const MemoryInterstage &dt) {
    if (dt.regwrite) { regs->write_gp(dt.num_rd, dt.towrite_val); }

    return { WritebackInternalState {
        .inst = dt.inst,
        .inst_addr = dt.inst_addr,
        .regwrite = dt.regwrite,
        .memtoreg = dt.memtoreg,
        .num_rd = dt.num_rd,
        .value = dt.towrite_val,
    } };
}

Address Core::compute_next_pc(const ExecuteInterstage &exec) const {
    if (exec.jump) { return Address(get_xlen_from_reg(exec.alu_val)); }
    if (exec.branch_taken) { return exec.branch_target; }
    return exec.inst_addr + 4;
}

uint64_t Core::get_xlen_from_reg(RegisterValue reg) const {
    switch (this->xlen) {
    case Xlen::_32: return reg.as_u32();
    case Xlen::_64: return reg.as_u64();
    }
}

CoreSingle::CoreSingle(
    Registers *regs,
    Predictor *predictor,
    FrontendMemory *mem_program,
    FrontendMemory *mem_data,
    Cop0State *cop0state,
    Xlen xlen)
    : Core(regs, predictor, mem_program, mem_data, cop0state, xlen) {
    reset();
}

void CoreSingle::do_step(bool skip_break) {
    Pipeline &p = state.pipeline;

    p.fetch = fetch(skip_break);
    p.decode = decode(p.fetch.final);
    p.execute = execute(p.decode.final);
    p.memory = memory(p.execute.final);
    p.writeback = writeback(p.memory.final);

    regs->write_pc(mem_wb.next_pc);

    if (mem_wb.excause != EXCAUSE_NONE) {
        handle_exception(
            mem_wb.excause, mem_wb.inst, mem_wb.inst_addr, regs->read_pc(), prev_inst_addr,
            mem_wb.mem_addr);
        return;
    }
    prev_inst_addr = mem_wb.inst_addr;
}

void CoreSingle::do_reset() {
    state.pipeline = {};
    prev_inst_addr = Address::null();
}

CorePipelined::CorePipelined(
    Registers *regs,
    Predictor *predictor,
    FrontendMemory *mem_program,
    FrontendMemory *mem_data,
    enum MachineConfig::HazardUnit hazard_unit,
    unsigned int min_cache_row_size,
    Cop0State *cop0state,
    Xlen xlen)
    : Core(regs, predictor, mem_program, mem_data, cop0state, xlen) {
    this->hazard_unit = hazard_unit;
    reset();
}

void CorePipelined::do_step(bool skip_break) {
    Pipeline &p = state.pipeline;

    const Address jump_branch_pc = mem_wb.inst_addr;
    const FetchInterstage saved_if_id = if_id;

    p.writeback = writeback(mem_wb);
    p.memory = memory(ex_mem);
    p.execute = execute(id_ex);
    p.decode = decode(if_id);
    p.fetch = fetch(skip_break);

    bool stall = false;
    if (hazard_unit != MachineConfig::HU_NONE) { stall |= handle_data_hazards(); }

    // After all stages are processed one of the following options will handle the PC.
    if (mem_wb.excause != EXCAUSE_NONE) {
        flush_and_continue_from_address(ex_mem.inst_addr);
        handle_exception(
            mem_wb.excause, mem_wb.inst, mem_wb.inst_addr, ex_mem.inst_addr, jump_branch_pc,
            mem_wb.mem_addr);
    } else if (stall) {
        // PC is not allowed to advance.
        handle_stall(saved_if_id);
    } else if (detect_mispredicted_jump()) {
        flush_and_continue_from_address(mem_wb.next_pc);
    } else {
        regs->write_pc(predictor->predict(if_id.inst, if_id.inst_addr));
    }
}
void CorePipelined::flush_and_continue_from_address(Address next_pc) {
    regs->write_pc(next_pc);
    if_id.flush();
    id_ex.flush();
    ex_mem.flush();
}

void CorePipelined::handle_stall(const FetchInterstage &saved_if_id) {
    /*
     * Stall handing:
     * - IF fetches new instruction, but it is not allowed to save into IF/ID register. This is
     * simulated by restoring the `if_id` to its original value.
     * - ID continues normally. On next cycle, perform the same as before as IF/ID will be
     * unchanged.
     * - EX is where stall is inserted by flush. The flushed instruction will be re-executed
     * as ID repeats its execution.
     */
    if_id = saved_if_id;
    id_ex.flush();
    id_ex.stall = true; // for visualization
    state.stall_count++;
}

bool CorePipelined::detect_mispredicted_jump() const {
    return mem_wb.is_valid && ex_mem.is_valid && mem_wb.next_pc != ex_mem.inst_addr;
}

template<typename InterstageReg>
bool is_hazard_in_stage(const InterstageReg &interstage, const DecodeInterstage &id_ex) {
    return (
        interstage.regwrite && interstage.num_rd != 0
        && ((id_ex.alu_req_rs && interstage.num_rd == id_ex.num_rs)
            || (id_ex.alu_req_rt && interstage.num_rd == id_ex.num_rt)));
    // Note: We make exception with $0 as that has no effect and is used in nop instruction
}

bool CorePipelined::handle_data_hazards() {
    // Note: We make exception with $0 as that has no effect when
    // written and is used in nop instruction
    bool stall = false;

    if (is_hazard_in_stage(mem_wb, id_ex)) {
        if (hazard_unit == MachineConfig::HU_STALL_FORWARD) {
            // Forward result value
            if (id_ex.alu_req_rs && mem_wb.num_rd == id_ex.num_rs) {
                id_ex.val_rs = mem_wb.towrite_val;
                id_ex.ff_rs = FORWARD_FROM_W;
            }
            if (id_ex.alu_req_rt && mem_wb.num_rd == id_ex.num_rt) {
                id_ex.val_rt = mem_wb.towrite_val;
                id_ex.ff_rt = FORWARD_FROM_W;
            }
        } else {
            stall = true;
        }
    }
    if (is_hazard_in_stage(ex_mem, id_ex)) {
        if (hazard_unit == MachineConfig::HU_STALL_FORWARD) {
            if (ex_mem.memread) {
                stall = true;
            } else {
                // Forward result value
                if (id_ex.alu_req_rs && ex_mem.num_rd == id_ex.num_rs) {
                    id_ex.val_rs = ex_mem.alu_val;
                    id_ex.ff_rs = FORWARD_FROM_M;
                }
                if (id_ex.alu_req_rt && ex_mem.num_rd == id_ex.num_rt) {
                    id_ex.val_rt = ex_mem.alu_val;
                    id_ex.ff_rt = FORWARD_FROM_M;
                }
            }
        } else {
            stall = true;
        }
    }
    return stall;
}

void CorePipelined::do_reset() {
    state.pipeline = {};
}

bool StopExceptionHandler::handle_exception(
    Core *core,
    Registers *regs,
    ExceptionCause excause,
    Address inst_addr,
    Address next_addr,
    Address jump_branch_pc,
    Address mem_ref_addr) {
    Q_UNUSED(core)
    DEBUG(
        "Exception cause %d instruction PC 0x%08lx next PC 0x%08lx jump branch PC 0x%08lx "
        "registers PC 0x%08lx mem ref 0x%08lx",
        excause, inst_addr.get_raw(), next_addr.get_raw(), jump_branch_pc.get_raw(),
        regs->read_pc().get_raw(), mem_ref_addr.get_raw());
    return true;
}
