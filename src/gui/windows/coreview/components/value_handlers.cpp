#include "value_handlers.h"

#include <QGraphicsPathItem>
#include <cmath>

using svgscene::SimpleTextItem;

const QString BoolValue::TRUE_STRING = QStringLiteral("1");
const QString BoolValue::FALSE_STRING = QStringLiteral("0");

const QString BoolValue::COMPONENT_NAME = QStringLiteral("bool-value");
const QString PCValue::COMPONENT_NAME = QStringLiteral("pc-value");
const QString RegValue::COMPONENT_NAME = QStringLiteral("reg-value");
const QString RegIdValue::COMPONENT_NAME = QStringLiteral("reg-id-value");
const QString DebugValue::COMPONENT_NAME = QStringLiteral("debug-value");
const QString MultiTextValue::COMPONENT_NAME = QStringLiteral("multi-text-value");
const QString InstructionValue::COMPONENT_NAME = QStringLiteral("instruction-value");

constexpr QChar to_hex_digit(unsigned int value) {
    constexpr QChar digits[16]
        = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    return digits[value];
}

constexpr char16_t to_dec_digit(unsigned int value) {
    constexpr char16_t digits[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    return digits[value];
}

template<size_t PrefixLen>
void set_hex_string(QString &buffer, uint32_t value) {
    auto it = buffer.begin() + PrefixLen;
    for (size_t i = 0; i < 8; ++i) {
        *it++ = to_hex_digit((value >> 28) & 0xf);
        value <<= 4;
    }
}

void set_dec_value(QString &buffer, uint32_t value, QChar fill_char = ' ') {
    if (value < 10) {
        buffer.resize(2, fill_char);
        buffer[0] = to_dec_digit(value);
        return;
    }

    auto len = (size_t)ceil(log10(value) + 1);
    buffer.resize(len, fill_char);
    auto buffer_data = buffer.data();
    auto it = buffer_data + len;
    while (value > 0 && it != buffer_data) {
        *--it = to_dec_digit(value % 10);
        value /= 10;
    }
}

BoolValue::BoolValue(SimpleTextItem *const element, const bool &data)
    : element(element)
    , data(data) {}

void BoolValue::update() {
    if (data.fetch_and_compare()) {
        if (data.get()) {
            element->setText(TRUE_STRING);
        } else {
            element->setText(FALSE_STRING);
        }
    }
}

PCValue::PCValue(SimpleTextItem *element, const machine::Address &data)
    : element(element)
    , data(data) {}

PCValue::PCValue(const PCValue &other)
    : QObject(other.parent())
    , element(other.element)
    , data(other.data) {}

void PCValue::clicked() {
    emit jump_to_pc(data.get_direct());
}

void PCValue::update() {
    if (data.fetch_and_compare()) {
        set_hex_string<2>(buffer, data.get().get_raw());
        element->setText(buffer);
    }
}

RegValue::RegValue(SimpleTextItem *element, const machine::RegisterValue &data)
    : element(element)
    , data(data) {}

void RegValue::update() {
    if (data.fetch_and_compare()) {
        set_hex_string<0>(buffer, data.get().as_u32());
        element->setText(buffer);
    }
}

RegIdValue::RegIdValue(svgscene::SimpleTextItem *element, const machine::RegisterId &data)
    : element(element)
    , data(data) {}

void RegIdValue::update() {
    if (data.fetch_and_compare()) {
        set_dec_value(buffer, data.get(), '0');
        element->setText(buffer);
    }
}

DebugValue::DebugValue(SimpleTextItem *element, const unsigned int &data)
    : element(element)
    , data(data) {}

void DebugValue::update() {
    if (data.fetch_and_compare()) {
        set_dec_value(buffer, data.get());
        element->setText(buffer);
    }
}
MultiTextValue::MultiTextValue(SimpleTextItem *const element, Data data)
    : element(element)
    , current_text_index(data.first)
    , text_table(data.second)
    , originalBrush(element->brush()) {}

void MultiTextValue::update() {
    if (current_text_index != 0) {
        // Highlight non-default value.
        element->setBrush(Qt::red);
    } else {
        element->setBrush(originalBrush);
    }
    element->setText(text_table.at(current_text_index));
}

InstructionValue::InstructionValue(SimpleTextItem *const element, Data data)
    : element(element)
    , instruction_data(data.first)
    , address_data(data.second) {}

void InstructionValue::update() {
    bool inst_changed = instruction_data.fetch_and_compare();
    bool addr_changed = address_data.fetch_and_compare();
    // Both must be updated.
    if (inst_changed || addr_changed) {
        element->setText(instruction_data.get().to_str(address_data.get()));
    }
}
