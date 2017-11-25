#include "core.h"
#include "programloader.h"

#define DM_SUPPORTED (1L<<0)
#define DM_MEM2REG (1L<<1)
#define DM_MEMWRITE (1L<<2)
#define DM_ALUSRC (1L<<3)
#define DM_REGD (1L<<4)
#define DM_REGWRITE (1L<<5)
#define DM_BRANCH (1L<<6)

 struct DecodeMap {
    long flags;
    enum AluOp alu;
};


// This is temporally operation place holder
#define NOPE { .flags = 0, .alu = ALU_OP_SLL }

#define FLAGS_ALU_I (DM_SUPPORTED | DM_ALUSRC | DM_REGWRITE)

// This is map from opcode to signals.
static const struct DecodeMap dmap[]  = {
    { .flags = DM_SUPPORTED | DM_REGD | DM_REGWRITE, .alu = ALU_OP_SLL }, // Alu operations
    NOPE, // Branch on alu operations
    NOPE, // J
    NOPE, // JAL
    NOPE, // BEQ
    NOPE, // BNE
    NOPE, // BLEZ
    NOPE, // BGTZ
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
    regs->pc_inc();
    return {
        .inst = inst
    };
}

struct Core::dtDecode Core::decode(struct dtFetch dt) {
    struct DecodeMap dec = dmap[dt.inst.opcode()];
    if (!dec.flags & DM_SUPPORTED)
        // TODO message
        throw QTMIPS_EXCEPTION(UnsupportedInstruction, "", "");

    return {
        .inst = dt.inst,
        .mem2reg = dec.flags & DM_MEM2REG,
        .memwrite = dec.flags & DM_MEMWRITE,
        .alusrc = dec.flags & DM_ALUSRC,
        .regd = dec.flags & DM_REGD,
        .regwrite = dec.flags & DM_REGWRITE,
        .branch = dec.flags & DM_BRANCH,
        .aluop = dt.inst.opcode() == 0 ? (enum AluOp)dt.inst.funct() : dec.alu,
        .val_rs = regs->read_gp(dt.inst.rs()),
        .val_rt = regs->read_gp(dt.inst.rt()),
    };
    // TODO on jump there should be delay slot. Does processor addes it or compiler. And do we care?
}

struct Core::dtExecute Core::execute(struct dtDecode dt) {
    // TODO signals

    return {
        .regwrite = dt.regwrite,
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

CoreSingle::CoreSingle(Registers *regs, MemoryAccess *mem) : \
    Core(regs, mem) {
    // Nothing to do
}

void CoreSingle::step() {
    struct dtFetch f = fetch();
    struct dtDecode d = decode(f);
    struct dtExecute e = execute(d);
    struct dtMemory m = memory(e);
    writeback(m);
}

CorePipelined::CorePipelined(Registers *regs, MemoryAccess *mem) : \
    Core(regs, mem) {
    // Initialize to NOPE //
    // dtFetch
    dt_f.inst = Instruction(0x00);
    // dtDecode
    dt_d.inst = dt_f.inst;
    dt_d.mem2reg = false;
    dt_d.memwrite = false;
    dt_d.alusrc = false;
    dt_d.regd = false;
    dt_d.regwrite = false;
    dt_d.branch = false;
    dt_d.aluop = ALU_OP_SLL;
    dt_d.val_rs = 0;
    dt_d.val_rt = 0;
    // dtExecute
    dt_e.regwrite = dt_d.regwrite;
    dt_e.rwrite = dt_d.regwrite;
    dt_e.alu_val = 0;
    // dtMemory
    dt_m.regwrite = dt_e.regwrite;
    dt_m.rwrite = dt_e.rwrite;
    dt_m.alu_val = dt_e.alu_val;
}

void CorePipelined::step() {
    // TODO implement pipelined
    writeback(dt_m);
    dt_m =memory(dt_e);
    dt_e = execute(dt_d);
    dt_d = decode(dt_f);
    dt_f = fetch();
}
