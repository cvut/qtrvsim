/**
 * Components defined in here update the GUI placeholders with up to date
 * values that is read from provided source.
 *
 * Components accept different types and produce different formatting.
 *
 * @file
 */
#ifndef QTRVSIM_VALUE_HANDLERS_H
#define QTRVSIM_VALUE_HANDLERS_H

#include <instruction.h>
#include <machine/memory/address.h>
#include <machine/register_value.h>
#include <machine/registers.h>
#include <svgscene/components/simpletextitem.h>
#include <svgscene/utils/memory_ownership.h>

class BoolValue {
public:
    BoolValue(svgscene::SimpleTextItem *element, const bool &data);
    void update();
    static constexpr const char *COMPONENT_NAME = "bool-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const bool &data;
};

class PCValue : public QObject {
    Q_OBJECT

public slots:
    void clicked();
signals:
    void jump_to_pc(machine::Address pc_value);

public:
    PCValue(svgscene::SimpleTextItem *element, const machine::Address &data);
    PCValue(const PCValue &);
    void update();
    static constexpr const char *COMPONENT_NAME = "pc-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const machine::Address &data;
};

class RegValue {
public:
    RegValue(svgscene::SimpleTextItem *element, const machine::RegisterValue &data);
    void update();
    static constexpr const char *COMPONENT_NAME = "reg-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const machine::RegisterValue &data;
};

class RegIdValue {
public:
    RegIdValue(svgscene::SimpleTextItem *element, const uint8_t &data);
    void update();
    static constexpr const char *COMPONENT_NAME = "reg-id-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const uint8_t &data;
};

class DebugValue {
public:
    DebugValue(svgscene::SimpleTextItem *element, const unsigned int &data);
    void update();
    static constexpr const char *COMPONENT_NAME = "debug-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const unsigned &data;
};

class MultiTextValue {
    using Data = std::pair<const unsigned int &, const std::vector<QString> &>;

public:
    MultiTextValue(svgscene::SimpleTextItem *element, Data data);
    void update();
    static constexpr const char *COMPONENT_NAME = "multi-text-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const unsigned &current_text_index;
    const std::vector<QString> &text_table;
};

class InstructionValue {
    using Data = std::pair<const machine::Instruction &, const machine::Address &>;

public:
    InstructionValue(svgscene::SimpleTextItem *element, Data data);
    void update();
    static constexpr const char *COMPONENT_NAME = "instruction-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const machine::Instruction &instruction_data;
    const machine::Address &address_data;
};

#endif // QTRVSIM_VALUE_HANDLERS_H
