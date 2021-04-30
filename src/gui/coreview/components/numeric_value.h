#ifndef QTMIPS_NUMERIC_VALUE_H
#define QTMIPS_NUMERIC_VALUE_H

#include <svgscene/components/simpletextitem.h>
#include <svgscene/utils/memory_ownership.h>

typedef uint64_t (*u64_getter)();

class NumericValue : public QObject {
public:
    explicit NumericValue(
        BORROWED svgscene::SimpleTextItem *element,
        u64_getter getter,
        unsigned int text_width_chars = 8,
        unsigned int base = 16,
        QChar fill_char = '0');

    void update();

private:
    BORROWED svgscene::SimpleTextItem *element;
    u64_getter getter;
    const uint8_t text_width_chars;
    const uint8_t base;
    const QChar fill_char;
};

#endif // QTMIPS_NUMERIC_VALUE_H
