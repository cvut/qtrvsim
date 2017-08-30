#ifndef REGISTERS_H
#define REGISTERS_H

#include <QObject>
#include <cstdint>

class Registers : public QObject {
    Q_OBJECT
public:
    Registers();

    std::uint32_t read_pc(); // Return current value of program counter
    std::uint32_t pc_inc(); // Increment program counter by four bytes
    std::uint32_t pc_jmp(std::int32_t offset); // Relative jump from current location in program counter
    void pc_abs_jmp(std::uint32_t address); // Absolute jump in program counter (write to pc)

    std::uint32_t read_gp(std::uint8_t i); // Read general-purpose register
    void write_gp(std::uint8_t i, std::uint32_t value); // Write general-purpose register
    std::uint32_t read_hi_lo(bool hi); // true - read HI / false - read LO
    void write_hi_lo(bool hi, std::uint32_t value);

signals:
    // TODO signals

private:
    std::uint32_t gp[31]; // general-purpose registers ($0 is intentionally skipped)
    std::uint32_t hi, lo;
    std::uint32_t pc; // program counter
};

#endif // REGISTERS_H
