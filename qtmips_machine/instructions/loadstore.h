#ifndef LOADSTORE_H
#define LOADSTORE_H

#include "instruction.h"

enum InstructionLoadStoreT {
    ILST_B, // Byte
    ILST_HW, // Half word
    ILST_WL, // Word left
    ILST_W, // Word
    ILST_BU, // Byte unsigned
    ILST_HU, // Half word unsigned
    ILST_WR // Word right
};

class InstructionLoad : public InstructionI {
public:
    InstructionLoad(enum InstructionLoadStoreT type, std::uint8_t rs, std::uint8_t rt, std::uint16_t offset);
    QVector<QString> to_strs();
private:
    enum InstructionLoadStoreT type;
};

class InstructionStore : public InstructionI {
public:
    InstructionStore(enum InstructionLoadStoreT type, std::uint8_t rs, std::uint8_t rt, std::uint16_t offset);
    QVector<QString> to_strs();
private:
    enum InstructionLoadStoreT type;
};

#endif // LOADSTORE_H
