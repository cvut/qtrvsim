/**
 * Manual memory ownership toolkit.
 */
#ifndef QTRVSIM_MEMORY_OWNERSHIP_H
#define QTRVSIM_MEMORY_OWNERSHIP_H

#include <QScopedPointer>

/**
 * Tag for pointer owned by someone else. It is recommended to mention owner
 * in comment to make lifetimes manually verifiable.
 */
#define BORROWED

/**
 * Pointer with static lifetime.
 */
#define STATIC

/**
 * Pointer is owned and managed by Qt.
 */
#define QT_OWNED

/**
 * Owned pointer deallocated by automatic destructor.
 */
template<typename T>
using Box = QScopedPointer<T>;

#endif // QTRVSIM_MEMORY_OWNERSHIP_H
