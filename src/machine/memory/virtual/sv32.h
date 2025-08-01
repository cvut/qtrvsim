#ifndef SV32_H
#define SV32_H

// SV32-specific definitions: page-table entry (PTE) bitfields, shifts/masks, and PTE to physical address helpers.
namespace machine {
    static constexpr unsigned PAGE_SHIFT = 12;
    static constexpr unsigned VPN_BITS = 10;
    static constexpr unsigned VPN0_SHIFT = PAGE_SHIFT;
    static constexpr unsigned VPN1_SHIFT = PAGE_SHIFT + VPN_BITS;
    static constexpr unsigned VPN_MASK = (1u << VPN_BITS) - 1;
    static constexpr unsigned PPN_BITS = 22;
    static constexpr unsigned PPN_MASK = (1u << PPN_BITS) - 1;
    static constexpr uint32_t PHYS_PPN_START = 0x200;

    struct Sv32Pte {
        uint32_t v : 1;
        uint32_t r : 1;
        uint32_t w : 1;
        uint32_t x : 1;
        uint32_t u : 1;
        uint32_t g : 1;
        uint32_t a : 1;
        uint32_t d : 1;
        uint32_t rsw : 2;
        uint32_t ppn : PPN_BITS;

        constexpr uint32_t to_uint() const {
            return (ppn << 10) |
                (d << 7) |
                (a << 6) |
                (g << 5) |
                (u << 4) |
                (x << 3) |
                (w << 2) |
                (r << 1) |
                (v << 0);
        }

        static constexpr Sv32Pte from_uint(uint32_t raw) {
            Sv32Pte p{};
                p.v = (raw >> 0) & 0x1;
                p.r = (raw >> 1) & 0x1;
                p.w = (raw >> 2) & 0x1;
                p.x = (raw >> 3) & 0x1;
                p.u = (raw >> 4) & 0x1;
                p.g = (raw >> 5) & 0x1;
                p.a = (raw >> 6) & 0x1;
                p.d = (raw >> 7) & 0x1;
                p.rsw = (raw >> 8) & 0x3;
                p.ppn = (raw >> 10) & PPN_MASK;
                return p;
            }
        bool is_leaf()  const { return r || x; }
        bool is_valid() const { return v && (!w || r); }
    };

    inline Address make_phys(uint32_t va_raw, const Sv32Pte &pte, int level) {
        uint32_t offset = va_raw & ((1u << PAGE_SHIFT) - 1);
        uint32_t phys_ppn = pte.ppn;
        if (level == 1) {
            uint32_t vpn0 = (va_raw >> PAGE_SHIFT) & VPN_MASK;
            phys_ppn = (phys_ppn & ~VPN_MASK) | vpn0;
        }
        return Address { (uint64_t(phys_ppn) << PAGE_SHIFT) | offset };
    }
}

#endif //SV32_H
