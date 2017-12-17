#ifndef CORE_H
#define CORE_H

#include <QObject>
#include "qtmipsexception.h"
#include "registers.h"
#include "memory.h"
#include "instruction.h"
#include "alu.h"

namespace machine {

class Core : public QObject {
    Q_OBJECT
public:
    Core(Registers *regs, MemoryAccess *mem);

    virtual void step() = 0; // Do single step

signals:

protected:
    Registers *regs;
    MemoryAccess *mem;

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
        bool memread;
        bool memwrite;
        bool regwrite;
        enum MemoryAccess::AccessControl memctl;
        std::uint32_t val_rt;
        std::uint8_t rwrite; // Writeback register (multiplexed between rt and rd according to regd)
        std::uint32_t alu_val; // Result of ALU execution
    };
    struct dtMemory {
        bool regwrite;
        std::uint8_t rwrite;
        std::uint32_t towrite_val;
    };

    struct dtFetch fetch();
    struct dtDecode decode(struct dtFetch);
    struct dtExecute execute(struct dtDecode);
    struct dtMemory memory(struct dtExecute);
    void writeback(struct dtMemory);
    void handle_pc(struct dtDecode);

    // Initialize structures to NOPE instruction
    void dtFetchInit(struct dtFetch &dt);
    void dtDecodeInit(struct dtDecode &dt);
    void dtExecuteInit(struct dtExecute &dt);
    void dtMemoryInit(struct dtMemory &dt);
};

class CoreSingle : public Core {
public:
    CoreSingle(Registers *regs, MemoryAccess *mem);

    void step();

private:
    struct Core::dtDecode jmp_delay_decode;
};

class CorePipelined : public Core {
public:
    CorePipelined(Registers *regs, MemoryAccess *mem);

    void step();

private:
    struct Core::dtFetch dt_f;
    struct Core::dtDecode dt_d;
    struct Core::dtExecute dt_e;
    struct Core::dtMemory dt_m;
};

}

#endif // CORE_H
