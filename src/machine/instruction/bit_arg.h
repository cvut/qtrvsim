#ifndef QTRVSIM_BIT_ARG_H
#define QTRVSIM_BIT_ARG_H

#include "common/containers/constexpr/carray.h"
#include "common/containers/constexpr/cvector.h"

namespace machine {

/**
 * Defines encoding of instruction field into binary code.
 */
struct InstructionField {
    /** Defines continuous part of argument in instruction code. */
    struct Subfield {
        constexpr inline Subfield(uint8_t count, uint8_t offset) : count(count), offset(offset) {}
        constexpr inline Subfield() : count(0), offset(0) {};
        constexpr inline Subfield(const Subfield &) = default;
        constexpr inline Subfield &operator=(const Subfield &) = default;

        template<typename T>
        constexpr T decode(T val) const {
            return (val >> offset) & ((1L << count) - 1);
        }
        template<typename T>
        constexpr T encode(T val) const {
            return ((val & ((1L << count) - 1)) << offset);
        }

        // Members
        uint8_t count;
        uint8_t offset;
    };

    // Based on currently supported instructions.
    static constexpr size_t MAX_FIELDS_PER_INSTRUCTION = 5;
    using Subfields = cvector<Subfield, MAX_FIELDS_PER_INSTRUCTION>;
    /** Type of raw instruction code. */
    using code_type = uint32_t;

    constexpr InstructionField( // NOLINT(google-explicit-constructor)
        const Subfields fields,
        size_t shift = 0)
        : fields(fields)
        , shift(shift)
        , bits_used(count_bits_used()) {}

    Subfields::const_iterator begin() const { return fields.begin(); }
    Subfields::const_iterator end() const { return fields.end(); }

    constexpr code_type decode(uint32_t ins) const {
        code_type ret = 0;
        size_t offset = 0;
        for (const Subfield field : fields) {
            ret |= field.decode(ins) << offset;
            offset += field.count;
        }
        return ret << shift;
    }
    constexpr code_type encode(uint32_t imm) const {
        code_type ret = 0;
        imm >>= shift;
        for (Subfield field : fields) {
            ret |= field.encode(imm);
            imm >>= field.count;
        }
        return ret;
    }

    // Members
    Subfields fields;
    size_t shift;
    size_t bits_used;

private:
    /** Total number of bits used in instruction by this arg. */
    constexpr size_t count_bits_used() const {
        size_t count = 0;
        for (const Subfield &field : fields) {
            count += field.count;
        }
        return count;
    }
};
} // namespace machine

#endif // QTRVSIM_BIT_ARG_H
