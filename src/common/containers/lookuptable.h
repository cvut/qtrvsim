#ifndef QTRVSIM_LOOKUPTABLE_H
#define QTRVSIM_LOOKUPTABLE_H

#include "common/containers/constexpr/carray.h"

#include <limits>

/** Constructs lookup table from list at compile time. */
template<typename Key, typename Val>
class LookUpTable {
    carray<const Val *, 1 << std::numeric_limits<Key>::digits> table;

public:
    /**
     * Constructs lookup table from iterable container (list) and pointer to member of single
     * list item which extracts the key used for lookup.
     */
    template<typename T>
    LookUpTable(T &data, Key Val::*key) {
        for (const Val &val : data) {
            table[val.*key] = std::addressof(val);
        }
    }

    constexpr const Val *operator[](size_t index) const { return table[index]; }
};

#endif // QTRVSIM_LOOKUPTABLE_H
