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
};

struct DecodeInterstage {
    Instruction inst = Instruction::NOP;
    bool memread = false;          // If memory should be read
    bool memwrite = false;         // If memory should write input
    bool alusrc = false;           // If second value to alu is immediate value (rt used otherwise)
    bool regwrite = true;          // If output should be written back to register
    bool alu_req_rs = false;       // requires rs value for ALU
    bool alu_req_rt = false;       // requires rt value for ALU or SW
    bool branch = false;           // branch instruction
    bool jump = false;             // jump
    bool bj_not = false;           // negate branch condition
    enum AluOp aluop = AluOp::ADD; // Decoded ALU operation
    enum AccessControl memctl = AC_NONE; // Decoded memory access type
    uint8_t num_rs = 0;                  // Number of the register s1
    uint8_t num_rt = 0;                  // Number of the register s2
    uint8_t num_rd = 0;                  // Number of the register d
    RegisterValue val_rs = 0;            // Value from register rs
    RegisterValue val_rs_orig = 0;       // Value from register rs1 without forwarding
    RegisterValue val_rt = 0;            // Value from register rt
    RegisterValue val_rt_orig = 0;       // Value from register rs1 without forwarding
    RegisterValue immediate_val = 0;     // Sign-extended immediate value
                                         // rd according to regd)
    ForwardFrom ff_rs = FORWARD_NONE;
    ForwardFrom ff_rt = FORWARD_NONE;
    Address inst_addr = STAGEADDR_NONE; // Address of instruction
    enum ExceptionCause excause = EXCAUSE_NONE;
    bool stall = false;
    bool is_valid = false;
    bool alu_mod = false; // alternative versions of ADD and right-shift
    bool alu_pc = false;  // PC is input to ALU

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
        this->internal.alu_op_num = static_cast<unsigned>(interstage.aluop);
    }
    DecodeState() = default;
    DecodeState(const DecodeState &) = default;
};

struct ExecuteInterstage {
    Instruction inst = Instruction::NOP;
    bool memread = false;
    bool memwrite = false;
    bool regwrite = true;
    bool is_valid = false;
    bool branch = false;
    bool branch_taken = false;
    bool jump = false;
    bool bj_not = false;
    bool alu_zero = false;
    enum AccessControl memctl = AC_NONE;
    RegisterValue val_rt = 0;
    uint8_t num_rd = 0;
    RegisterValue alu_val = 0;          // Result of ALU execution
    Address inst_addr = STAGEADDR_NONE; // Address of instruction
    enum ExceptionCause excause = EXCAUSE_NONE;
    Address branch_target = 0_addr;

public:
    /** Reset to value corresponding to NOP. */
    void flush() { *this = {}; }
};

struct ExecuteInternalState {
    bool alu_src = false;
    bool branch = false;
    bool alu_pc = false; // PC is input to ALU
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
};

struct MemoryInterstage {
    Instruction inst = Instruction::NOP;
    bool memtoreg = false;
    bool regwrite = true;
    uint8_t num_rd = 0;
    RegisterValue towrite_val = 0;
    Address mem_addr = 0_addr;          // Address used to access memory
    Address inst_addr = STAGEADDR_NONE; // Address of instruction
    Address next_pc = 0_addr;           // computed and expected `inst_addr` of next instruction in
                                        // pipeline.
    enum ExceptionCause excause = EXCAUSE_NONE;
    bool is_valid = false;

public:
    /** Reset to value corresponding to NOP. */
    void flush() { *this = {}; }
};

struct MemoryInternalState {
    bool memwrite = false;
    bool memread = false;
    bool branch = false;
    bool jump = false;
    bool branch_or_jump = false;
    RegisterValue mem_read_val = 0;
    RegisterValue mem_write_val = 0;
    unsigned excause_num = 0;
    RegisterValue mem_addr = 0;
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
};

struct WritebackInternalState {
    Instruction inst = Instruction::NOP;
    Address inst_addr = STAGEADDR_NONE;
    bool regwrite = true;
    bool memtoreg = false;
    uint8_t num_rd = 0;
    RegisterValue value = 0;
};

struct WritebackState {
    WritebackInternalState internal {};

    WritebackState(WritebackInternalState stage) : internal(std::move(stage)) {}
    WritebackState() = default;
    WritebackState(const WritebackState &) = default;
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
