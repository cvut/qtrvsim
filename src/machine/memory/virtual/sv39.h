#ifndef SV39_H
#define SV39_H

#include "generic_pte.h"

// SV39-specific definitions: page-table entry (PTE) bitfields, shifts/masks, and PTE to physical
// address helpers. This header documents the SV39 layout (RISC-V 64-bit virtual memory):
//  - Page size: 4 KiB  (PAGE_SHIFT = 12).
//  - Virtual page number (VPN) is split into three 9-bit indices VPN[2], VPN[1], VPN[0].
//  - PTE low bits encode flags and low-order info; high bits encode the physical
//    page number (PPN).
namespace machine {

// Sv39Pte wraps the raw 64-bit PTE value and provides helpers to read flags and fields.
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
// 10..53 PPN (physical page number: 44 bits)
// 54..63 reserved for software/higher bits
//
// A PTE is considered a "leaf" when it grants read or execute permission (R or X).
// Validation rules: PTE is valid if V==1 and (if W==1 then R must also be 1).
struct Sv39Pte : public GenericPte {
    static constexpr unsigned PAGE_SHIFT = 12; // Page size = 2^PAGE_SHIFT bytes. For SV39 this is 4
    // KiB.
    static constexpr unsigned VPN_BITS = 9; // Number of bits for each VPN level in SV39: 9 bits for
    // VPN[2], VPN[1] and VPN[0].

    // Shift values for extracting VPN parts from a virtual address:
    static constexpr unsigned VPN0_SHIFT = PAGE_SHIFT; // VPN0 is the low VPN field, it starts at
                                                       // the
    // page offset (PAGE_SHIFT).
    static constexpr unsigned VPN1_SHIFT = PAGE_SHIFT + VPN_BITS; // VPN1 is the next field above
                                                                  // VPN0.
    static constexpr unsigned VPN2_SHIFT = PAGE_SHIFT + VPN_BITS * 2; // VPN2 is the next field
                                                                      // above VPN1.

    static constexpr unsigned VPN_MASK = (1u << VPN_BITS) - 1; // Mask to extract a single VPN level
                                                               // (9
    // bits, value 0..511).

    // Number of bits available for the physical page number (PPN) in SV39 PTE.
    // SV39 encodes a 44-bit PPN in a 64-bit PTE (PPN[0]=9 bits, PPN[1]=9 bits, PPN[2]=26 bits),
    // which permits addressing a large physical memory range.
    static constexpr unsigned PPN_BITS = 44;
    static constexpr std::uint64_t PPN_MASK = ((std::uint64_t(1) << PPN_BITS) - 1u);

    // SATP constants for Sv39
    static constexpr uint64_t SATP_MODE_MASK = (0xFull << 60);
    static constexpr uint64_t SATP_PPN_MASK = (1ull << 44) - 1ull;

    using RawType = uint64_t;

    Sv39Pte() = default;
    explicit Sv39Pte(uint64_t raw_) : GenericPte(raw_) {}

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
    static constexpr uint64_t V_MASK = (1ull << V_SHIFT);
    static constexpr uint64_t R_MASK = (1ull << R_SHIFT);
    static constexpr uint64_t W_MASK = (1ull << W_SHIFT);
    static constexpr uint64_t X_MASK = (1ull << X_SHIFT);
    static constexpr uint64_t U_MASK = (1ull << U_SHIFT);
    static constexpr uint64_t G_MASK = (1ull << G_SHIFT);
    static constexpr uint64_t A_MASK = (1ull << A_SHIFT);
    static constexpr uint64_t D_MASK = (1ull << D_SHIFT);

    // Mask for the 2-bit RSW field (bits 8..9).
    static constexpr uint64_t RSW_MASK = (0x3ull << RSW_SHIFT);

    // Mask that selects the PPN field within the raw PTE (bits 10..(10 + PPN_BITS - 1))
    static constexpr std::uint64_t PPN_MASK64 = (PPN_MASK << PPN_SHIFT);

    static Sv39Pte from_uint(uint64_t r) { return Sv39Pte(r); }
    uint64_t to_uint() const noexcept override { return raw; }

    // Flag accessors
    bool v() const noexcept override { return (raw >> V_SHIFT) & 0x1ull; }
    bool r() const noexcept override { return (raw >> R_SHIFT) & 0x1ull; }
    bool w() const noexcept override { return (raw >> W_SHIFT) & 0x1ull; }
    bool x() const noexcept override { return (raw >> X_SHIFT) & 0x1ull; }
    bool u() const noexcept override { return (raw >> U_SHIFT) & 0x1ull; }
    bool g() const noexcept override { return (raw >> G_SHIFT) & 0x1ull; }
    bool a() const noexcept override { return (raw >> A_SHIFT) & 0x1ull; }
    bool d() const noexcept override { return (raw >> D_SHIFT) & 0x1ull; }
    uint64_t rsw() const noexcept { return (raw >> RSW_SHIFT) & 0x3ull; }
    uint64_t ppn() const noexcept override { return (raw >> PPN_SHIFT) & PPN_MASK; }

    // Convenience methods used by the page-table walker
    bool is_leaf() const noexcept override { return r() || x(); }
    bool is_valid() const noexcept override { return v() && (!w() || r()); }

    // Helper to construct a PTE from fields.
    static Sv39Pte make(
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
        r |= ((rsw_ & 0x3ull) << RSW_SHIFT);
        r |= ((ppn_ & PPN_MASK) << PPN_SHIFT);
        return Sv39Pte(r);
    }
    Address make_phys(uint64_t va_raw, int level) const override;
};

inline Address Sv39Pte::make_phys(uint64_t va_raw, int level) const {
    // Offset within the page
    uint64_t offset = va_raw & ((1ull << PAGE_SHIFT) - 1ull);
    // Physical page number as encoded in the PTE
    uint64_t phys_ppn = ppn();

    // If this PTE is a leaf at a level > 0 it describes a superpage. The low PPN bits
    // (corresponding to VPN[0..level-1]) are taken from the virtual address's VPN fields
    // and replace the corresponding low bits of the PTE's PPN.
    if (level > 0) {
        // Build a combined value from VPN[0]..VPN[level-1]
        uint64_t vpn_combined = 0;
        for (int i = 0; i < level; ++i) {
            uint64_t vpn_i = (va_raw >> (PAGE_SHIFT + VPN_BITS * i)) & VPN_MASK;
            vpn_combined |= (vpn_i << (VPN_BITS * i));
        }
        // Mask selecting the low (level * VPN_BITS) bits
        uint64_t low_bits_mask = (1ull << (VPN_BITS * level)) - 1ull;
        phys_ppn = (phys_ppn & ~low_bits_mask) | (vpn_combined & low_bits_mask);
    }

    return Address { (phys_ppn << PAGE_SHIFT) | offset };
}
} // namespace machine

#endif // SV39_H