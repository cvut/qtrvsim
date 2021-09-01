#include "core.h"

#include "execute/alu.h"
#include "programloader.h"
#include "utils.h"

using namespace machine;

constexpr uint32_t NOP_HEX = 0x00000013;

Core::Core(
    Registers *regs,
    Predictor *predictor,
    FrontendMemory *mem_program,
    FrontendMemory *mem_data,
    unsigned int min_cache_row_size,
    Cop0State *cop0state)
    : ex_handlers() {
    this->regs = regs;
    this->cop0state = cop0state;
    this->predictor = predictor;
    this->mem_program = mem_program;
    this->mem_data = mem_data;
    this->ex_default_handler = new StopExceptionHandler();
    this->state.min_cache_row_size = min_cache_row_size;
    this->state.hwr_userlocal = 0xe0000000;
    if (cop0state != nullptr) { cop0state->setup_core(this); }
    for (int i = 0; i < EXCAUSE_COUNT; i++) {
        state.stop_on_exception[i] = true;
        state.step_over_exception[i] = true;
    }
    state.step_over_exception[EXCAUSE_INT] = false;
}

Core::~Core() {
    delete ex_default_handler;
}

void Core::step(bool skip_break) {
    state.cycle_count++;
    emit cycle_c_value(state.cycle_count);
    do_step(skip_break);
    emit step_done();
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

Registers *Core::get_regs() {
    return regs;
}

Cop0State *Core::get_cop0state() {
    return cop0state;
}

FrontendMemory *Core::get_mem_data() {
    return mem_data;
}

FrontendMemory *Core::get_mem_program() {
    return mem_program;
}

void Core::insert_hwbreak(Address address) {
    state.hw_breaks.insert(address, new hwBreak(address));
}

void Core::remove_hwbreak(Address address) {
    hwBreak *hwbrk = state.hw_breaks.take(address);
    delete hwbrk;
}

bool Core::is_hwbreak(Address address) {
    hwBreak *hwbrk = state.hw_breaks.value(address);
    return hwbrk != nullptr;
}

void Core::set_stop_on_exception(enum ExceptionCause excause, bool value) {
    state.stop_on_exception[excause] = value;
}

bool Core::get_stop_on_exception(enum ExceptionCause excause) const {
    return state.stop_on_exception[excause];
}

void Core::set_step_over_exception(enum ExceptionCause excause, bool value) {
    state.step_over_exception[excause] = value;
}

bool Core::get_step_over_exception(enum ExceptionCause excause) const {
    return state.step_over_exception[excause];
}

void Core::register_exception_handler(ExceptionCause excause, ExceptionHandler *exhandler) {
    if (excause == EXCAUSE_NONE) {
        delete ex_default_handler;
        ex_default_handler = exhandler;
    } else {
        ExceptionHandler *old = ex_handlers.take(excause);
        delete old;
        ex_handlers.insert(excause, exhandler);
    }
}

bool Core::handle_exception(
    Core *core,
    Registers *regs,
    ExceptionCause excause,
    Instruction inst,
    Address inst_addr,
    Address next_addr,
    Address jump_branch_pc,
    Address mem_ref_addr) {
    bool ret = false;
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

    ExceptionHandler *exhandler = ex_handlers.value(excause);
    if (exhandler != nullptr) {
        ret = exhandler->handle_exception(
            core, regs, excause, inst_addr, next_addr, jump_branch_pc, mem_ref_addr);
    } else if (ex_default_handler != nullptr) {
        ret = ex_default_handler->handle_exception(
            core, regs, excause, inst_addr, next_addr, jump_branch_pc, mem_ref_addr);
    }
    if (get_stop_on_exception(excause)) { emit core->stop_on_exception_reached(); }

    return ret;
}

void Core::set_c0_userlocal(uint32_t address) {
    state.hwr_userlocal = address;
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
    UNUSED(mode)

    uint32_t mask;
    uint32_t shift;
    uint32_t temp;

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
    case AC_WORD_RIGHT:
        if (memwrite) {
            shift = (3u - (mem_addr.get_raw() & 3u)) << 3;
            mask = 0xffffffff << shift;
            temp = mem_data->read_u32(mem_addr & ~3u);
            temp = (temp & ~mask) | (rt_value.as_u32() << shift);
            mem_data->write_u32(mem_addr & ~3u, temp);
        } else {
            shift = (3u - (mem_addr.get_raw() & 3u)) << 3;
            mask = 0xffffffff >> shift;
            towrite_val = mem_data->read_u32(mem_addr & ~3u);
            towrite_val = (towrite_val.as_u32() >> shift) | (rt_value.as_u32() & ~mask);
        }
        break;
    case AC_WORD_LEFT:
        if (memwrite) {
            shift = (mem_addr.get_raw() & 3u) << 3;
            mask = 0xffffffff >> shift;
            temp = mem_data->read_u32(mem_addr & ~3);
            temp = (temp & ~mask) | (rt_value.as_u32() >> shift);
            mem_data->write_u32(mem_addr & ~3, temp);
        } else {
            shift = (mem_addr.get_raw() & 3u) << 3;
            mask = 0xffffffff << shift;
            towrite_val = mem_data->read_u32(mem_addr & ~3);
            towrite_val = (towrite_val.as_u32() << shift) | (rt_value.as_u32() & ~mask);
        }
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
        hwBreak *brk = state.hw_breaks.value(inst_addr);
        if (brk != nullptr) { excause = EXCAUSE_HWBREAK; }
    }
    if (cop0state != nullptr && excause == EXCAUSE_NONE) {
        if (cop0state->core_interrupt_request()) { excause = EXCAUSE_INT; }
    }

    regs->write_pc(this->predictor->predict(inst, inst_addr));

    emit fetch_inst_addr_value(inst_addr);
    emit instruction_fetched(inst, inst_addr, excause, true);
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

    emit decode_inst_addr_value(dt.inst_addr);
    emit instruction_decoded(dt.inst, dt.inst_addr, excause, dt.is_valid);
    emit decode_instruction_value(dt.inst.data());
    emit decode_reg1_value(val_rs.as_u32());
    emit decode_reg2_value(val_rt.as_u32());
    emit decode_immediate_value(immediate_val);
    emit decode_regw_value(regwrite);
    emit decode_memtoreg_value(bool(flags & IMF_MEMREAD));
    emit decode_memwrite_value(bool(flags & IMF_MEMWRITE));
    emit decode_memread_value(bool(flags & IMF_MEMREAD));
    emit decode_alusrc_value(bool(flags & IMF_ALUSRC));
    emit decode_rs_num_value(num_rs);
    emit decode_rt_num_value(num_rt);
    emit decode_rd_num_value(num_rd);

    return { DecodeInternalState {
                 .alu_op_num = static_cast<unsigned>(alu_op),
                 .excause_num = static_cast<unsigned>(excause),
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
                                .stop_if = bool(flags & IMF_STOP_IF),
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

    emit execute_inst_addr_value(dt.inst_addr);
    emit instruction_executed(dt.inst, dt.inst_addr, excause, dt.is_valid);
    emit execute_alu_value(alu_val.as_u32());
    emit execute_reg1_value(dt.val_rs.as_u32());
    emit execute_reg2_value(dt.val_rt.as_u32());
    emit execute_reg1_ff_value(dt.ff_rs);
    emit execute_reg2_ff_value(dt.ff_rt);
    //    emit execute_immediate_value(dt.immediate_val);
    emit execute_regw_value(dt.regwrite);
    emit execute_memtoreg_value(dt.memread);
    emit execute_memread_value(dt.memread);
    emit execute_memwrite_value(dt.memwrite);
    emit execute_alusrc_value(dt.alusrc);
    emit execute_rd_num_value(dt.num_rd);

    const unsigned stall_status = [&]() {
        if (dt.stall) {
            return 1;
        } else if (dt.ff_rs != FORWARD_NONE || dt.ff_rt != FORWARD_NONE) {
            return 2;
        } else {
            return 0;
        }
    }();
    emit execute_stall_forward_value(stall_status);

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
                 .stop_if = dt.stop_if,
                 .is_valid = dt.is_valid,
                 .branch = dt.branch,
                 // In the diagram `branch_taken` is computed in memory/handle_pc, but here it can
                 // be done on one place only.
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
    Address mem_addr = Address(dt.alu_val.as_u32());
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

    emit memory_inst_addr_value(dt.inst_addr);
    emit instruction_memory(dt.inst, dt.inst_addr, dt.excause, dt.is_valid);
    emit memory_alu_value(dt.alu_val.as_u32());
    emit memory_rt_value(dt.val_rt.as_u32());
    emit memory_mem_value(memread ? towrite_val.as_u32() : 0);
    emit memory_regw_value(regwrite);
    emit memory_memtoreg_value(dt.memread);
    emit memory_memread_value(dt.memread);
    emit memory_memwrite_value(memwrite);
    emit memory_regw_num_value(dt.num_rd);
    emit memory_excause_value(excause);

    return { MemoryInternalState {
                 .memwrite = dt.memwrite,
                 .memread = dt.memread,
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
                 .excause = dt.excause,
                 .stop_if = dt.stop_if,
                 .is_valid = dt.is_valid,
             } };
}

WritebackState Core::writeback(const MemoryInterstage &dt) {
    emit writeback_inst_addr_value(dt.inst_addr);
    emit instruction_writeback(dt.inst, dt.inst_addr, dt.excause, dt.is_valid);
    emit writeback_value(dt.towrite_val.as_u32());
    emit writeback_memtoreg_value(dt.memtoreg);
    emit writeback_regw_value(dt.regwrite);
    emit writeback_regw_num_value(dt.num_rd);
    if (dt.regwrite) { regs->write_gp(dt.num_rd, dt.towrite_val); }

    return { WritebackInternalState {
        .inst = dt.inst,
        .inst_addr = dt.inst_addr,
        .regwrite = dt.regwrite,
        .num_rd = dt.num_rd,
        .value = dt.towrite_val,
    } };
}

Address Core::handle_pc(const ExecuteInterstage &dt) {
    emit instruction_program_counter(dt.inst, dt.inst_addr, EXCAUSE_NONE, dt.is_valid);

    if (dt.jump) { return Address(dt.alu_val.as_u32()); } // TODO handle XLEN
    if (dt.branch_taken) { return dt.branch_target; }
    return dt.inst_addr + 4;
}

void Core::flush() {
    dtExecuteInit(state.pipeline.execute.final);
    emit instruction_executed(
        state.pipeline.execute.final.inst, state.pipeline.execute.final.inst_addr,
        state.pipeline.execute.final.excause, state.pipeline.execute.final.is_valid);
    emit execute_inst_addr_value(STAGEADDR_NONE);
    dtDecodeInit(state.pipeline.decode.final);
    emit instruction_decoded(
        state.pipeline.decode.final.inst, state.pipeline.decode.final.inst_addr,
        state.pipeline.decode.final.excause, state.pipeline.decode.final.is_valid);
    emit decode_inst_addr_value(STAGEADDR_NONE);
    dtFetchInit(state.pipeline.fetch.final);
    emit instruction_fetched(
        state.pipeline.fetch.final.inst, state.pipeline.fetch.final.inst_addr,
        state.pipeline.fetch.final.excause, state.pipeline.fetch.final.is_valid);
    emit fetch_inst_addr_value(STAGEADDR_NONE);
}

void Core::dtFetchInit(FetchInterstage &dt) {
    dt.inst_addr = Address(0);
    dt.inst = Instruction(NOP_HEX);
    dt.excause = EXCAUSE_NONE;
    dt.is_valid = false;
}

void Core::dtDecodeInit(DecodeInterstage &dt) {
    dt.inst_addr = Address(0);
    dt.inst = Instruction(NOP_HEX);
    dt.memread = false;
    dt.memwrite = false;
    dt.alusrc = false;
    dt.regwrite = false;
    dt.jump = false;
    dt.bj_not = false;
    // dt.aluop = ALU_OP_SLL;
    dt.aluop = AluOp::ADD;
    dt.memctl = AC_NONE;
    dt.num_rs = 0;
    dt.num_rt = 0;
    dt.num_rd = 0;
    dt.val_rs = 0;
    dt.val_rt = 0;
    dt.immediate_val = 0;
    dt.ff_rs = FORWARD_NONE;
    dt.ff_rt = FORWARD_NONE;
    dt.excause = EXCAUSE_NONE;
    dt.stall = false;
    dt.stop_if = false;
    dt.is_valid = false;
}

void Core::dtExecuteInit(ExecuteInterstage &dt) {
    dt.inst_addr = Address(0);
    dt.inst = Instruction(NOP_HEX);
    dt.memread = false;
    dt.memwrite = false;
    dt.regwrite = false;
    dt.memctl = AC_NONE;
    dt.val_rt = 0;
    dt.num_rd = 0;
    dt.alu_val = 0;
    dt.excause = EXCAUSE_NONE;
    dt.stop_if = false;
    dt.is_valid = false;
}

void Core::dtMemoryInit(MemoryInterstage &dt) {
    dt.inst_addr = Address(0);
    dt.inst = Instruction(NOP_HEX);
    dt.memtoreg = false;
    dt.regwrite = false;
    dt.num_rd = false;
    dt.towrite_val = 0;
    dt.mem_addr = 0x0_addr;
    dt.excause = EXCAUSE_NONE;
    dt.stop_if = false;
    dt.is_valid = false;
}

CoreSingle::CoreSingle(
    Registers *regs,
    Predictor *predictor,
    FrontendMemory *mem_program,
    FrontendMemory *mem_data,
    unsigned int min_cache_row_size,
    Cop0State *cop0state)
    : Core(regs, predictor, mem_program, mem_data, min_cache_row_size, cop0state) {
    reset();
}

void CoreSingle::do_step(bool skip_break) {
    state.pipeline.fetch = fetch(skip_break);
    state.pipeline.decode = decode(state.pipeline.fetch.final);
    state.pipeline.execute = execute(state.pipeline.decode.final);
    state.pipeline.memory = memory(state.pipeline.execute.final);
    state.pipeline.writeback = writeback(state.pipeline.memory.final);

    regs->write_pc(handle_pc(state.pipeline.execute.final));

    if (state.pipeline.memory.final.excause != EXCAUSE_NONE) {
        handle_exception(
            this, regs, state.pipeline.memory.final.excause, state.pipeline.memory.final.inst,
            state.pipeline.memory.final.inst_addr, regs->read_pc(), prev_inst_addr,
            state.pipeline.memory.final.mem_addr);
        return;
    }
    prev_inst_addr = state.pipeline.memory.final.inst_addr;
}

void CoreSingle::do_reset() {
    prev_inst_addr = Address::null();
}

CorePipelined::CorePipelined(
    Registers *regs,
    Predictor *predictor,
    FrontendMemory *mem_program,
    FrontendMemory *mem_data,
    enum MachineConfig::HazardUnit hazard_unit,
    unsigned int min_cache_row_size,
    Cop0State *cop0state)
    : Core(regs, predictor, mem_program, mem_data, min_cache_row_size, cop0state) {
    this->hazard_unit = hazard_unit;

    reset();
}

void CorePipelined::do_step(bool skip_break) {
    bool stall = false;
    Address jump_branch_pc = state.pipeline.memory.final.inst_addr;

    // Process stages
    state.pipeline.writeback = writeback(state.pipeline.memory.final);
    state.pipeline.memory = memory(state.pipeline.execute.final);
    state.pipeline.execute = execute(state.pipeline.decode.final);
    state.pipeline.decode = decode(state.pipeline.fetch.final);

    if (state.pipeline.memory.final.excause != EXCAUSE_NONE) {
        regs->write_pc(state.pipeline.execute.final.inst_addr);
        flush();
        handle_exception(
            this, regs, state.pipeline.memory.final.excause, state.pipeline.memory.final.inst,
            state.pipeline.memory.final.inst_addr, state.pipeline.execute.final.inst_addr,
            jump_branch_pc, state.pipeline.memory.final.mem_addr);
    }

    state.pipeline.decode.final.ff_rs = FORWARD_NONE;
    state.pipeline.decode.final.ff_rt = FORWARD_NONE;

    if (hazard_unit != MachineConfig::HU_NONE) {
        // Note: We make exception with $0 as that has no effect when
        // written and is used in nop instruction

#define HAZARD(STAGE)                                                                              \
    ((STAGE).final.regwrite && (STAGE).final.num_rd != 0                                           \
     && ((state.pipeline.decode.final.alu_req_rs                                                   \
          && (STAGE).final.num_rd == state.pipeline.decode.final.num_rs)                           \
         || (state.pipeline.decode.final.alu_req_rt                                                \
             && (STAGE).final.num_rd == state.pipeline.decode.final.num_rt))) //
        // Note: We make exception with $0 as that has no effect and is used in nop instruction

        // Write back internal combinatoricly propagates written instruction to decode internal
        // so nothing has to be done for that internal
        if (HAZARD(state.pipeline.memory)) {
            // Hazard with instruction in memory internal
            if (hazard_unit == MachineConfig::HU_STALL_FORWARD) {
                // Forward result value
                if (state.pipeline.decode.final.alu_req_rs
                    && state.pipeline.memory.final.num_rd == state.pipeline.decode.final.num_rs) {
                    state.pipeline.decode.final.val_rs = state.pipeline.memory.final.towrite_val;
                    state.pipeline.decode.final.ff_rs = FORWARD_FROM_W;
                }
                if (state.pipeline.decode.final.alu_req_rt
                    && state.pipeline.memory.final.num_rd == state.pipeline.decode.final.num_rt) {
                    state.pipeline.decode.final.val_rt = state.pipeline.memory.final.towrite_val;
                    state.pipeline.decode.final.ff_rt = FORWARD_FROM_W;
                }
            } else {
                stall = true;
            }
        }
        if (HAZARD(state.pipeline.execute)) {
            // Hazard with instruction in execute internal
            if (hazard_unit == MachineConfig::HU_STALL_FORWARD) {
                if (state.pipeline.execute.final.memread) {
                    stall = true;
                } else {
                    // Forward result value
                    if (state.pipeline.decode.final.alu_req_rs
                        && state.pipeline.execute.final.num_rd
                               == state.pipeline.decode.final.num_rs) {
                        state.pipeline.decode.final.val_rs = state.pipeline.execute.final.alu_val;
                        state.pipeline.decode.final.ff_rs = FORWARD_FROM_M;
                    }
                    if (state.pipeline.decode.final.alu_req_rt
                        && state.pipeline.execute.final.num_rd
                               == state.pipeline.decode.final.num_rt) {
                        state.pipeline.decode.final.val_rt = state.pipeline.execute.final.alu_val;
                        state.pipeline.decode.final.ff_rt = FORWARD_FROM_M;
                    }
                }
            } else {
                stall = true;
            }
        }
#undef HAZARD
    }
    if (state.pipeline.execute.final.stop_if || state.pipeline.memory.final.stop_if) {
        stall = true;
    }

    emit hu_stall_value(stall);

    // Now process program counter (loop connections from decode internal)
    if (!stall && !state.pipeline.decode.final.stop_if) {
        state.pipeline.decode.final.stall = false;
        state.pipeline.fetch = fetch(skip_break);
        // figure out stalling...
        Address real_addr = handle_pc(state.pipeline.execute.final);
        if (state.pipeline.execute.final.is_valid
            && real_addr != state.pipeline.decode.final.inst_addr) {
            regs->write_pc(real_addr);
            dtDecodeInit(state.pipeline.decode.final);
            dtFetchInit(state.pipeline.fetch.final);
        }
    } else {
        // Run fetch internal on empty
        fetch(skip_break);
        // clear decode latch (insert nope to execute internal)
        if (!state.pipeline.decode.final.stop_if) {
            dtDecodeInit(state.pipeline.decode.final);
            state.pipeline.decode.final.stall = true;
        } else {
            dtFetchInit(state.pipeline.fetch.final);
        }
        // emit instruction_decoded(state.pipeline.decode.inst,
        // state.pipeline.decode.inst_addr, state.pipeline.decode.excause,
        // state.pipeline.decode.is_valid);
    }
    if (stall || state.pipeline.decode.final.stop_if) {
        state.stall_count++;
        emit stall_c_value(state.stall_count);
    }
}

void CorePipelined::do_reset() {
    dtFetchInit(state.pipeline.fetch.final);
    dtFetchInit(state.pipeline.fetch.result);
    state.pipeline.fetch.final.inst_addr = 0x0_addr;
    state.pipeline.fetch.result.inst_addr = 0x0_addr;
    dtDecodeInit(state.pipeline.decode.result);
    dtDecodeInit(state.pipeline.decode.final);
    state.pipeline.decode.result.inst_addr = 0x0_addr;
    state.pipeline.decode.final.inst_addr = 0x0_addr;
    dtExecuteInit(state.pipeline.execute.result);
    dtExecuteInit(state.pipeline.execute.final);
    state.pipeline.execute.result.inst_addr = 0x0_addr;
    state.pipeline.execute.final.inst_addr = 0x0_addr;
    dtMemoryInit(state.pipeline.memory.result);
    dtMemoryInit(state.pipeline.memory.final);
    state.pipeline.memory.result.inst_addr = 0x0_addr;
    state.pipeline.memory.final.inst_addr = 0x0_addr;
}

bool StopExceptionHandler::handle_exception(
    Core *core,
    Registers *regs,
    ExceptionCause excause,
    Address inst_addr,
    Address next_addr,
    Address jump_branch_pc,
    Address mem_ref_addr) {
#if 0
    printf("Exception cause %d instruction PC 0x%08lx next PC 0x%08lx jump branch PC 0x%08lx "
           "in_delay_slot %d registers PC 0x%08lx mem ref 0x%08lx\n",
           excause, (unsigned long)inst_addr, (unsigned long)next_addr,
           (unsigned long)jump_branch_pc, (int)in_delay_slot,
           (unsigned long)regs->read_pc(), (unsigned long)mem_ref_addr);
#else
    (void)excause;
    (void)inst_addr;
    (void)next_addr;
    (void)mem_ref_addr;
    (void)regs;
    (void)jump_branch_pc;
    (void)core;
#endif
    return true;
}
