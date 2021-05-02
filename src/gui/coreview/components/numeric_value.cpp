#include "numeric_value.h"

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
