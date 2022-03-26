#ifndef CONSTOLSTATE_H
#define CONSTOLSTATE_H

#include "machinedefs.h"
#include "memory/address.h"
#include "register_value.h"

#include <QObject>
#include <QString>
#include <cstdint>

namespace machine {

class Core;

class ControlState : public QObject {
    Q_OBJECT
    friend class Core;

public:
    enum CsrRegisters {
        Unsupported = 0,

        // Machine Information Registers
        //mvendorid,  // Vendor ID.
        //marchid,    // Architecture ID.
        //mimpid,     // Implementation ID.
        //mhartid,    // Hardware thread ID.

        // Machine Trap Setup
        mstatus,   // Machine status register.
        // misa,      // ISA and extensions
        // mie,       // Machine interrupt-enable register.
        mtvec,     // Machine trap-handler base address.

        // Machine Trap Handling
        // mscratch,  // Scratch register for machine trap handlers.
        mepc,     // Machine exception program counter.
        mcause,   // Machine trap cause.
        mtval,    // Machine bad address or instruction.
        //mip,       // Machine interrupt pending.
        //mtinst     // Machine trap instruction (transformed).
        //mtval2     // Machine bad guest physical address.

        mcycle,   // Machine cycle counter.
        Compare,  // Timer interrupt control
        CSR_REGS_CNT,
    };

    enum StatusReg {
        Status_IE = 0x00000001,
        Status_EXL = 0x00000002,
        Status_ERL = 0x00000004,
        Status_IntMask = 0x0000ff00,
        Status_Int0 = 0x00000100,
    };

    ControlState(Core *core = nullptr);
    ControlState(const ControlState &);

    uint64_t read_csr(enum CsrRegisters reg) const;
    uint64_t read_csr(uint32_t rd, uint8_t sel) const; // Read CSR register
    void write_csr(enum CsrRegisters reg, RegisterValue value);
    void write_csr(
        uint32_t reg,
        uint8_t sel,
        RegisterValue value); // Write coprocessor CSR register
    static QString csr_name(enum CsrRegisters reg);

    bool operator==(const ControlState &c) const;
    bool operator!=(const ControlState &c) const;

    void reset(); // Reset all values to zero

    bool core_interrupt_request();
    Address exception_pc_address();

signals:
    void csr_update(enum CsrRegisters reg, uint64_t val);
    void csr_read(enum CsrRegisters reg, uint64_t val) const;

public slots:
    void set_interrupt_signal(uint irq_num, bool active);
    void set_status_exl(bool value);

protected:
    void setup_core(Core *core);
    void update_execption_cause(enum ExceptionCause excause);
    void update_count_and_compare_irq();

private:
    typedef uint64_t (ControlState::*reg_read_t)(enum CsrRegisters reg) const;

    typedef void (
        ControlState::*reg_write_t)(enum CsrRegisters reg, uint64_t value);

    struct csrRegDesc_t {
        const char *name;
        uint64_t write_mask;
        uint64_t init_value;
        reg_read_t reg_read;
        reg_write_t reg_write;
    };

    static const csrRegDesc_t csrRegDesc[CSR_REGS_CNT];

    uint64_t read_csr_default(enum CsrRegisters reg) const;
    void write_csr_default(enum CsrRegisters reg, uint64_t value);
    void write_csr_count_compare(enum CsrRegisters reg, uint64_t value);
    Core *core;
    uint64_t csr_data[CSR_REGS_CNT] {}; // CSR registers
    uint64_t last_core_cycles {};
};

} // namespace machine

Q_DECLARE_METATYPE(machine::ControlState)
Q_DECLARE_METATYPE(machine::ControlState::CsrRegisters)

#endif // CONSTOLSTATE_H
