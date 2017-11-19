#include "core.h"
#include "programloader.h"

 struct DecodeMap {
    bool supported, mem2reg, memwrite, alubimm, regd, regwrite, branch;
};


// This is temporally operation place holder
#define NOPE { .supported = false }

// This is map from opcode to signals.
static const struct DecodeMap dmap[]  = {
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = false, .regwrite = true, .branch = false }, // Alu operations and more
    // TODO These are just copies of first one
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = false, .regwrite = false, .branch = true }, // Branch on alu operations
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // J
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // JAL
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // BEQ
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // BNE
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // BLEZ
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // BGTZ
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // ADDI
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // ADDIU
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // SLTI
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // SLTIU
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // ANDI
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // ORI
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // XORI
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // LUI
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
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // LB
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // LH
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // LWL
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // LW
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // LBU
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // LHU
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // LWR
    NOPE, // 39
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // SB
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // SH
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // SWL
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // SW
    NOPE, // 44
    NOPE, // 45
    { .supported = true, .mem2reg = false, .memwrite = false, .alubimm = false, .regd = true, .regwrite = true, .branch = false }, // SWR
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
    if (!dec.supported)
        // TODO message
        throw QTMIPS_EXCEPTION(UnsupportedInstruction, "", "");
    enum AluOp d_alu = ALU_OP_SLL; // TODO decode for real
    return {
        .mem2reg = dec.mem2reg,
        .memwrite = dec.memwrite,
        .alubimm = dec.alubimm,
        .regd = dec.regd,
        .regwrite = dec.regwrite,
        .branch = dec.branch,
        .aluop = d_alu,
        .val_rs = regs->read_gp(dt.inst.rs()),
        .val_rt = regs->read_gp(dt.inst.rt()),
        .val_sa = dt.inst.shamt(),
        .val_immediate = dt.inst.immediate(),
    };
    // TODO on jump there should be delay slot. Does processor addes it or compiler. And do we care?
}

struct Core::dtExecute Core::execute(struct dtDecode dt) {
    // TODO signals
    return {
        .mem2reg = dt.mem2reg,
        .val = alu_operate(dt.aluop, dt.val_rs, dt.val_rt, dt.val_sa)
    };
}

struct Core::dtMemory Core::memory(struct dtExecute dt) {
    // TODO signals
    return {
        .mem2reg = dt.mem2reg,
        .val = dt.val,
    };
}

void Core::writeback(struct dtMemory dt) {
    if (dt.mem2reg) {

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
    // Nothing to do
}

void CorePipelined::step() {
    // TODO implement pipelined
    struct dtFetch f = fetch();
    struct dtDecode d = decode(f);
    struct dtExecute e = execute(d);
    struct dtMemory m =memory(e);
    writeback(m);
}
