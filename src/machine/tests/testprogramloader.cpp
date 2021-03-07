#include "machine/instruction.h"
#include "machine/memory/memory_utils.h"
#include "machine/programloader.h"
#include "memory/backend/memory.h"
#include "tst_machine.h"

using namespace machine;

// This is common program start (initial value of program counter)
#define PC_INIT 0x80020000

void MachineTests::program_loader() {
    ProgramLoader pl("data");
    Memory m(BIG);
    pl.to_memory(&m);

    // 	addi $1, $0, 6
    QCOMPARE(
        Instruction(memory_read_u32(&m, PC_INIT)), Instruction(8, 0, 1, 6));
    // j (8)0020000 (only 28 bits are used and they are logically shifted left
    // by 2)
    QCOMPARE(
        Instruction(memory_read_u32(&m, PC_INIT + 4)),
        Instruction(2, Address(0x20000 >> 2)));
    // TODO add some more code to data and do more compares (for example more
    // sections)
}
