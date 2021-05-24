#include "value_handlers.h"

using svgscene::SimpleTextItem;

BoolValue::BoolValue(SimpleTextItem *const element, const bool &data)
    : element(element)
    , data(data) {}

void BoolValue::update() {
    element->setText(data ? "1" : "0");
}

PCValue::PCValue(SimpleTextItem *element, const machine::Address &data)
    : element(element)
    , data(data) {}

void PCValue::update() {
    element->setText(QString("0x%1").arg(data.get_raw(), 8, 16, QChar('0')).toUpper());
}

RegValue::RegValue(SimpleTextItem *element, const machine::RegisterValue &data)
    : element(element)
    , data(data) {}

void RegValue::update() {
    element->setText(QString("%1").arg(data.as_u32(), 8, 16, QChar('0')).toUpper());
}

RegIdValue::RegIdValue(svgscene::SimpleTextItem *element, const uint8_t &data)
    : element(element)
    , data(data) {}

void RegIdValue::update() {
    element->setText(QString("%1").arg(data, 2, 10, QChar('0')));
}

DebugValue::DebugValue(SimpleTextItem *element, const unsigned int &data)
    : element(element)
    , data(data) {}

void DebugValue::update() {
    element->setText(QString("%1").arg(data, 0, 10, QChar(' ')));
}
MultiTextValue::MultiTextValue(SimpleTextItem *const element, Data data)
    : element(element)
    , current_text_index(data.first)
    , text_table(data.second) {}

void MultiTextValue::update() {
    element->setText(text_table.at(current_text_index));
}

InstructionValue::InstructionValue(SimpleTextItem *const element, Data data)
    : element(element)
    , instruction_data(data.first)
    , address_data(data.second) {}

void InstructionValue::update() {
    element->setText(instruction_data.to_str(address_data));
}
