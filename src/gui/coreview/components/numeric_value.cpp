#include "numeric_value.h"

NumericValue::NumericValue(
    BORROWED svgscene::SimpleTextItem *element,
    u64_getter getter,
    unsigned int text_width_chars,
    unsigned int base,
    QChar fill_char)
    : element(element)
    , getter(getter)
    , text_width_chars(text_width_chars)
    , base(base)
    , fill_char(fill_char) {}

void NumericValue::update() {
    element->setText(QString("%1").arg(getter(), text_width_chars, base, fill_char));
}
