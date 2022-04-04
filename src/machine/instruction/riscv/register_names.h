#ifndef QTRVSIM_REGISTER_NAMES_H
#define QTRVSIM_REGISTER_NAMES_H

#include "common/containers/constexpr/carray.h"
#include "common/containers/constexpr/cstring.h"

namespace machine {
static constexpr carray<cstring, 32> register_names = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
};
}

#endif // QTRVSIM_REGISTER_NAMES_H
