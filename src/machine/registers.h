#ifndef REGISTERS_H
#define REGISTERS_H

#include "memory/address.h"
#include "register_value.h"
#include "simulator_exception.h"

#include <QObject>
#include <array>
#include <cstdint>

namespace machine {

/**
 * General-purpose register count
 */
static const size_t REGISTER_COUNT = 32;

/**
 * General-purpose register identifier
 */
class RegisterId {
public:
    // TODO: Should this constructor allow implicit conversion?
    inline RegisterId(uint8_t value);

    uint8_t data;
};

inline RegisterId::RegisterId(uint8_t value) : data(value) {
    // Bounds on the id are checked at creation time and its value is immutable.
    // Therefore all check at when used are redundant.
    // Main advantege is, that possible errors will appear when creating the
    // value, which is probably close to the bug source.
    SANITY_ASSERT(
        data < REGISTER_COUNT,
        QString("Trying to create register id for out-of-bounds register ")
            + QString(data));
};

inline RegisterId operator"" _reg(unsigned long long value) {
    return { static_cast<uint8_t>(value) };
}

/**
 * Register file
 */
class Registers : public QObject {
    Q_OBJECT
public:
    Registers();
    Registers(const Registers &);

    Address read_pc() const;             // Return current value of program counter
    Address pc_inc();                    // Increment program counter by four bytes
    Address pc_jmp(int32_t offset);      // Relative jump from current
                                         // location in
                                         // program counter
    void pc_abs_jmp(Address address);    // Absolute jump in program counter (write
                                         // to pc)
    void pc_abs_jmp_28(Address address); // Absolute jump in current 256MB
                                         // section (basically J implementation)

    RegisterValue read_gp(RegisterId reg) const;        // Read general-purpose
                                                        // register
    void write_gp(RegisterId reg, RegisterValue value); // Write general-purpose
                                                        // register
    RegisterValue read_hi_lo(bool hi) const; // true - read HI / false - read LO
    void write_hi_lo(bool hi, RegisterValue value);

    bool operator==(const Registers &c) const;
    bool operator!=(const Registers &c) const;

    void reset(); // Reset all values to zero (except pc)

signals:
    void pc_update(Address val);
    void gp_update(RegisterId reg, RegisterValue val);
    void hi_lo_update(bool hi, RegisterValue val);
    void gp_read(RegisterId reg, RegisterValue val) const;
    void hi_lo_read(bool hi, RegisterValue val) const;

private:
    /**
     * General purpose registers
     *
     * Zero register is always zero, but is allocated to avoid off-by-one.
     * Getters and setters will never try to read or write zero register.
     */
    std::array<RegisterValue, REGISTER_COUNT> gp {};
    RegisterValue hi {}, lo {};
    Address pc {}; // program counter
};

} // namespace machine

Q_DECLARE_METATYPE(machine::Registers)

#endif // REGISTERS_H
