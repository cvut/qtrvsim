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
        bool mem2reg; // Write memory output to register (instead alu output)
        bool memwrite; // If memory should write input
        bool alubimm; // If b value to alu is immediate value (rt used otherwise)
        bool regd; // If rd is used (otherwise rt is used for write target)
        bool regwrite; // If output should be written back to register (which one depends on regd)
        bool branch; // If this is branch instruction
        enum AluOp aluop; // Decoded ALU operation
        std::uint32_t val_rs; // Value from register rs
        std::uint32_t val_rt; // Value from register rt
        std::uint8_t val_sa; // Value of sa in instruction it self
        std::uint16_t val_immediate; // Value of immediate in instruction it self
    };
    struct dtExecute {
        bool mem2reg;
        std::uint32_t val;
        // TODO
    };
    struct dtMemory {
        bool mem2reg;
        // TODO
        std::uint32_t val;
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
};

#endif // CORE_H
