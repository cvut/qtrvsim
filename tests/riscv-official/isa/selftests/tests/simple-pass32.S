# See LICENSE for license details.

#include "riscv_test.h"
#undef RVTEST_RV64U
#define RVTEST_RV64U RVTEST_RV32U

#include "simple-pass64.S"
