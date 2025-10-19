/**
 * State of the core pipeline.
 *
 * Each internal has a state struct. The state struct is composed of the
 * internal state for visualization and two sets of outgoing interstage
 * registers - result and final. Both are filled with the same data as the
 * stage finishes, but result is never later modified. Final will be modified
 * by flushed, exceptions etc. and it will be used for further execution.
 *
 * TODO:
 * - Move init functions here as methods (constructor/discard).
 *
 * @file
 */
#ifndef STAGES_H
#define STAGES_H

#include "instruction.h"
#include "machinedefs.h"
#include "memory/address.h"
#include "registers.h"

#include <cstdint>
#include <utility>

namespace machine {

enum ForwardFrom {
    FORWARD_NONE = 0b00,
    FORWARD_FROM_W = 0b01,
    FORWARD_FROM_M = 0b10,
};

struct PCInterstage {
    bool stop_if = false;
};

struct PCState {
    PCInterstage final {};
};

struct FetchInterstage {
    Instruction inst = Instruction::NOP; // Loaded instruction
    Address inst_addr = STAGEADDR_NONE;  // Address of instruction
    Address next_inst_addr = 0_addr;     // `inst_addr` + `inst.size()`
    /** Inspecting other stages to get this value is problematic due to stalls and flushed.
     * Therefore we pass it through the whole pipeline. */
    Address predicted_next_inst_addr = 0_addr;
    enum ExceptionCause excause = EXCAUSE_NONE;
    bool is_valid = false;

public:
    /** Reset to value corresponding to NOP. */
    void flush() { *this = {}; }
};

struct FetchInternalState {
    RegisterValue fetched_value = 0;
    unsigned excause_num = 0;
};

struct FetchState {
    FetchInternalState internal {};
    FetchInterstage result {};
    FetchInterstage final {};

    FetchState(const FetchInternalState &stage, const FetchInterstage &interstage)
        : internal(stage)
        , result(interstage)
        , final(interstage) {
        this->internal.excause_num = static_cast<unsigned>(interstage.excause);
    }

    FetchState() = default;
    FetchState(const FetchState &) = default;
    FetchState &operator=(const FetchState &) = default;
};

struct DecodeInterstage {
    Instruction inst = Instruction::NOP;
    Address inst_addr = STAGEADDR_NONE;
    Address next_inst_addr = 0_addr;
    Address predicted_next_inst_addr = 0_addr;
    RegisterValue val_rs = 0;        // Value from register rs
    RegisterValue val_rs_orig = 0;   // Value from register rs1 without forwarding
    RegisterValue val_rt = 0;        // Value from register rt
    RegisterValue val_rt_orig = 0;   // Value from register rs1 without forwarding
    RegisterValue immediate_val = 0; // Sign-extended immediate value
                                     // rd according to regd)
    RegisterValue csr_read_val = 0;  // Value read from csr
    CSR::Address csr_address = CSR::Address(0);
    ExceptionCause excause = EXCAUSE_NONE;
    ForwardFrom ff_rs = FORWARD_NONE;
    ForwardFrom ff_rt = FORWARD_NONE;
    AluComponent alu_component; // Selects computational component in alu - basic ALU / MUL.
    AluCombinedOp aluop = { .alu_op = AluOp::ADD }; // Decoded ALU operation
    AccessControl memctl = AC_NONE;                 // Decoded memory access type
    RegisterId num_rs = 0;                          // Number of the register s1
    RegisterId num_rt = 0;                          // Number of the register s2
    RegisterId num_rd = 0;                          // Number of the register d
    bool memread = false;                           // If memory should be read
    bool memwrite = false;                          // If memory should write input
    bool alusrc = false;      // If second value to alu is immediate value (rt used otherwise)
    bool regwrite = false;    // If output should be written back to register
    bool regwrite_fp = false; // If output should be written back to floating point register
    bool alu_req_rs = false;  // requires rs value for ALU
    bool alu_req_rt = false;  // requires rt value for ALU or SW
    bool alu_fp = false;      // ALU operation is floating point
    bool branch_bxx = false;  // branch instruction
    bool branch_jal = false;  // jump
    bool branch_val = false;  // negate branch condition
    bool branch_jalr = false; // JALR: write PC+4 to register and jump to ALU result
    bool stall = false;
    bool is_valid = false;
    bool w_operation = false; // ALU or other operation is limited to word size (32-bits)
    bool alu_mod = false;     // alternative versions of ADD and right-shift
    bool alu_pc = false;      // PC is input to ALU
    bool csr = false;         // Zicsr, implies csr read and csr write
    bool csr_to_alu = false;
    bool csr_write = false;
    bool xret = false;        // Return from exception, MRET and SRET
    bool insert_stall_before = false;

public:
    /** Reset to value corresponding to NOP. */
    void flush() { *this = {}; }
};

struct DecodeInternalState {
    /**
     * ALU OP as a number
     * GUI needs to show a number, not enumerated value (for simple interface).
     * Core is responsive for the conversion.
     */
    unsigned alu_op_num = 0;
    unsigned excause_num = 0;
    RegisterValue inst_bus = 0;
    bool alu_mul = false;
};

struct DecodeState {
    DecodeInternalState internal {};
    DecodeInterstage result {};
    DecodeInterstage final {};

