#include "core.h"
#include "programloader.h"

#define DM_SUPPORTED (1L<<0)
#define DM_MEM2REG (1L<<1)
#define DM_MEMWRITE (1L<<2)
#define DM_ALUSRC (1L<<3)
#define DM_REGD (1L<<4)
#define DM_REGWRITE (1L<<5)

 struct DecodeMap {
    long flags;
    enum AluOp alu;
};


// This is temporally operation place holder
#define NOALU .alu = ALU_OP_SLL
#define NOPE { .flags = 0, NOALU }

#define FLAGS_ALU_I (DM_SUPPORTED | DM_ALUSRC | DM_REGWRITE)

// This is map from opcode to signals.
static const struct DecodeMap dmap[]  = {
    { .flags = DM_SUPPORTED | DM_REGD | DM_REGWRITE, NOALU }, // Alu operations (aluop is decoded from function explicitly)
    { .flags = DM_SUPPORTED, NOALU }, // REGIMM (BLTZ, BGEZ, )
    { .flags = DM_SUPPORTED, NOALU }, // J
    NOPE, // JAL
    { .flags = DM_SUPPORTED, NOALU }, // BEQ
    { .flags = DM_SUPPORTED, NOALU }, // BNE
    { .flags = DM_SUPPORTED, NOALU }, // BLEZ
    { .flags = DM_SUPPORTED, NOALU }, // BGTZ
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_ADD }, // ADDI
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_ADDU }, // ADDIU
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_SLT }, // SLTI
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_SLTU }, // SLTIU
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_AND }, // ANDI
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_OR }, // ORI
    { .flags = FLAGS_ALU_I, .alu = ALU_OP_XOR }, // XORI
    NOPE, // LUI
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
    NOPE, // LB
    NOPE, // LH
    NOPE, // LWL
    NOPE, // LW
    NOPE, // LBU
    NOPE, // LHU
    NOPE, // LWR
    NOPE, // 39
    NOPE, // SB
    NOPE, // SH
    NOPE, // SWL
    NOPE, // SW
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

Core::Core(Registers *regs, MemoryAccess *mem) {
    this->regs = regs;
    this->mem = mem;
}

struct Core::dtFetch Core::fetch() {
    // TODO signals
    Instruction inst(mem->read_word(regs->read_pc()));
    return {
        .inst = inst
    };
}

struct Core::dtDecode Core::decode(struct dtFetch dt) {
    struct DecodeMap dec = dmap[dt.inst.opcode()];
    if (!(dec.flags & DM_SUPPORTED))
        // TODO message
        throw QTMIPS_EXCEPTION(UnsupportedInstruction, "", "");

    // TODO solve forwarding somehow in here
    std::uint32_t rs = regs->read_gp(dt.inst.rs());
    std::uint32_t rt = regs->read_gp(dt.inst.rt());

    return {
        .inst = dt.inst,
        .mem2reg = dec.flags & DM_MEM2REG,
        .memwrite = dec.flags & DM_MEMWRITE,
        .alusrc = dec.flags & DM_ALUSRC,
        .regd = dec.flags & DM_REGD,
        .regwrite = dec.flags & DM_REGWRITE,
        .aluop = dt.inst.opcode() == 0 ? (enum AluOp)dt.inst.funct() : dec.alu,
        .val_rs = rs,
        .val_rt = rt,
    };
    // TODO on jump there should be delay slot. Does processor addes it or compiler. And do we care?
}

struct Core::dtExecute Core::execute(struct dtDecode dt) {
    // TODO signals

    // Handle conditional move (we have to change regwrite signal if conditional is not met)
    // TODO can't we do this some cleaner way?
    bool regwrite = dt.regwrite;
    if (dt.inst.opcode() == 0 && ((dt.inst.funct() == 10 && dt.val_rt != 0) || (dt.inst.funct() == 11 && dt.val_rt == 0)))
        regwrite = false;

    return {
        .regwrite = regwrite,
        .rwrite = dt.regd ? dt.inst.rd() : dt.inst.rt(),
        .alu_val = alu_operate(dt.aluop, dt.val_rs, dt.alusrc ? dt.inst.immediate() : dt.val_rt, dt.inst.shamt(), regs),
    };
}

struct Core::dtMemory Core::memory(struct dtExecute dt) {
    // TODO signals
    return {
        .regwrite = dt.regwrite,
        .rwrite = dt.rwrite,
        .alu_val = dt.alu_val,
    };
}

void Core::writeback(struct dtMemory dt) {
    if (dt.regwrite) {
        regs->write_gp(dt.rwrite, dt.alu_val);
    }
}

void Core::handle_pc(struct dtDecode dt) {
    // TODO signals
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
        regs->pc_jmp((std::int32_t)(((dt.inst.immediate() & 0x7fff) << 2) | ((dt.inst.immediate() & 0x8000) << 16)));
    else
        regs->pc_inc();
}

void Core::dtFetchInit(struct dtFetch &dt) {
    dt.inst = Instruction(0x00);
}

void Core::dtDecodeInit(struct dtDecode &dt) {
    dt.inst = Instruction(0x00);
    dt.mem2reg = false;
    dt.memwrite = false;
    dt.alusrc = false;
    dt.regd = false;
    dt.regwrite = false;
    dt.aluop = ALU_OP_SLL;
    dt.val_rs = 0;
    dt.val_rt = 0;
}

void Core::dtExecuteInit(struct dtExecute &dt) {
    dt.regwrite = false;
    dt.rwrite = false;
    dt.alu_val = 0;
}

void Core::dtMemoryInit(struct dtMemory &dt) {
    dt.regwrite = false;
    dt.rwrite = false;
    dt.alu_val = 0;
}

CoreSingle::CoreSingle(Registers *regs, MemoryAccess *mem) : \
    Core(regs, mem) {
    dtDecodeInit(jmp_delay_decode);
}

void CoreSingle::step() {
    struct dtFetch f = fetch();
    struct dtDecode d = decode(f);
    struct dtExecute e = execute(d);
    struct dtMemory m = memory(e);
    writeback(m);
    handle_pc(jmp_delay_decode);
    jmp_delay_decode = d; // Copy current decode
}

CorePipelined::CorePipelined(Registers *regs, MemoryAccess *mem) : \
    Core(regs, mem) {
    dtFetchInit(dt_f);
    dtDecodeInit(dt_d);
    dtExecuteInit(dt_e);
    dtMemoryInit(dt_m);
}

void CorePipelined::step() {
    // TODO implement pipelined
    writeback(dt_m);
    dt_m =memory(dt_e);
    dt_e = execute(dt_d);
    dt_d = decode(dt_f);
    dt_f = fetch();
    handle_pc(dt_d);
}
