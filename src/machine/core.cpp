#include "core.h"

#include "common/logging.h"
#include "execute/alu.h"
#include "utils.h"

#include <cinttypes>

LOG_CATEGORY("machine.core");

using namespace machine;

static InstructionFlags unsupported_inst_flags_to_check(Xlen xlen,
                            ConfigIsaWord isa_word) {
    unsigned flags_to_check = IMF_SUPPORTED;
    if (xlen == Xlen::_32)
        flags_to_check |= IMF_RV64;
    if (!isa_word.contains('A'))
        flags_to_check |= IMF_AMO;
    if (!isa_word.contains('M'))
        flags_to_check |= IMF_MUL;
    return InstructionFlags(flags_to_check);
}

Core::Core(Registers *regs,
    Predictor *predictor,
    FrontendMemory *mem_program,
    FrontendMemory *mem_data,
    CSR::ControlState *control_state,
    Xlen xlen, ConfigIsaWord isa_word)
    : pc_if(state.pipeline.pc.final)
    , if_id(state.pipeline.fetch.final)
    , id_ex(state.pipeline.decode.final)
    , ex_mem(state.pipeline.execute.final)
    , mem_wb(state.pipeline.memory.final)
    , xlen(xlen)
    , check_inst_flags_val(IMF_SUPPORTED)
    , check_inst_flags_mask(unsupported_inst_flags_to_check(xlen, isa_word))
    , regs(regs)
    , control_state(control_state)
    , predictor(predictor)
    , mem_data(mem_data)
    , mem_program(mem_program)
    , ex_handlers()
    , ex_default_handler(new StopExceptionHandler()) {
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

CSR::ControlState *Core::get_control_state() const {
    return control_state;
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
Xlen Core::get_xlen() const {
    return xlen;
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
    if (excause == EXCAUSE_INSN_ILLEGAL) {
        throw SIMULATOR_EXCEPTION(
            UnsupportedInstruction, "Instruction with following encoding is not supported",
            QString::number(inst.data(), 16));
    }

    if (excause == EXCAUSE_HWBREAK) { regs->write_pc(inst_addr); }

    if (control_state != nullptr) {
        control_state->write_internal(CSR::Id::MEPC, inst_addr.get_raw());
        control_state->update_exception_cause(excause);
        if (control_state->read_internal(CSR::Id::MTVEC) != 0
            && !get_step_over_exception(excause)) {
            control_state->exception_initiate(CSR::PrivilegeLevel::MACHINE, CSR::PrivilegeLevel::MACHINE);
            regs->write_pc(control_state->exception_pc_address());
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

static int32_t amo32_operations(enum AccessControl memctl, int32_t a, int32_t b) {
    switch(memctl) {
    case AC_AMOSWAP32: return b;
    case AC_AMOADD32:  return a + b;
    case AC_AMOXOR32:  return a ^ b;
    case AC_AMOAND32:  return a & b;
    case AC_AMOOR32:   return a | b;
    case AC_AMOMIN32:  return a < b? a: b;
    case AC_AMOMAX32:  return a < b? b: a;
    case AC_AMOMINU32: return (uint32_t)a < (uint32_t)b? a: b;
    case AC_AMOMAXU32: return (uint32_t)a < (uint32_t)b? b: a;
    default: break;
    }
    return 0;
}

static int64_t amo64_operations(enum AccessControl memctl, int64_t a, int64_t b) {
    switch(memctl) {
    case AC_AMOSWAP64: return b;
    case AC_AMOADD64:  return a + b;
    case AC_AMOXOR64:  return a ^ b;
    case AC_AMOAND64:  return a & b;
    case AC_AMOOR64:   return a | b;
    case AC_AMOMIN64:  return a < b? a: b;
    case AC_AMOMAX64:  return a < b? b: a;
    case AC_AMOMINU64: return (uint64_t)a < (uint64_t)b? a: b;
    case AC_AMOMAXU64: return (uint64_t)a < (uint64_t)b? b: a;
    default: break;
    }
    return 0;
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
    case AC_LR32:
        if (!memread) { break; }
        state.LoadReservedRange = AddressRange(mem_addr, mem_addr + 3);
        towrite_val = (int32_t)(mem_data->read_u32(mem_addr));
        break;
    case AC_SC32:
        if (!memwrite) { break; }
        if (state.LoadReservedRange.contains(AddressRange(mem_addr, mem_addr + 3))) {
            mem_data->write_u32(mem_addr, rt_value.as_u32());
            towrite_val = 0;
        } else {
            towrite_val = 1;
        }
        state.LoadReservedRange.reset();
        break;
    case AC_LR64:
        if (!memread) { break; }
        state.LoadReservedRange = AddressRange(mem_addr, mem_addr + 7);
        towrite_val = mem_data->read_u64(mem_addr);
        break;
    case AC_SC64:
        if (!memwrite) { break; }
        if (state.LoadReservedRange.contains(AddressRange(mem_addr, mem_addr + 7))) {
            mem_data->write_u64(mem_addr, rt_value.as_u64());
            towrite_val = 0;
        } else {
            towrite_val = 1;
        }
        break;
    case AC_FISRT_AMO_MODIFY32 ... AC_LAST_AMO_MODIFY32:
    {
        if (!memread || !memwrite) { break; }
        int32_t fetched_value;
        fetched_value = (int32_t)(mem_data->read_u32(mem_addr));
        towrite_val = amo32_operations(memctl, fetched_value, rt_value.as_u32());
        mem_data->write_u32(mem_addr, towrite_val.as_u32());
        towrite_val = fetched_value;
        break;
    }
    case AC_FISRT_AMO_MODIFY64 ... AC_LAST_AMO_MODIFY64:
    {
        if (!memread || !memwrite) { break; }
        int64_t fetched_value;
        fetched_value = (int64_t)(mem_data->read_u64(mem_addr));
        towrite_val = (uint64_t)amo64_operations(memctl, fetched_value, rt_value.as_u64());
        mem_data->write_u64(mem_addr, towrite_val.as_u64());
        towrite_val = fetched_value;
        break;
    }
    default: break;
    }

    return EXCAUSE_NONE;
}

FetchState Core::fetch(PCInterstage pc, bool skip_break) {
    if (pc.stop_if) { return {}; }

    const Address inst_addr = Address(regs->read_pc());
    const Instruction inst(mem_program->read_u32(inst_addr));
    ExceptionCause excause = EXCAUSE_NONE;

    if (!skip_break && hw_breaks.contains(inst_addr)) { excause = EXCAUSE_HWBREAK; }

    if (control_state != nullptr) {
        control_state->increment_internal(CSR::Id::MCYCLE, 1);
    }

    if (control_state != nullptr && excause == EXCAUSE_NONE) {
        if (control_state->core_interrupt_request()) { excause = EXCAUSE_INT; }
    }

    return { FetchInternalState { .fetched_value = inst.data() },
             FetchInterstage {
                 .inst = inst,
                 .inst_addr = inst_addr,
                 .next_inst_addr = inst_addr + inst.size(),
                 .predicted_next_inst_addr = predictor->predict(inst, inst_addr),
                 .excause = excause,
                 .is_valid = true,
             } };
}

DecodeState Core::decode(const FetchInterstage &dt) {
    InstructionFlags flags;
    bool w_operation = this->xlen != Xlen::_64;
    AluCombinedOp alu_op {};
    AccessControl mem_ctl;
    ExceptionCause excause = dt.excause;

    dt.inst.flags_alu_op_mem_ctl(flags, alu_op, mem_ctl);

    if ((flags ^ check_inst_flags_val) & check_inst_flags_mask) {
        excause = EXCAUSE_INSN_ILLEGAL;
    }

    RegisterId num_rs = (flags & (IMF_ALU_REQ_RS | IMF_ALU_RS_ID)) ? dt.inst.rs() : 0;
    RegisterId num_rt = (flags & IMF_ALU_REQ_RT) ? dt.inst.rt() : 0;
    RegisterId num_rd = (flags & IMF_REGWRITE) ? dt.inst.rd() : 0;
    // When instruction does not specify register, it is set to x0 as operations on x0 have no
    // side effects (not even visualization).
    RegisterValue val_rs
        = (flags & IMF_ALU_RS_ID) ? uint64_t(size_t(num_rs)) : regs->read_gp(num_rs);
    RegisterValue val_rt = regs->read_gp(num_rt);
    RegisterValue immediate_val = dt.inst.immediate();
    const bool regwrite = flags & IMF_REGWRITE;

    CSR::Address csr_address = (flags & IMF_CSR) ? dt.inst.csr_address() : CSR::Address(0);
    RegisterValue csr_read_val
        = ((control_state != nullptr && (flags & IMF_CSR))) ? control_state->read(csr_address) : 0;
    bool csr_write = (flags & IMF_CSR) && (!(flags & IMF_CSR_TO_ALU) || (num_rs != 0));

    if ((flags & IMF_EXCEPTION) && (excause == EXCAUSE_NONE)) {
        if (flags & IMF_EBREAK) {
            excause = EXCAUSE_BREAK;
        } else if (flags & IMF_ECALL) {
            excause = EXCAUSE_ECALL_M;
            // TODO: EXCAUSE_ECALL_S, EXCAUSE_ECALL_U
        }
    }
    if (flags & IMF_FORCE_W_OP)
        w_operation = true;

    return { DecodeInternalState {
                 .alu_op_num = static_cast<unsigned>(alu_op.alu_op),
                 .excause_num = static_cast<unsigned>(excause),
                 .inst_bus = dt.inst.data(),
                 .alu_mul = bool(flags & IMF_MUL),
             },
             DecodeInterstage { .inst = dt.inst,
                                .inst_addr = dt.inst_addr,
                                .next_inst_addr = dt.next_inst_addr,
                                .predicted_next_inst_addr = dt.predicted_next_inst_addr,
                                .val_rs = val_rs,
                                .val_rs_orig = val_rs,
                                .val_rt = val_rt,
                                .val_rt_orig = val_rt,
                                .immediate_val = immediate_val,
                                .csr_read_val = csr_read_val,
                                .csr_address = csr_address,
                                .excause = excause,
                                .ff_rs = FORWARD_NONE,
                                .ff_rt = FORWARD_NONE,
                                .alu_component = (flags & IMF_AMO) ? AluComponent::PASS :
                                                 (flags & IMF_MUL) ? AluComponent::MUL : AluComponent::ALU,
                                .aluop = alu_op,
                                .memctl = mem_ctl,
                                .num_rs = num_rs,
                                .num_rt = num_rt,
                                .num_rd = num_rd,
                                .memread = bool(flags & IMF_MEMREAD),
                                .memwrite = bool(flags & IMF_MEMWRITE),
                                .alusrc = bool(flags & IMF_ALUSRC),
                                .regwrite = regwrite,
                                .alu_req_rs = bool(flags & IMF_ALU_REQ_RS),
                                .alu_req_rt = bool(flags & IMF_ALU_REQ_RT),
                                .branch_bxx = bool(flags & IMF_BRANCH),
                                .branch_jal = bool(flags & IMF_JUMP),
                                .branch_val = bool(flags & IMF_BJ_NOT),
                                .branch_jalr = bool(flags & IMF_BRANCH_JALR),
                                .stall = false,
                                .is_valid = dt.is_valid,
                                .w_operation = w_operation,
                                .alu_mod = bool(flags & IMF_ALU_MOD),
                                .alu_pc = bool(flags & IMF_PC_TO_ALU),
                                .csr = bool(flags & IMF_CSR),
                                .csr_to_alu = bool(flags & IMF_CSR_TO_ALU),
                                .csr_write = csr_write,
                                .xret = bool(flags & IMF_XRET),
                                .insert_stall_before = bool(flags & IMF_CSR) } };
}

ExecuteState Core::execute(const DecodeInterstage &dt) {
    enum ExceptionCause excause = dt.excause;
    // TODO refactor to produce multiplexor index and multiplex function
    const RegisterValue alu_fst = [=] {
        if (dt.alu_pc) return RegisterValue(dt.inst_addr.get_raw());
        return dt.val_rs;
    }();
    const RegisterValue alu_sec = [=] {
        if (dt.csr_to_alu) return dt.csr_read_val;
        if (dt.alusrc) return dt.immediate_val;
        return dt.val_rt;
    }();
    const RegisterValue alu_val = [=] {
        if (excause != EXCAUSE_NONE) return RegisterValue(0);
        return alu_combined_operate(dt.aluop, dt.alu_component, dt.w_operation, dt.alu_mod, alu_fst, alu_sec);
    }();
    const Address branch_jal_target = dt.inst_addr + dt.immediate_val.as_i64();

    const unsigned stall_status = [=] {
        if (dt.stall) return 1;
        if (dt.ff_rs != FORWARD_NONE || dt.ff_rt != FORWARD_NONE) return 2;
        return 0;
    }();

    return { ExecuteInternalState {
                 .alu_src1 = dt.val_rs,
                 .alu_src2 = alu_sec,
                 .immediate = dt.immediate_val,
                 .rs = dt.val_rs_orig,
                 .rt = dt.val_rt_orig,
                 .stall_status = stall_status,
                 .alu_op_num = static_cast<unsigned>(dt.aluop.alu_op),
                 .forward_from_rs1_num = static_cast<unsigned>(dt.ff_rs),
                 .forward_from_rs2_num = static_cast<unsigned>(dt.ff_rt),
                 .excause_num = static_cast<unsigned>(dt.excause),
                 .alu_src = dt.alusrc,
                 .alu_mul = dt.alu_component == AluComponent::MUL,
                 .branch_bxx = dt.branch_bxx,
                 .alu_pc = dt.alu_pc,
             },
             ExecuteInterstage {
                 .inst = dt.inst,
                 .inst_addr = dt.inst_addr,
                 .next_inst_addr = dt.next_inst_addr,
                 .predicted_next_inst_addr = dt.predicted_next_inst_addr,
                 .branch_jal_target = branch_jal_target,
                 .val_rt = dt.val_rt,
                 .alu_val = alu_val,
                 .immediate_val = dt.immediate_val,
                 .csr_read_val = dt.csr_read_val,
                 .csr_address = dt.csr_address,
                 .excause = excause,
                 .memctl = dt.memctl,
                 .num_rd = dt.num_rd,
                 .memread = dt.memread,
                 .memwrite = dt.memwrite,
                 .regwrite = dt.regwrite,
                 .is_valid = dt.is_valid,
                 .branch_bxx = dt.branch_bxx,
                 .branch_jal = dt.branch_jal,
                 .branch_val = dt.branch_val,
                 .branch_jalr = dt.branch_jalr,
                 .alu_zero = alu_val == 0,
                 .csr = dt.csr,
                 .csr_write = dt.csr_write,
                 .xret = dt.xret,
             } };
}

MemoryState Core::memory(const ExecuteInterstage &dt) {
    RegisterValue towrite_val = dt.alu_val;
    Address mem_addr = Address(get_xlen_from_reg(dt.alu_val));
    bool memread = dt.memread;
    bool memwrite = dt.memwrite;
    bool regwrite = dt.regwrite;
    Address computed_next_inst_addr;

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

    // Conditional branch (BXX = BEQ | BNE...) is executed and should be taken.
    const bool branch_bxx_taken = dt.branch_bxx && (!dt.branch_val ^ !dt.alu_zero);
    // Unconditional jump should be taken (JALX = JAL | JALR).
    const bool branch_jalx = dt.branch_jalr || dt.branch_jal;

    computed_next_inst_addr = compute_next_inst_addr(dt, branch_bxx_taken);

    bool csr_written = false;
    if (control_state != nullptr && dt.is_valid && dt.excause == EXCAUSE_NONE) {
        control_state->increment_internal(CSR::Id::MINSTRET, 1);
        if (dt.csr_write) {
            control_state->write(dt.csr_address, dt.alu_val);
            csr_written = true;
        }
        if (dt.xret) {
            control_state->exception_return(CSR::PrivilegeLevel::MACHINE);
            if (this->xlen == Xlen::_32)
                computed_next_inst_addr = Address(control_state->read_internal(CSR::Id::MEPC).as_u32());
            else
                computed_next_inst_addr = Address(control_state->read_internal(CSR::Id::MEPC).as_u64());
            csr_written = true;
        }
    }

    return { MemoryInternalState {
                 .mem_read_val = towrite_val,
                 .mem_write_val = dt.val_rt,
                 .mem_addr = dt.alu_val,
                 .excause_num = static_cast<unsigned>(excause),
                 .memwrite = memwrite,
                 .memread = memread,
                 .branch_bxx = dt.branch_bxx,
                 .branch_jal = dt.branch_jal,
                 // PC should be modified by branch/jump instruction.
                 .branch_outcome = branch_bxx_taken || branch_jalx,
                 .branch_jalx = branch_jalx,
                 .branch_jalr = dt.branch_jalr,
                 .xret = dt.xret,
             },
             MemoryInterstage {
                 .inst = dt.inst,
                 .inst_addr = dt.inst_addr,
                 .next_inst_addr = dt.next_inst_addr,
                 .predicted_next_inst_addr = dt.predicted_next_inst_addr,
                 .computed_next_inst_addr = computed_next_inst_addr,
                 .mem_addr = mem_addr,
                 .towrite_val = [=]() -> RegisterValue {
                     if (dt.csr) return dt.csr_read_val;
                     if (dt.branch_jalr || dt.branch_jal) return dt.next_inst_addr.get_raw();
                     return towrite_val;
                 }(),
                 .excause = dt.excause,
                 .num_rd = dt.num_rd,
                 .memtoreg = memread,
                 .regwrite = regwrite,
                 .is_valid = dt.is_valid,
                 .csr_written = csr_written,
             } };
}

WritebackState Core::writeback(const MemoryInterstage &dt) {
    if (dt.regwrite) { regs->write_gp(dt.num_rd, dt.towrite_val); }

    return WritebackState { WritebackInternalState {
        .inst = (dt.excause == EXCAUSE_NONE)? dt.inst: Instruction::NOP,
        .inst_addr = dt.inst_addr,
        .value = dt.towrite_val,
        .num_rd = dt.num_rd,
        .regwrite = dt.regwrite,
        .memtoreg = dt.memtoreg,
    } };
}

Address Core::compute_next_inst_addr(const ExecuteInterstage &exec, bool branch_taken) const {
    if (branch_taken || exec.branch_jal) { return exec.branch_jal_target; }
    if (exec.branch_jalr) { return Address(get_xlen_from_reg(exec.alu_val)); }
    return exec.next_inst_addr;
}

uint64_t Core::get_xlen_from_reg(RegisterValue reg) const {
    switch (this->xlen) {
    case Xlen::_32: return reg.as_u32();
    case Xlen::_64: return reg.as_u64();
    default: UNREACHABLE
    }
}

CoreSingle::CoreSingle(Registers *regs,
    Predictor *predictor,
    FrontendMemory *mem_program,
    FrontendMemory *mem_data,
    CSR::ControlState *control_state,
    Xlen xlen, ConfigIsaWord isa_word)
    : Core(regs, predictor, mem_program, mem_data, control_state, xlen, isa_word) {
    reset();
}

void CoreSingle::do_step(bool skip_break) {
    Pipeline &p = state.pipeline;

    p.fetch = fetch(pc_if, skip_break);
    p.decode = decode(p.fetch.final);
    p.execute = execute(p.decode.final);
    p.memory = memory(p.execute.final);
    p.writeback = writeback(p.memory.final);

    regs->write_pc(mem_wb.computed_next_inst_addr);

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
    CSR::ControlState *control_state,
    Xlen xlen,
    ConfigIsaWord isa_word,
    MachineConfig::HazardUnit hazard_unit)
    : Core(regs, predictor, mem_program, mem_data, control_state, xlen, isa_word) {
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
    p.fetch = fetch(pc_if, skip_break);

    bool exception_in_progress = mem_wb.excause != EXCAUSE_NONE;
    if (exception_in_progress) { ex_mem.flush(); }
    exception_in_progress |= ex_mem.excause != EXCAUSE_NONE;
    if (exception_in_progress) { id_ex.flush(); }
    exception_in_progress |= id_ex.excause != EXCAUSE_NONE;
    if (exception_in_progress) { if_id.flush(); }

    bool stall = false;
    if (hazard_unit != MachineConfig::HU_NONE) { stall |= handle_data_hazards(); }

    /* PC and exception pseudo stage
     * ============================== */
    pc_if = {};
    if (mem_wb.excause != EXCAUSE_NONE) {
        /* By default, execution continues with the next instruction after exception. */
        regs->write_pc(mem_wb.computed_next_inst_addr);
        /* Exception handler may override this behavior and change the PC (e.g. hwbreak). */
        handle_exception(
            mem_wb.excause, mem_wb.inst, mem_wb.inst_addr, mem_wb.computed_next_inst_addr,
            jump_branch_pc, mem_wb.mem_addr);
    } else if (detect_mispredicted_jump() || mem_wb.csr_written) {
        /* If the jump was predicted incorrectly or csr register was written, we need to flush the
         * pipeline. */
        flush_and_continue_from_address(mem_wb.computed_next_inst_addr);
    } else if (exception_in_progress) {
        /* An exception is in progress which caused the pipeline before the exception to be flushed.
         * Therefore, next pc cannot be determined from if_id (now NOP).
         * To make the visualization cleaner we stop fetching (and PC update) until the exception
         * is handled. */
        pc_if.stop_if = true;
    } else if (stall || is_stall_requested()) {
        /* Fetch from the same PC is repeated due to stall in the pipeline. */
        handle_stall(saved_if_id);
    } else {
        /* Normal execution. */
        regs->write_pc(if_id.predicted_next_inst_addr);
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
    return mem_wb.computed_next_inst_addr != mem_wb.predicted_next_inst_addr;
}

bool CorePipelined::is_stall_requested() const {
    return id_ex.insert_stall_before && ex_mem.is_valid;
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
        "Exception cause %d instruction PC 0x%08" PRIx64 " next PC 0x%08" PRIx64
        " jump branch PC 0x%08" PRIx64 "registers PC 0x%08" PRIx64 " mem ref 0x%08" PRIx64,
        excause, inst_addr.get_raw(), next_addr.get_raw(), jump_branch_pc.get_raw(),
        regs->read_pc().get_raw(), mem_ref_addr.get_raw());
    return true;
}
