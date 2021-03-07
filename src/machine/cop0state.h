#ifndef COP0STATE_H
#define COP0STATE_H

#include "machinedefs.h"
#include "memory/address.h"
#include "register_value.h"

#include <QObject>
#include <QString>
#include <cstdint>

namespace machine {

class Core;

class Cop0State : public QObject {
    Q_OBJECT
    friend class Core;

public:
    enum Cop0Registers {
        Unsupported = 0,
        UserLocal,
        BadVAddr, // Reports the address for the most recent address-related
                  // exception
        Count,    // Processor cycle count
        Compare,  // Timer interrupt control
        Status,   // Processor status and control
        Cause,    // Cause of last exception
        EPC,      // Program counter at last exception
        EBase,    // Exception vector base register
        Config,   // Configuration registers
        COP0REGS_CNT,
    };

    enum StatusReg {
        Status_IE = 0x00000001,
        Status_EXL = 0x00000002,
        Status_ERL = 0x00000004,
        Status_IntMask = 0x0000ff00,
        Status_Int0 = 0x00000100,
    };

    Cop0State(Core *core = nullptr);
    Cop0State(const Cop0State &);

    uint32_t read_cop0reg(enum Cop0Registers reg) const;
    uint32_t read_cop0reg(uint8_t rd, uint8_t sel) const; // Read coprocessor 0
                                                          // register
    void write_cop0reg(enum Cop0Registers reg, RegisterValue value);
    void write_cop0reg(
        uint8_t reg,
        uint8_t sel,
        RegisterValue value); // Write coprocessor 0 register
    static QString cop0reg_name(enum Cop0Registers reg);

    bool operator==(const Cop0State &c) const;
    bool operator!=(const Cop0State &c) const;

    void reset(); // Reset all values to zero

    bool core_interrupt_request();
    Address exception_pc_address();

signals:
    void cop0reg_update(enum Cop0Registers reg, uint32_t val);
    void cop0reg_read(enum Cop0Registers reg, uint32_t val) const;

public slots:
    void set_interrupt_signal(uint irq_num, bool active);
    void set_status_exl(bool value);

protected:
    void setup_core(Core *core);
    void update_execption_cause(enum ExceptionCause excause, bool in_delay_slot);
    void update_count_and_compare_irq();

private:
    typedef uint32_t (Cop0State::*reg_read_t)(enum Cop0Registers reg) const;

    typedef void (
        Cop0State::*reg_write_t)(enum Cop0Registers reg, uint32_t value);

    struct cop0reg_desc_t {
        const char *name;
        uint32_t write_mask;
        uint32_t init_value;
        reg_read_t reg_read;
        reg_write_t reg_write;
    };

    static const cop0reg_desc_t cop0reg_desc[COP0REGS_CNT];

    uint32_t read_cop0reg_default(enum Cop0Registers reg) const;
    void write_cop0reg_default(enum Cop0Registers reg, uint32_t value);
    void write_cop0reg_count_compare(enum Cop0Registers reg, uint32_t value);
    void write_cop0reg_user_local(enum Cop0Registers reg, uint32_t value);
    Core *core;
    uint32_t cop0reg[COP0REGS_CNT] {}; // coprocessor 0 registers
    uint32_t last_core_cycles {};
};

} // namespace machine

Q_DECLARE_METATYPE(machine::Cop0State)
Q_DECLARE_METATYPE(machine::Cop0State::Cop0Registers)

#endif // COP0STATE_H
