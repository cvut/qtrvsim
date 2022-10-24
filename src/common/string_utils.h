#ifndef QTRVSIM_STRING_UTILS_H
#define QTRVSIM_STRING_UTILS_H

#include <QString>

namespace str {
template<typename T>
QString asHex(T number) {
    return QString("0x%1").arg(number, 0, 16);
}
} // namespace str

#endif // QTRVSIM_STRING_UTILS_H
