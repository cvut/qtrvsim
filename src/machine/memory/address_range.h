#ifndef ADDRESS_RANGE_H
#define ADDRESS_RANGE_H

#include "utils.h"

#include <QMetaType>
#include <cstdint>

#include "memory/address.h"


namespace machine {

/**
 * Physical memory range - for nou used for range reservation for AMO operations
 *
 * If the first and the last point to the same address then range represnets
 * exaclly one valid byte. If the last is smaller than the first then range
 * is empty.
 */
class AddressRange {
public:
    Address first; //> The first valid location of the range
    Address last; //> The last valid location of the range

    /**
     *  Default constructor results for empty range.
     */
    constexpr AddressRange() : first(1), last(0) {};

    constexpr AddressRange(const AddressRange &range) = default;    //> Copy constructor
    constexpr AddressRange(const Address &afirst, const Address &alast) :
               first(afirst), last(alast) {};
    constexpr AddressRange(const Address &asingleAddr) :
               first(asingleAddr), last(asingleAddr) {};
    constexpr AddressRange &operator=(const AddressRange &address) = default; //> Assign constructor

    /**
     * Is empty range
     */
    constexpr bool is_empty() const { return first > last; };

    /* Eq */
    constexpr inline bool operator==(const AddressRange &other) const {
        return (first == other.first && last == other.last) ||
               (is_empty() && other.is_empty());
    };
    constexpr inline bool operator!=(const AddressRange &other) const {
        return !((first == other.first && last == other.last) ||
               (is_empty() && other.is_empty()));
    };

    constexpr bool within(const AddressRange &other) const {
        return (first >= other.first) && (last <= other.last);
    };

    constexpr bool contains(const AddressRange &other) const {
        return (first <= other.first) && (last >= other.last);
    };

    constexpr bool overlaps(const AddressRange &other) const {
        return (first <= other.last) && (last >= other.first) &&
                !is_empty() && !other.is_empty();
    };

    void reset() {
        first = Address(1);
        last = Address(0);
    }
};

} // namespace machine

Q_DECLARE_METATYPE(machine::AddressRange)

#endif // ADDRESS_RANGE_H
