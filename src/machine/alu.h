#ifndef ALU_H
#define ALU_H

#include "machinedefs.h"
#include "register_value.h"
#include "registers.h"

#include <QObject>
#include <QString>
#include <cstdint>

namespace machine {

// Do ALU operation.
// excause: Reported exception by given operation
// operation: This is function field from instruction or shifted opcode for
// immediate instructions s: Loaded from rs. Also calles as source. t: Loaded
// from rt or immediate field from instruction it self. Also called as target.
// sa: This is value directly from instruction it self (sa section) used for
// shift operations sz: This is value directly from instruction it self used in
// filed extract instructions regs: Registers used. We need direct access to lo
// and hi registers (those are not accessed from core it self but from alu
// directly Returned value is commonly saved to rt/rd or any other way passed
// trough core
RegisterValue alu_operate(
    enum AluOp operation,
    RegisterValue s,
    RegisterValue t,
    uint8_t sa,
    uint8_t sz,
    Registers *regs,
    bool &discard,
    ExceptionCause &excause);

} // namespace machine

Q_DECLARE_METATYPE(machine::AluOp)
Q_DECLARE_METATYPE(machine::ExceptionCause)

#endif // ALU_H
