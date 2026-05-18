#include "utilandtext.h"
using namespace machine;

/* TODO: Decide whether this should be moved to machine to avoid duplication or kept aside as
         a visualization concern */
const char *get_exception_name(ExceptionCause exception_cause) {
    switch (exception_cause) {
    case EXCAUSE_NONE: return "NONE";
    case EXCAUSE_INSN_FAULT: return "INSN_FAULT";
    case EXCAUSE_INSN_ILLEGAL: return "INSN_ILLEGAL";
    case EXCAUSE_BREAK: return "BREAK";
    case EXCAUSE_LOAD_MISALIGNED: return "LOAD_MISALIGNED";
    case EXCAUSE_LOAD_FAULT: return "LOAD_FAULT";
    case EXCAUSE_STORE_MISALIGNED: return "STORE_MISALIGNED";
    case EXCAUSE_STORE_FAULT: return "STORE_FAULT";
    case EXCAUSE_ECALL_U: return "ECALL_U";
    case EXCAUSE_ECALL_S: return "ECALL_S";
    case EXCAUSE_RESERVED_10: return "RESERVED_10";
    case EXCAUSE_ECALL_M: return "ECALL_M";
    case EXCAUSE_INSN_PAGE_FAULT: return "INSN_PAGE_FAULT";
    case EXCAUSE_LOAD_PAGE_FAULT: return "LOAD_PAGE_FAULT";
    case EXCAUSE_RESERVED_14: return "RESERVED_14";
    case EXCAUSE_STORE_PAGE_FAULT: return "STORE_PAGE_FAULT";
    // Simulator specific exception cause codes, alliases
    case EXCAUSE_HWBREAK: return "HWBREAK";
    case EXCAUSE_ECALL_ANY: return "ECALL_ANY";
    case EXCAUSE_INT_M: return "INT_M";
    case EXCAUSE_INT_S: return "INT_S";
    default: UNREACHABLE
    }
}

const char *get_privilege_level_name(machine::CSR::PrivilegeLevel level) {
    switch (level) {
    case CSR::PrivilegeLevel::UNPRIVILEGED: return "UNPRIVILEGED";
    case CSR::PrivilegeLevel::SUPERVISOR: return "SUPERVISOR";
    case CSR::PrivilegeLevel::HYPERVISOR: return "HYPERVISOR";
    case CSR::PrivilegeLevel::MACHINE: return "MACHINE";
    default: UNREACHABLE
    }
}
