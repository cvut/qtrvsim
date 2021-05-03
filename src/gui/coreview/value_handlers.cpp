#include "value_handlers.h"

BoolValue::BoolValue(svgscene::SimpleTextItem *const element, const bool &data)
    : element(element)
    , data(data) {}

void BoolValue::update() {
    element->setText(data ? "1" : "0");
}
PCValue::PCValue(svgscene::SimpleTextItem *element, const machine::Address &data)
    : element(element)
    , data(data) {}

void PCValue::update() {
    element->setText(QString("0x%1").arg(data.get_raw(), 8, 16, QChar('0')));
}
RegValue::RegValue(svgscene::SimpleTextItem *element, const machine::RegisterValue &data)
    : element(element)
    , data(data) {}
void RegValue::update() {
    element->setText(QString("0x%1").arg(data.as_u32(), 8, 16, QChar('0')));
}
DebugValue::DebugValue(svgscene::SimpleTextItem *element, const unsigned int &data)
    : element(element)
    , data(data) {}
void DebugValue::update() {
    element->setText(QString("0x%1").arg(data, 0, 10, QChar(' ')));
}
MultiTextValue::MultiTextValue(
    svgscene::SimpleTextItem *const element,
    std::pair<const unsigned int &, const std::vector<QString> &> data)
    : element(element)
    , current_text_index(data.first)
    , text_table(data.second) {}

void MultiTextValue::update() {
    element->setText(text_table.at(current_text_index));
}

InstructionValue::InstructionValue(
    svgscene::SimpleTextItem *const element,
    std::pair<const machine::Instruction &, const machine::Address &> data)
    : element(element)
    , instruction_data(data.first)
    , address_data(data.second) {}

void InstructionValue::update() {
    element->setText(instruction_data.to_str(address_data));
}
