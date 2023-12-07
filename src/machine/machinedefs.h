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
    AC_LR32,
    AC_SC32,
    AC_AMOSWAP32,
    AC_AMOADD32,
    AC_AMOXOR32,
    AC_AMOAND32,
    AC_AMOOR32,
    AC_AMOMIN32,
    AC_AMOMAX32,
    AC_AMOMINU32,
    AC_AMOMAXU32,
    AC_LR64,
    AC_SC64,
    AC_AMOSWAP64,
    AC_AMOADD64,
    AC_AMOXOR64,
    AC_AMOAND64,
    AC_AMOOR64,
    AC_AMOMIN64,
    AC_AMOMAX64,
    AC_AMOMINU64,
    AC_AMOMAXU64,
    AC_CACHE_OP,
};

constexpr AccessControl AC_FIRST_REGULAR = AC_I8;
constexpr AccessControl AC_LAST_REGULAR = AC_U64;
constexpr AccessControl AC_FIRST_SPECIAL = AC_LR32;
constexpr AccessControl AC_LAST_SPECIAL = AC_CACHE_OP;
constexpr AccessControl AC_FISRT_AMO_MODIFY32 = AC_AMOSWAP32;
constexpr AccessControl AC_LAST_AMO_MODIFY32 = AC_AMOMAXU32;
constexpr AccessControl AC_FISRT_AMO_MODIFY64 = AC_AMOSWAP64;
constexpr AccessControl AC_LAST_AMO_MODIFY64 = AC_AMOMAXU64;

constexpr bool is_regular_access(AccessControl type) {
    return AC_FIRST_REGULAR <= type and type <= AC_LAST_REGULAR;
}

constexpr bool is_special_access(AccessControl type) {
    return AC_FIRST_SPECIAL <= type and type <= AC_LAST_SPECIAL;
}
static_assert(is_special_access(AC_CACHE_OP), "");
static_assert(is_special_access((AccessControl)11), "");

enum ExceptionCause {
    EXCAUSE_NONE = 0, // Use zero as default value when no exception is
    // ECAUSE_INSN_MISALIGNED - not defined for now, overlaps with EXCAUSE_NON
    EXCAUSE_INSN_FAULT = 1,        // Instruction access fault
    EXCAUSE_INSN_ILLEGAL = 2,      // Illegal instruction
    EXCAUSE_BREAK = 3,             // Breakpoint
    EXCAUSE_LOAD_MISALIGNED = 4,   // Load address misaligned
    EXCAUSE_LOAD_FAULT = 5,        // Load access fault
    EXCAUSE_STORE_MISALIGNED = 6,  // Store/AMO address misaligned
    EXCAUSE_STORE_FAULT = 7,       // Store/AMO access fault
    EXCAUSE_ECALL_U = 8,           // Environment call from U-mode
    EXCAUSE_ECALL_S = 9,           // Environment call from S-mode
    EXCAUSE_RESERVED_10 = 10,      // Reserved
    EXCAUSE_ECALL_M = 11,          // Environment call from M-mode
    EXCAUSE_INSN_PAGE_FAULT = 12,  // Instruction page fault
    EXCAUSE_LOAD_PAGE_FAULT = 13,  // Load page fault
    EXCAUSE_RESERVED_14 = 14,      // Reserved
    EXCAUSE_STORE_PAGE_FAULT = 15, // Store/AMO page fault
    // Simulator specific exception cause codes, alliases
    EXCAUSE_HWBREAK = 16,
    EXCAUSE_ECALL_ANY = 17,        // sythetic exception to mark ECALL instruction
    EXCAUSE_INT = 18,              // External/asynchronous interrupt, bit 32 or 63
    EXCAUSE_COUNT = 19,
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
