#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <QObject>
#include <qstring.h>

class Instruction : public QObject {
    Q_OBJECT
public:
    Instruction();
    Instruction(std::uint32_t inst);
    Instruction(std::uint8_t opcode, std::uint8_t rs, std::uint8_t rt, std::uint8_t rd, std::uint8_t shamt, std::uint8_t funct); // Type R
    Instruction(std::uint8_t opcode, std::uint8_t rs, std::uint8_t rt, std::uint16_t immediate); // Type I
    Instruction(std::uint8_t opcode, std::uint32_t address); // Type J
    Instruction(const Instruction&);

    QString to_str();

    std::uint8_t opcode() const;
    std::uint8_t rs() const;
    std::uint8_t rt() const;
    std::uint8_t rd() const;
    std::uint8_t shamt() const;
    std::uint8_t funct() const;
    std::uint16_t immediate() const;
    std::uint32_t address() const;
    std::uint32_t data() const;

    bool operator==(const Instruction &c) const;
    bool operator!=(const Instruction &c) const;
    Instruction &operator=(const Instruction &c);

private:
    std::uint32_t dt;
};

Q_DECLARE_METATYPE(Instruction)

#endif // INSTRUCTION_H
