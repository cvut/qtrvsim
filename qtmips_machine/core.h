#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <qtmipsexception.h>
#include <machineconfig.h>
#include <registers.h>
#include <memory.h>
#include <instruction.h>
#include <alu.h>

namespace machine {

class Core : public QObject {
    Q_OBJECT
public:
    Core(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data);

    virtual void step() = 0; // Do single step

    virtual void reset() = 0; // Reset core (only core, memory and registers has to be reseted separately)

signals:
    void instruction_fetched(const machine::Instruction &inst);
    void instruction_decoded(const machine::Instruction &inst);
    void instruction_executed(const machine::Instruction &inst);
    void instruction_memory(const machine::Instruction &inst);
    void instruction_writeback(const machine::Instruction &inst);
    void instruction_program_counter(const machine::Instruction &inst);

protected:
    Registers *regs;
    MemoryAccess *mem_data, *mem_program;

    struct dtFetch {
        Instruction inst; // Loaded instruction
    };
    struct dtDecode {
        Instruction inst;
        bool memread; // If memory should be read
        bool memwrite; // If memory should write input
        bool alusrc; // If second value to alu is immediate value (rt used otherwise)
        bool regd; // If rd is used (otherwise rt is used for write target)
        bool regwrite; // If output should be written back to register (which one depends on regd)
        enum AluOp aluop; // Decoded ALU operation
        enum MemoryAccess::AccessControl memctl; // Decoded memory access type
        std::uint32_t val_rs; // Value from register rs
        std::uint32_t val_rt; // Value from register rt
    };
    struct dtExecute {
        Instruction inst;
        bool memread;
        bool memwrite;
        bool regwrite;
        enum MemoryAccess::AccessControl memctl;
        std::uint32_t val_rt;
        std::uint8_t rwrite; // Writeback register (multiplexed between rt and rd according to regd)
        std::uint32_t alu_val; // Result of ALU execution
    };
    struct dtMemory {
        Instruction inst;
        bool regwrite;
        std::uint8_t rwrite;
        std::uint32_t towrite_val;
    };

    struct dtFetch fetch();
    struct dtDecode decode(const struct dtFetch&);
    struct dtExecute execute(const struct dtDecode&);
    struct dtMemory memory(const struct dtExecute&);
    void writeback(const struct dtMemory&);
    void handle_pc(const struct dtDecode&);

    // Initialize structures to NOPE instruction
    void dtFetchInit(struct dtFetch &dt);
    void dtDecodeInit(struct dtDecode &dt);
    void dtExecuteInit(struct dtExecute &dt);
    void dtMemoryInit(struct dtMemory &dt);
};

class CoreSingle : public Core {
public:
    CoreSingle(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data, bool jmp_delay_slot);
    ~CoreSingle();

    void step();

    void reset();

private:
    struct Core::dtDecode *jmp_delay_decode;
};

class CorePipelined : public Core {
public:
    CorePipelined(Registers *regs, MemoryAccess *mem_program, MemoryAccess *mem_data, enum MachineConfig::HazardUnit hazard_unit = MachineConfig::HU_STALL_FORWARD);

    void step();

    void reset();

private:
    struct Core::dtFetch dt_f;
    struct Core::dtDecode dt_d;
    struct Core::dtExecute dt_e;
    struct Core::dtMemory dt_m;

    enum MachineConfig::HazardUnit hazard_unit;
};

}

#endif // CORE_H
