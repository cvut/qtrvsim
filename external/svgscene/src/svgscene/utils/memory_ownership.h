/**
 * Manual memory ownership toolkit.
 */
#pragma once

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
 * Owned pointer deallocated by automatic destructor.
 */
template<typename T>
using Box = QScopedPointer<T>;