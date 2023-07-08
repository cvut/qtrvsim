#include "value_handlers.h"

#include <QGraphicsPathItem>

using svgscene::SimpleTextItem;

const QString BoolValue::COMPONENT_NAME = QStringLiteral("bool-value");
const QString PCValue::COMPONENT_NAME = QStringLiteral("pc-value");
const QString RegValue::COMPONENT_NAME = QStringLiteral("reg-value");
const QString RegIdValue::COMPONENT_NAME = QStringLiteral("reg-id-value");
const QString DebugValue::COMPONENT_NAME = QStringLiteral("debug-value");
const QString MultiTextValue::COMPONENT_NAME = QStringLiteral("multi-text-value");
const QString InstructionValue::COMPONENT_NAME = QStringLiteral("instruction-value");

BoolValue::BoolValue(SimpleTextItem *const element, const bool &data)
    : element(element)
    , data(data) {}

void BoolValue::update() {
    element->setText(data ? QStringLiteral("1") : QStringLiteral("0"));
}

PCValue::PCValue(SimpleTextItem *element, const machine::Address &data)
    : element(element)
    , data(data) {}

PCValue::PCValue(const PCValue &other)
    : QObject(other.parent())
    , element(other.element)
    , data(other.data) {}

void PCValue::clicked() {
    emit jump_to_pc(data);
}

void PCValue::update() {
    element->setText(QString("0x%1").arg(data.get_raw(), 8, 16, QChar('0')));
}

RegValue::RegValue(SimpleTextItem *element, const machine::RegisterValue &data)
    : element(element)
    , data(data) {}

void RegValue::update() {
    element->setText(QString("%1").arg(data.as_u32(), 8, 16, QChar('0')));
}

RegIdValue::RegIdValue(svgscene::SimpleTextItem *element, const machine::RegisterId &data)
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
    element->setText(instruction_data.to_str(address_data));
}