    DecodeState(const DecodeInternalState &stage, const DecodeInterstage &interstage)
        : internal(stage)
        , result(interstage)
        , final(interstage) {
        this->internal.excause_num = static_cast<unsigned>(interstage.excause);
        this->internal.alu_op_num = static_cast<unsigned>(interstage.aluop.alu_op);
    }
    DecodeState() = default;
    DecodeState(const DecodeState &) = default;
    DecodeState &operator=(const DecodeState &) = default;
};

struct ExecuteInterstage {
    Instruction inst = Instruction::NOP;
    Address inst_addr = STAGEADDR_NONE;
    Address next_inst_addr = 0_addr;
    Address predicted_next_inst_addr = 0_addr;
    Address branch_jal_target = 0_addr; //> Potential branch target (inst_addr + 4 + imm).
    RegisterValue val_rt = 0;
    RegisterValue alu_val = 0; // Result of ALU execution
    RegisterValue immediate_val = 0;
    RegisterValue csr_read_val = 0;
    CSR::Address csr_address = CSR::Address(0);
    ExceptionCause excause = EXCAUSE_NONE;
    AccessControl memctl = AC_NONE;
    RegisterId num_rd = 0;
    bool memread = false;
    bool memwrite = false;
    bool regwrite = false;
    bool regwrite_fp = false;
    bool alu_fp = false;
    bool is_valid = false;
    bool branch_bxx = false;
    bool branch_jal = false;
    bool branch_val = false;
    bool branch_jalr = false; //> @copydoc DecodeInterstage::branch_jalr
    bool alu_zero = false;
    bool csr = false;
    bool csr_write = false;
    bool xret = false;

public:
    /** Reset to value corresponding to NOP. */
    void flush() { *this = {}; }
};

struct ExecuteInternalState {
    RegisterValue alu_src1 = 0;
    RegisterValue alu_src2 = 0;
    RegisterValue immediate = 0;
    RegisterValue rs = 0;
    RegisterValue rt = 0;
    unsigned stall_status = 0;
    /**
     * ALU OP as a number.
     * GUI needs to show a number, not enumerated value (for simple interface).
     * Core is responsive for the conversion.
     */
    unsigned alu_op_num = 0;
    /**
     * Forwarding setting as a number.
     * Same note as alu_op_num.
     */
    unsigned forward_from_rs1_num = 0;
    /**
     * Forwarding setting as a number.
     * Same note as alu_op_num.
     */
    unsigned forward_from_rs2_num = 0;
    unsigned excause_num = 0;
    bool alu_src = false;
    bool alu_mul = false;
    bool branch_bxx = false;
    bool alu_pc = false; // PC is input to ALU
};

struct ExecuteState {
    ExecuteInternalState internal {};
    ExecuteInterstage result {};
    ExecuteInterstage final {};

    ExecuteState(const ExecuteInternalState &stage, const ExecuteInterstage &interstage)
        : internal(stage)
        , result(interstage)
        , final(interstage) {
        this->internal.excause_num = static_cast<unsigned>(interstage.excause);
    }
    ExecuteState() = default;
    ExecuteState(const ExecuteState &) = default;
    ExecuteState &operator=(const ExecuteState &) = default;
};

struct MemoryInterstage {
    Instruction inst = Instruction::NOP;
    Address inst_addr = STAGEADDR_NONE;
    Address next_inst_addr = 0_addr;
    Address predicted_next_inst_addr = 0_addr;
    Address computed_next_inst_addr = 0_addr;
    Address mem_addr = 0_addr; // Address used to access memory
    RegisterValue towrite_val = 0;
    ExceptionCause excause = EXCAUSE_NONE;
    RegisterId num_rd = 0;
    bool memtoreg = false;
    bool regwrite = false;
    bool regwrite_fp = false;
    bool is_valid = false;
    bool csr_written = false;

public:
    /** Reset to value corresponding to NOP. */
    void flush() { *this = {}; }
};

struct MemoryInternalState {
    RegisterValue mem_read_val = 0;
    RegisterValue mem_write_val = 0;
    RegisterValue mem_addr = 0;
    unsigned excause_num = 0;
    bool memwrite = false;
    bool memread = false;
    bool branch_bxx = false;
    bool branch_jal = false;
    bool branch_outcome = false;
    bool branch_jalx = false;
    bool branch_jalr = false;
    bool xret = false;
};

struct MemoryState {
    MemoryInternalState internal {};
    MemoryInterstage result {};
    MemoryInterstage final {};

    MemoryState(const MemoryInternalState &stage, const MemoryInterstage &interstage)
        : internal(stage)
        , result(interstage)
        , final(interstage) {
        this->internal.excause_num = static_cast<unsigned>(interstage.excause);
    }

    MemoryState() = default;
    MemoryState(const MemoryState &) = default;
    MemoryState &operator=(const MemoryState &) = default;
};

struct WritebackInternalState {
    Instruction inst = Instruction::NOP;
    Address inst_addr = STAGEADDR_NONE;
    RegisterValue value = 0;
    RegisterId num_rd = 0;
    bool regwrite = false;
    bool memtoreg = false;
};

struct WritebackState {
    WritebackInternalState internal {};

    explicit WritebackState(WritebackInternalState stage) : internal(std::move(stage)) {}
    WritebackState() = default;
    WritebackState(const WritebackState &) = default;
    WritebackState &operator=(const WritebackState &) = default;
};

struct Pipeline {
    PCState pc {};
    FetchState fetch {};
    DecodeState decode {};
    ExecuteState execute {};
    MemoryState memory {};
    WritebackState writeback {};
};
} // namespace machine

#endif // STAGES_H
