#ifndef CORE_H
#define CORE_H

#include <QObject>
#include "qtmipsexception.h"
#include "registers.h"
#include "memory.h"
#include "instruction.h"
#include "alu.h"

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
        bool mem2reg; // Write memory output to register (instead alu output)
        bool memwrite; // If memory should write input
        bool alusrc; // If second value to alu is immediate value (rt used otherwise)
        bool regd; // If rd is used (otherwise rt is used for write target)
        bool regwrite; // If output should be written back to register (which one depends on regd)
        bool branch; // If this is branch instruction
        enum AluOp aluop; // Decoded ALU operation
        std::uint32_t val_rs; // Value from register rs
        std::uint32_t val_rt; // Value from register rt
    };
    struct dtExecute {
        bool regwrite;
        std::uint8_t rwrite; // Writeback register (multiplexed between rt and rd according to regd)
        std::uint32_t alu_val; // Result of ALU execution
        // TODO
    };
    struct dtMemory {
        bool regwrite;
        std::uint8_t rwrite;
        std::uint32_t alu_val;
        // TODO
    };

    struct dtFetch fetch();
    struct dtDecode decode(struct dtFetch);
    struct dtExecute execute(struct dtDecode);
    struct dtMemory memory(struct dtExecute);
    void writeback(struct dtMemory);

};

class CoreSingle : public Core {
public:
    CoreSingle(Registers *regs, MemoryAccess *mem);

    void step();
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

#endif // CORE_H
