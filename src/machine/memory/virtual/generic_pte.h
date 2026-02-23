#ifndef GENERIC_PTE_H
#define GENERIC_PTE_H

#include <cstdint>

// Abstract parent interface for PTEs. Concrete PTE types (Sv32, Sv39, ...) implement these methods.
namespace machine {
static constexpr uint64_t PHYS_PPN_START = 0x200; // I have noticed that programs are loaded into
                                                  // memory starting at 0x200.

class Address;

struct GenericPte {
    uint64_t raw = 0;
    GenericPte() = default;
    explicit GenericPte(uint64_t r) : raw(r) {}
    virtual ~GenericPte() = default;

    // Raw accessor
    virtual uint64_t to_uint() const noexcept { return raw; }

    template<typename Derived>
    static Derived from_uint(uint64_t r) {
        static_assert(
            std::is_constructible<Derived, uint64_t>::value,
            "Derived must be constructible from uint64_t");
        return Derived(r);
    }

    // Flag accessors
    virtual bool v() const noexcept = 0;
    virtual bool r() const noexcept = 0;
    virtual bool w() const noexcept = 0;
    virtual bool x() const noexcept = 0;
    virtual bool u() const noexcept = 0;
    virtual bool g() const noexcept = 0;
    virtual bool a() const noexcept = 0;
    virtual bool d() const noexcept = 0;

    // PPN extraction
    virtual uint64_t ppn() const noexcept = 0;

    // Convenience
    virtual bool is_leaf() const noexcept = 0;
    virtual bool is_valid() const noexcept = 0;

    // Construct the physical address for a leaf PTE given the full VA and the level where
    // the leaf was found (level numbering: top .. 0). Each subclass implements layout-specific
    // logic.
    virtual Address make_phys(uint64_t va_raw, int level) const = 0;
};

} // namespace machine

#endif // GENERIC_PTE_H
