#ifndef SV32_H
#define SV32_H

#include "generic_pte.h"

// SV32-specific definitions: page-table entry (PTE) bitfields, shifts/masks, and PTE to physical
// address helpers. This header documents the SV32 layout (RISC-V 32-bit virtual memory):
//  - Page size: 4 KiB  (PAGE_SHIFT = 12).
//  - Virtual page number (VPN) is split into two 10-bit indices VPN[1] and VPN[0].
//  - PTE low bits encode flags and low-order info; high bits encode the physical
//    page number (PPN).
namespace machine {

// Sv32Pte wraps the raw 32-bit PTE value and provides helpers to read flags and fields.
// Layout (bit indices):
//  0 V (valid)
//  1 R (read)
//  2 W (write)
//  3 X (execute)
//  4 U (user)
//  5 G (global)
//  6 A (accessed)
//  7 D (dirty)
//  8..9 RSW (reserved for supervisor use)
//  10..31 PPN (physical page number)
//
// A PTE is considered a "leaf" when it grants read or execute permission (R or X).
// Validation rules: PTE is valid if V==1 and (if W==1 then R must also be 1).
struct Sv32Pte : public GenericPte {
    static constexpr unsigned PAGE_SHIFT = 12; // Page size = 2^PAGE_SHIFT bytes. For SV32 this is 4
    // KiB.
    static constexpr unsigned VPN_BITS = 10; // Number of bits for each VPN level in SV32: 10 bits
                                             // for
    // VPN[1] and 10 bits for VPN[0].

    // Shift values for extracting VPN parts from a virtual address:
    static constexpr unsigned VPN0_SHIFT = PAGE_SHIFT; // VPN0 is the low VPN field, it starts at
                                                       // the
    // page offset (PAGE_SHIFT).
    static constexpr unsigned VPN1_SHIFT = PAGE_SHIFT + VPN_BITS; // VPN1 is the next field above
                                                                  // VPN0.

    static constexpr unsigned VPN_MASK = (1u << VPN_BITS) - 1; // Mask to extract a single VPN level
                                                               // (10
    // bits, value 0..1023).

    // Number of bits available for the physical page number (PPN) in SV32 PTE.
    // SV32 uses 22 PPN bits (bits 10..31 of a 32-bit PTE) which permits addressing up to 4GiB of
    // physical memory.
    static constexpr unsigned PPN_BITS = 22;
    static constexpr unsigned PPN_MASK = (1u << PPN_BITS) - 1;

    // SATP constants for Sv32
    static constexpr uint64_t SATP_MODE_MASK = (1ull << 31);
    static constexpr uint64_t SATP_PPN_MASK = (1ull << 22) - 1ull;

    using RawType = uint32_t;

    Sv32Pte() = default;
    explicit Sv32Pte(uint64_t raw_) : GenericPte(raw_) {}

    // Bit positions (shifts) for the fields described above.
    static constexpr unsigned V_SHIFT = 0;
    static constexpr unsigned R_SHIFT = 1;
    static constexpr unsigned W_SHIFT = 2;
    static constexpr unsigned X_SHIFT = 3;
    static constexpr unsigned U_SHIFT = 4;
    static constexpr unsigned G_SHIFT = 5;
    static constexpr unsigned A_SHIFT = 6;
    static constexpr unsigned D_SHIFT = 7;
    static constexpr unsigned RSW_SHIFT = 8;  // two bits: 8..9
    static constexpr unsigned PPN_SHIFT = 10; // PPN starts at bit 10

    // Masks for each single-bit flag
    static constexpr uint64_t V_MASK = (1u << V_SHIFT);
    static constexpr uint64_t R_MASK = (1u << R_SHIFT);
    static constexpr uint64_t W_MASK = (1u << W_SHIFT);
    static constexpr uint64_t X_MASK = (1u << X_SHIFT);
    static constexpr uint64_t U_MASK = (1u << U_SHIFT);
    static constexpr uint64_t G_MASK = (1u << G_SHIFT);
    static constexpr uint64_t A_MASK = (1u << A_SHIFT);
    static constexpr uint64_t D_MASK = (1u << D_SHIFT);

    // Mask for the 2-bit RSW field (bits 8..9).
    static constexpr uint64_t RSW_MASK = (0x3u << RSW_SHIFT);

    // Mask that selects the PPN field within the raw PTE (bits 10..(10 + PPN_BITS - 1))
    static constexpr uint64_t PPN_MASK32 = ((PPN_MASK) << PPN_SHIFT);

    static Sv32Pte from_uint(uint64_t r) { return Sv32Pte(r); }
    uint64_t to_uint() const noexcept override { return raw; }

    // Flag accessors
    bool v() const noexcept override { return (raw >> V_SHIFT) & 0x1u; }
    bool r() const noexcept override { return (raw >> R_SHIFT) & 0x1u; }
    bool w() const noexcept override { return (raw >> W_SHIFT) & 0x1u; }
    bool x() const noexcept override { return (raw >> X_SHIFT) & 0x1u; }
    bool u() const noexcept override { return (raw >> U_SHIFT) & 0x1u; }
    bool g() const noexcept override { return (raw >> G_SHIFT) & 0x1u; }
    bool a() const noexcept override { return (raw >> A_SHIFT) & 0x1u; }
    bool d() const noexcept override { return (raw >> D_SHIFT) & 0x1u; }
    uint64_t rsw() const noexcept { return (raw >> RSW_SHIFT) & 0x3u; }
    uint64_t ppn() const noexcept override { return (raw >> PPN_SHIFT) & PPN_MASK; }

    // Convenience methods used by the page-table walker
    bool is_leaf() const noexcept override { return r() || x(); }
    bool is_valid() const noexcept override { return v() && (!w() || r()); }

    // Helper to construct a PTE from fields.
    static Sv32Pte make(
        bool v_,
        bool r_,
        bool w_,
        bool x_,
        bool u_,
        bool g_,
        bool a_,
        bool d_,
        uint64_t rsw_,
        uint64_t ppn_) {
        uint64_t r = 0;
        r |= (uint64_t(v_) << V_SHIFT);
        r |= (uint64_t(r_) << R_SHIFT);
        r |= (uint64_t(w_) << W_SHIFT);
        r |= (uint64_t(x_) << X_SHIFT);
        r |= (uint64_t(u_) << U_SHIFT);
        r |= (uint64_t(g_) << G_SHIFT);
        r |= (uint64_t(a_) << A_SHIFT);
        r |= (uint64_t(d_) << D_SHIFT);
        r |= ((rsw_ & 0x3u) << RSW_SHIFT);
        r |= ((ppn_ & PPN_MASK) << PPN_SHIFT);
        return Sv32Pte(r);
    }
    Address make_phys(uint64_t va_raw, int level) const override;
};

inline Address Sv32Pte::make_phys(uint64_t va_raw, int level) const {
    uint64_t offset = va_raw & ((1ull << PAGE_SHIFT) - 1);
    uint64_t phys_ppn = ppn();

    if (level == 1) {
        uint64_t vpn0 = (va_raw >> PAGE_SHIFT) & VPN_MASK;
        phys_ppn = (phys_ppn & ~uint64_t(VPN_MASK)) | (vpn0 & VPN_MASK);
    }
    return Address { (phys_ppn << PAGE_SHIFT) | offset };
}
} // namespace machine

#endif // SV32_H
