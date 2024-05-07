#ifndef QTRVSIM_STRING_UTILS_H
#define QTRVSIM_STRING_UTILS_H

#include <QString>

namespace str {
template<typename T>
QString asHex(T number) {
    if (number < 0) {
        return QString::asprintf("-0x%x", -number);
    } else {
        return QString::asprintf("0x%x", number);
    }
}
} // namespace str

#endif // QTRVSIM_STRING_UTILS_H
