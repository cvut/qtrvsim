#ifndef QTRVSIM_QSTRING_HASH_H
#define QTRVSIM_QSTRING_HASH_H

#include <QHash>
#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)

namespace std {
template<>
struct hash<QString> {
    std::size_t operator()(const QString &s) const noexcept {
        return qHash(s);
    }
};
    #if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)

template<>
struct hash<QStringView> {
    std::size_t operator()(const QStringView &s) const noexcept {
        return qHash(s);
    }
};
    #endif

} // namespace std

    #if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
// Supported since Qt5.10
using QStringView = QString;
    #endif

#endif

#endif // QTRVSIM_QSTRING_HASH_H
