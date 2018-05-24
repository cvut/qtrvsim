#include "core.h"
#include "programloader.h"
#include "utils.h"

using namespace machine;

#define DM_SUPPORTED (1L<<0)
#define DM_MEMWRITE (1L<<1)
#define DM_MEMREAD (1L<<2)
#define DM_ALUSRC (1L<<3)
#define DM_REGD (1L<<4)
#define DM_REGWRITE (1L<<5)

struct DecodeMap {
    long flags;
    enum AluOp alu;
    enum MemoryAccess::AccessControl mem_ctl;
};

#define NOALU .alu = ALU_OP_SLL
#define NOMEM .mem_ctl = MemoryAccess::AC_NONE
#define NOPE { .flags = 0, NOALU, NOMEM }

#define FLAGS_ALU_I (DM_SUPPORTED | DM_ALUSRC | DM_REGWRITE)

// This is map from opcode to signals.
static const struct DecodeMap dmap[]  = {
    { .flags = DM_SUPPORTED | DM_REGD | DM_REGWRITE, NOALU, NOMEM }, // Alu operations (aluop is decoded from function explicitly)
    { .flags = DM_SUPPORTED, NOALU, NOMEM }, // REGIMM (BLTZ, BGEZ)
    { .flags = DM_SUPPORTED, NOALU, NOMEM }, // J
    NOPE, // JAL
    { .flags = DM_SUPPORTED, NOALU, NOMEM }, // BEQ
    { .flags = DM_SUPPORTED, NOALU, NOMEM }, // BNE
    { .flags = DM_SUPPORTED, NOALU, NOMEM }, // BLEZ
    { .flags = DM_SUPPORTED, NOALU, NOMEM }, // BGTZ
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_ADD, NOMEM }, // ADDI
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_ADDU, NOMEM }, // ADDIU
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_SLT, NOMEM }, // SLTI
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_SLTU, NOMEM }, // SLTIU
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_AND, NOMEM }, // ANDI
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_OR, NOMEM }, // ORI
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_XOR, NOMEM }, // XORI
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_LUI, NOMEM}, // LUI
    NOPE, // 16
    NOPE, // 17
    NOPE, // 18
    NOPE, // 19
    NOPE, // 20
    NOPE, // 21
    NOPE, // 22
    NOPE, // 23
    NOPE, // 24
    NOPE, // 25
    NOPE, // 26
    NOPE, // 27
    NOPE, // 28
    NOPE, // 29
    NOPE, // 30
    NOPE, // 31
    { .flags = DM_SUPPORTED | DM_ALUSRC | DM_REGWRITE | DM_MEMREAD, .alu = ALU_OP_ADDU, .mem_ctl = MemoryAccess::AC_BYTE }, // LB
    { .flags = DM_SUPPORTED | DM_ALUSRC | DM_REGWRITE | DM_MEMREAD, .alu = ALU_OP_ADDU, .mem_ctl = MemoryAccess::AC_HALFWORD }, // LH
    NOPE, // LWL
    { .flags = DM_SUPPORTED | DM_ALUSRC | DM_REGWRITE | DM_MEMREAD, .alu = ALU_OP_ADDU, .mem_ctl = MemoryAccess::AC_WORD }, // LW
    { .flags = DM_SUPPORTED | DM_ALUSRC | DM_REGWRITE | DM_MEMREAD, .alu = ALU_OP_ADDU, .mem_ctl = MemoryAccess::AC_BYTE_UNSIGNED }, // LBU
    { .flags = DM_SUPPORTED | DM_ALUSRC | DM_REGWRITE | DM_MEMREAD, .alu = ALU_OP_ADDU, .mem_ctl = MemoryAccess::AC_HALFWORD_UNSIGNED }, // LHU
    NOPE, // LWR
    NOPE, // 39
    { .flags = DM_SUPPORTED | DM_ALUSRC | DM_MEMWRITE, .alu = ALU_OP_ADDU, .mem_ctl = MemoryAccess::AC_BYTE }, // SB
    { .flags = DM_SUPPORTED | DM_ALUSRC | DM_MEMWRITE, .alu = ALU_OP_ADDU, .mem_ctl = MemoryAccess::AC_HALFWORD }, // SH
    NOPE, // SWL
    { .flags = DM_SUPPORTED | DM_ALUSRC | DM_MEMWRITE, .alu = ALU_OP_ADDU, .mem_ctl = MemoryAccess::AC_WORD }, // SW
    NOPE, // 44
    NOPE, // 45
    NOPE, // SWR
    NOPE, // 47
    NOPE, // 48
    NOPE, // 49
    NOPE, // 50
    NOPE, // 51
    NOPE, // 52
    NOPE, // 53
    NOPE, // 54
    NOPE, // 55
    NOPE, // 56
    NOPE, // 57
    NOPE, // 58
    NOPE, // 59
    NOPE, // 60
    NOPE, // 61
    NOPE, // 62
    NOPE // 63
};

