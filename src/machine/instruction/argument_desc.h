#ifndef QTRVSIM_ARGUMENT_DESC_H
#define QTRVSIM_ARGUMENT_DESC_H

#include "bit_arg.h"
#include "register_value.h"

#include <cstdint>

namespace machine {

struct ArgumentDesc {
    char name;
    /**
     * Possible values:
     *  @val g: gp register id
     *  @val n: numeric immediate
     *  @val a: pc relative address offset
     *  @val b: pc relative address offset
     *  @val o: offset immediate
     */
    char kind;
    int64_t min;
    int64_t max;
    InstructionField arg;

    constexpr inline ArgumentDesc(
        char name,
        char kind,
        int64_t min,
        int64_t max,
        InstructionField arg)
        : name(name)
        , kind(kind)
        , min(min)
        , max(max)
        , arg(arg) {}

    /** Check whether given value fits into this instruction field. */
    constexpr bool is_value_in_field_range(RegisterValue val) const {
        if (min < 0) {
            return val.as_i64() <= max && val.as_i64() >= min;
        } else {
            return val.as_u64() <= static_cast<uint64_t>(max)
                   && val.as_u64() >= static_cast<uint64_t>(min);
        }
    }

    constexpr bool is_imm() const { return kind != 'g'; }
};

} // namespace machine

#endif // QTRVSIM_ARGUMENT_DESC_H
