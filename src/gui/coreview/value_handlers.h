#ifndef QTRVSIM_VALUE_HANDLERS_H
#define QTRVSIM_VALUE_HANDLERS_H

#include <instruction.h>
#include <machine/memory/address.h>
#include <machine/register_value.h>
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

class PCValue {
public:
    PCValue(svgscene::SimpleTextItem *element, const machine::Address &data);
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

class DebugValue {
public:
    DebugValue(svgscene::SimpleTextItem *element, const unsigned int &data);
    void update();
    static constexpr const char *COMPONENT_NAME = "reg-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const unsigned &data;
};

class MultiTextValue {
public:
    MultiTextValue(
        svgscene::SimpleTextItem *element,
        std::pair<const unsigned int &, const std::vector<QString> &> data);
    void update();
    static constexpr const char *COMPONENT_NAME = "multi-text-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const unsigned &current_text_index;
    const std::vector<QString> &text_table;
};

class InstructionValue {
public:
    InstructionValue(
        svgscene::SimpleTextItem *element,
        std::pair<const machine::Instruction &, const machine::Address &> data);
    void update();
    static constexpr const char *COMPONENT_NAME = "instruction-value";

private:
    BORROWED svgscene::SimpleTextItem *const element;
    const machine::Instruction &instruction_data;
    const machine::Address &address_data;
};

#endif // QTRVSIM_VALUE_HANDLERS_H