Core::Core(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data) {
    cycle_c = 0;
    this->regs = regs;
    this->mem_program = mem_program;
    this->mem_data = mem_data;
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

struct Core::dtFetch Core::fetch() {
    Instruction inst(mem_program->read_word(regs->read_pc()));
    emit instruction_fetched(inst);
    return {
        .inst = inst
    };
}

struct Core::dtDecode Core::decode(const struct dtFetch &dt) {
    emit instruction_decoded(dt.inst);
    const struct DecodeMap &dec = dmap[dt.inst.opcode()];
    if (!(dec.flags & DM_SUPPORTED))
        throw QTMIPS_EXCEPTION(UnsupportedInstruction, "Instruction with following opcode is not supported", QString::number(dt.inst.opcode(), 16));

    std::uint32_t val_rs = regs->read_gp(dt.inst.rs());
    std::uint32_t val_rt = regs->read_gp(dt.inst.rt());

    emit decode_instruction_value(dt.inst.data());
    emit decode_reg1_value(val_rs);
    emit decode_reg2_value(val_rt);
    emit decode_immediate_value(sign_extend(dt.inst.immediate()));

    return {
        .inst = dt.inst,
        .memread = dec.flags & DM_MEMREAD,
        .memwrite = dec.flags & DM_MEMWRITE,
        .alusrc = dec.flags & DM_ALUSRC,
        .regd = dec.flags & DM_REGD,
        .regwrite = dec.flags & DM_REGWRITE,
        .aluop = dt.inst.opcode() == 0 ? (enum AluOp)dt.inst.funct() : dec.alu,
        .memctl = dec.mem_ctl,
        .val_rs = val_rs,
        .val_rt = val_rt,
    };
}

struct Core::dtExecute Core::execute(const struct dtDecode &dt) {
    emit instruction_executed(dt.inst);

    // Handle conditional move (we have to change regwrite signal if conditional is not met)
    bool regwrite = dt.regwrite;
    if (dt.inst.opcode() == 0 && ((dt.inst.funct() == ALU_OP_MOVZ && dt.val_rt != 0) || (dt.inst.funct() == ALU_OP_MOVN && dt.val_rt == 0)))
        regwrite = false;

    std::uint32_t alu_sec = dt.val_rt;
    if (dt.alusrc)
        alu_sec = sign_extend(dt.inst.immediate()); // Sign extend to 32bit

    std::uint32_t alu_val = alu_operate(dt.aluop, dt.val_rs, alu_sec, dt.inst.shamt(), regs);

    emit execute_alu_value(alu_val);
    emit execute_reg1_value(dt.val_rs);
    emit execute_reg2_value(dt.val_rt);
    emit execute_immediate_value(sign_extend(dt.inst.immediate()));

    return {
        .inst = dt.inst,
        .memread = dt.memread,
        .memwrite = dt.memwrite,
        .regwrite = regwrite,
        .memctl = dt.memctl,
        .val_rt = dt.val_rt,
        .rwrite = dt.regd ? dt.inst.rd() : dt.inst.rt(),
        .alu_val = alu_val,
    };
}

struct Core::dtMemory Core::memory(const struct dtExecute &dt) {
    emit instruction_memory(dt.inst);
    std::uint32_t towrite_val = dt.alu_val;

    if (dt.memwrite)
        mem_data->write_ctl(dt.memctl, dt.alu_val, dt.val_rt);
    else if (dt.memread)
        towrite_val = mem_data->read_ctl(dt.memctl, dt.alu_val);

    emit memory_alu_value(dt.alu_val);
    emit memory_rt_value(dt.val_rt);
    emit memory_mem_value(dt.memread ? towrite_val : 0);

    return {
        .inst = dt.inst,
        .regwrite = dt.regwrite,
        .rwrite = dt.rwrite,
        .towrite_val = towrite_val,
    };
}

void Core::writeback(const struct dtMemory &dt) {
    emit instruction_writeback(dt.inst);
    emit writeback_value(dt.towrite_val);
    if (dt.regwrite)
        regs->write_gp(dt.rwrite, dt.towrite_val);
}

void Core::handle_pc(const struct dtDecode &dt) {
    emit instruction_program_counter(dt.inst);

    bool branch = false;
    bool link = false;
    // TODO implement link

    switch (dt.inst.opcode()) {
    case 0: // JR (JALR)
        if (dt.inst.funct() == ALU_OP_JR || dt.inst.funct() == ALU_OP_JALR) {
            regs->pc_abs_jmp(dt.val_rs);
            return;
        }
        break;
    case 1: // REGIMM instruction
        //switch (dt.inst.rt() & 0xF) { // Should be used when linking is supported
        switch (dt.inst.rt()) {
        case 0: // BLTZ(AL)
            branch = (std::int32_t)dt.val_rs < 0;
            break;
        case 1: // BGEZ(AL)
            branch = (std::int32_t)dt.val_rs >= 0;
            break;
        default:
            throw QTMIPS_EXCEPTION(UnsupportedInstruction, "REGIMM instruction with unknown rt code", QString::number(dt.inst.rt(), 16));
        }
        link = dt.inst.rs() & 0x10;
        break;
    case 2: // J
    case 3: // JAL
        regs->pc_abs_jmp_28(dt.inst.address() << 2);
        return;
    case 4: // BEQ
        branch = dt.val_rs == dt.val_rt;
        break;
    case 5: // BNE
        branch = dt.val_rs != dt.val_rt;
        break;
    case 6: // BLEZ
        branch = (std::int32_t)dt.val_rs <= 0;
        break;
    case 7: // BGTZ
        branch = (std::int32_t)dt.val_rs > 0;
        break;
    }

    if (branch)
        regs->pc_jmp((std::int32_t)(((dt.inst.immediate() & 0x8000) ? 0xFFFF0000 : 0) | (dt.inst.immediate() << 2)));
    else
        regs->pc_inc();
}

void Core::dtFetchInit(struct dtFetch &dt) {
    dt.inst = Instruction(0x00);
}

void Core::dtDecodeInit(struct dtDecode &dt) {
    dt.inst = Instruction(0x00);
    dt.memread = false;
    dt.memwrite = false;
    dt.alusrc = false;
    dt.regd = false;
    dt.regwrite = false;
    dt.aluop = ALU_OP_SLL;
    dt.val_rs = 0;
    dt.val_rt = 0;
}

void Core::dtExecuteInit(struct dtExecute &dt) {
    dt.inst = Instruction(0x00);
    dt.memread = false;
    dt.memwrite = false;
    dt.regwrite = false;
    dt.memctl = MemoryAccess::AC_NONE;
    dt.val_rt = 0;
    dt.rwrite = false;
    dt.alu_val = 0;
}

void Core::dtMemoryInit(struct dtMemory &dt) {
    dt.inst = Instruction(0x00);
    dt.regwrite = false;
    dt.rwrite = false;
    dt.towrite_val = 0;
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
}

void CoreSingle::do_reset() {
    if (jmp_delay_decode != nullptr)
        Core::dtDecodeInit(*jmp_delay_decode);
}

CorePipelined::CorePipelined(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data, enum MachineConfig::HazardUnit hazard_unit) : \
    Core(regs, mem_program, mem_data) {
    this->hazard_unit = hazard_unit;
    reset();
}

void CorePipelined::do_step() {
    // Process stages
    writeback(dt_m);
    dt_m = memory(dt_e);
    dt_e = execute(dt_d);
    dt_d = decode(dt_f);

    // TODO signals
    bool stall = false;
    if (hazard_unit != MachineConfig::HU_NONE) {
        // Note: We make exception with $0 as that has no effect when written and is used in nop instruction

#define HAZARD(STAGE) ( \
        (STAGE).regwrite && (STAGE).rwrite != 0 && \
        ((STAGE).rwrite == dt_d.inst.rs() || ( \
            ((STAGE).inst.type() == Instruction::T_R || (STAGE).inst.is_store()) && \
            (STAGE).rwrite == dt_d.inst.rt()) \
        )) // Note: We make exception with $0 as that has no effect and is used in nop instruction

        if (HAZARD(dt_e)) {
            // Hazard with instruction in execute stage
            if (hazard_unit == MachineConfig::HU_STALL_FORWARD) {
                if (dt_e.memread) // TODO extend by branch instructions
                    stall = true;
                else {
                    // Forward result value
                    if (dt_e.rwrite == dt_d.inst.rs())
                        dt_d.val_rs = dt_e.alu_val;
                    if (dt_e.rwrite == dt_d.inst.rt())
                        dt_d.val_rt = dt_e.alu_val;
                }
            } else
                stall = true;
        }
        if (HAZARD(dt_m)) {
            // Hazard with instruction in memory stage
            if (hazard_unit == MachineConfig::HU_STALL_FORWARD) {
                // Forward result value
                if (dt_m.rwrite == dt_d.inst.rs())
                    dt_d.val_rs = dt_m.towrite_val;
                if (dt_m.rwrite == dt_d.inst.rt())
                    dt_d.val_rt = dt_m.towrite_val;
            } else
                stall = true;
        }
        // Write back stage combinatoricly propagates written instruction to decode stage so nothing has to be done for that stage
#undef HAZARD
    }

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
    dtDecodeInit(dt_d);
    dtExecuteInit(dt_e);
    dtMemoryInit(dt_m);
}
