/**
 * Utility to parse and encode binary encoded fields.
 *
 * Use BitField if the field is guaranteed to be continuous.
 * Use SplitBitField if the field is not guaranteed to be continuous.
 *
 * @file
 */

#ifndef QTRVSIM_BITFIELD_H
#define QTRVSIM_BITFIELD_H

#include "common/containers/cvector.h"

#include <cstdint>

struct BitField {
    uint8_t count;
    uint8_t offset;

    template<typename T>
    [[nodiscard]] T decode(T val) const {
        return (val >> offset) & (((uint64_t)1 << count) - 1);
    }
    template<typename T>
    [[nodiscard]] T encode(T val) const {
        return ((val & (((uint64_t)1 << count) - 1)) << offset);
    }
    [[nodiscard]] uint64_t mask() const {
        return (((uint64_t)1 << count) - 1) << offset;
    }
};

template<size_t MAX_FIELD_PARTS>
struct SplitBitField {
    cvector<BitField, MAX_FIELD_PARTS> fields;
    size_t shift = 0;

    [[nodiscard]] typename decltype(fields)::const_iterator begin() const {
        return fields.cbegin();
    }
    [[nodiscard]] typename decltype(fields)::const_iterator end() const { return fields.cend(); }
    [[nodiscard]] uint32_t decode(uint32_t ins) const {
        uint32_t ret = 0;
        size_t offset = 0;
        for (BitField field : *this) {
            ret |= field.decode(ins) << offset;
            offset += field.count;
        }
        return ret << shift;
    }
    [[nodiscard]] uint32_t encode(uint32_t imm) const {
        uint32_t ret = 0;
        imm >>= shift;
        for (BitField field : *this) {
            ret |= field.encode(imm);
            imm >>= field.count;
        }
        return ret;
    }
};

#endif // QTRVSIM_BITFIELD_H
