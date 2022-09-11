#define _ENV_PHYSICAL_SINGLE_CORE_H

#include "../../riscv-tests/env/encoding.h"

//-----------------------------------------------------------------------
// Begin Macro
//-----------------------------------------------------------------------

#define RVTEST_RV64U                                                    \
  .macro init;                                                          \
  .endm

#define RVTEST_RV32U                                                    \
  .macro init;                                                          \
  .endm

#define RVTEST_RV64M                                                    \
  .macro init;                                                          \
  RVTEST_ENABLE_MACHINE;                                                \
  .endm

#define RVTEST_RV32M                                                    \
  .macro init;                                                          \
  RVTEST_ENABLE_MACHINE;                                                \
  .endm

#define RVTEST_RV64S                                                    \
  .macro init;                                                          \
  RVTEST_ENABLE_SUPERVISOR;                                             \
  .endm

#define RVTEST_RV32S                                                    \
  .macro init;                                                          \
  RVTEST_ENABLE_SUPERVISOR;                                             \
  .endm

#if __riscv_xlen == 64
# define CHECK_XLEN li a0, 1; slli a0, a0, 31; bgez a0, 1f; RVTEST_ARCH_CHECK_FAIL; 1:
#else
# define CHECK_XLEN li a0, 1; slli a0, a0, 31; bltz a0, 1f; RVTEST_ARCH_CHECK_FAIL; 1:
#endif

#define INIT_XREG                                                       \
  li x1, 0;                                                             \
  li x2, 0;                                                             \
  li x3, 0;                                                             \
  li x4, 0;                                                             \
  li x5, 0;                                                             \
  li x6, 0;                                                             \
  li x7, 0;                                                             \
  li x8, 0;                                                             \
  li x9, 0;                                                             \
  li x10, 0;                                                            \
  li x11, 0;                                                            \
  li x12, 0;                                                            \
  li x13, 0;                                                            \
  li x14, 0;                                                            \
  li x15, 0;                                                            \
  li x16, 0;                                                            \
  li x17, 0;                                                            \
  li x18, 0;                                                            \
  li x19, 0;                                                            \
  li x20, 0;                                                            \
  li x21, 0;                                                            \
  li x22, 0;                                                            \
  li x23, 0;                                                            \
  li x24, 0;                                                            \
  li x25, 0;                                                            \
  li x26, 0;                                                            \
  li x27, 0;                                                            \
  li x28, 0;                                                            \
  li x29, 0;                                                            \
  li x30, 0;                                                            \
  li x31, 0;

#define RVTEST_CODE_BEGIN                                               \
        .section .text.init;                                            \
        .align  6;                                                      \
        .globl _start;                                                  \
_start:                                                                 \
    INIT_XREG;                                                          \
    CHECK_XLEN;                                                         \
    .align 2;

//-----------------------------------------------------------------------
// End Macro
//-----------------------------------------------------------------------

#define RVTEST_CODE_END                                                 

//-----------------------------------------------------------------------
// Pass/Fail Macro
//-----------------------------------------------------------------------

#define RVTEST_PASS                                                     \
        li a1, 0x60;                                                    \
        slli a1, a1, 8;                                                 \
        addi a1, a1, 0xd;                                               \
        slli a1, a1, 12;                                                \
        ecall

#define TESTNUM gp

#define RVTEST_FAIL                                                     \
        addi a1,a1,0xBA;                                                \
        slli a1,a1,4;                                                   \
        addi a1, a1, 0xD;                                               \
        slli a1, a1, 16;                                                \
        ecall

#define RVTEST_ARCH_CHECK_FAIL                                          \
        addi a1,a1,0xAC;                                                \
        slli a1,a1,4;                                                   \
        addi a1, a1, 0xF;                                               \
        slli a1, a1, 16;                                                \
        ecall

//-----------------------------------------------------------------------
// Data Section Macro
//-----------------------------------------------------------------------

#define EXTRA_DATA
#define RVTEST_DATA_BEGIN
#define RVTEST_DATA_END
