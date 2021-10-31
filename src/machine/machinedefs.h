#ifndef MACHINEDEFS_H
#define MACHINEDEFS_H

#include "memory/address.h"

#include <cstdint>
#include <qmetatype.h>

namespace machine {

enum AccessControl {
    AC_NONE,
    AC_I8,
    AC_U8,
    AC_I16,
    AC_U16,
    AC_I32,
    AC_U32,
    AC_I64,
    AC_U64,
    AC_LOAD_LINKED,
    AC_STORE_CONDITIONAL,
    AC_WORD_RIGHT,
    AC_WORD_LEFT,
    AC_CACHE_OP,
};

constexpr AccessControl AC_FIRST_REGULAR = AC_I8;
constexpr AccessControl AC_LAST_REGULAR = AC_U64;
constexpr AccessControl AC_FIRST_SPECIAL = AC_LOAD_LINKED;
constexpr AccessControl AC_LAST_SPECIAL = AC_CACHE_OP;

constexpr bool is_regular_access(AccessControl type) {
    return AC_FIRST_REGULAR <= type and type <= AC_LAST_REGULAR;
}

constexpr bool is_special_access(AccessControl type) {
    return AC_FIRST_SPECIAL <= type and type <= AC_LAST_SPECIAL;
}
static_assert(is_special_access(AC_CACHE_OP), "");
static_assert(is_special_access((AccessControl)13), "");

enum ExceptionCause {
    EXCAUSE_NONE = 0, // Use zero as default value when no exception is
    // pending.
    EXCAUSE_INT = 1, // Int is 0 on real CPU and in Cause register.
    EXCAUSE_UNKNOWN = 2,
    EXCAUSE_ADDRL = 4,
    EXCAUSE_ADDRS = 5,
    EXCAUSE_IBUS = 6,
    EXCAUSE_DBUS = 7,
    EXCAUSE_SYSCALL = 8,
    EXCAUSE_BREAK = 9,
    EXCAUSE_OVERFLOW = 12,
    EXCAUSE_TRAP = 13,
    EXCAUSE_HWBREAK = 14,
    EXCAUSE_COUNT = 15,
};

enum LocationStatus {
    LOCSTAT_NONE = 0,
    LOCSTAT_CACHED = 1 << 0,
    LOCSTAT_DIRTY = 1 << 1,
    LOCSTAT_READ_ONLY = 1 << 2,
    LOCSTAT_ILLEGAL = 1 << 3,
};

const Address STAGEADDR_NONE = 0xffffffff_addr;
} // namespace machine

Q_DECLARE_METATYPE(machine::AccessControl)

#endif // MACHINEDEFS_H
