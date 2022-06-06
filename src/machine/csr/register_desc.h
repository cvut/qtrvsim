#ifndef QTRVSIM_REGISTER_DESC_H
#define QTRVSIM_REGISTER_DESC_H

#include "address.h"
#include "common/math/bit_ops.h"
#include "register_value.h"
#include "simulator_exception.h"

#include <QString>

namespace machine { namespace CSR {
    struct Id {
        enum {
            // Machine Information Registers
            MVENDORID,
            MARCHID,
            MIMPID,
            MHARTID,
            //            MCONFIGPTR,
            // Machine Trap Setup
            MSTATUS,
            //            MISA,
            //            MEDELEG,
            //            MIDELET,
            //            MIE,
            MTVEC,
            //            MCOUNTERN,
            //            MSTATUSH,
            // Machine Trap Handling
            MSCRATCH,
            MEPC,
            MCAUSE,
            MTVAL,
            MIP,
            MTINST,
            MTVAL2,
            // ...
            MCYCLE,
            MINSTRET,
            _COUNT,
        };
    };

    struct RegisterDesc;
    static inline void
    default_wlrl_write_handler(const RegisterDesc &desc, RegisterValue &reg, RegisterValue val) {
        UNUSED(desc);
        reg = val;
    }

    struct RegisterDesc {
        using WriteHandlerFn
            = void (*)(const RegisterDesc &desc, RegisterValue &reg, RegisterValue val);

        const char *name = "unknown";
        Address address = Address(0);
        const char *description = "";
        RegisterValue initial_value = 0;
        RegisterValue write_mask = 0;
        WriteHandlerFn write_handler = default_wlrl_write_handler;
    };

    static constexpr std::array<RegisterDesc, Id::_COUNT> REGISTERS { {
        [Id::MVENDORID] = { "mvendorid", 0xF11_csr, "Vendor ID." },
        [Id::MARCHID] = { "marchid", 0xF12_csr, "Architecture ID." },
        [Id::MIMPID] = { "mimpid", 0xF13_csr, "Implementation ID." },
        [Id::MHARTID] = { "mhardid", 0xF14_csr, "Hardware thread ID." },
        [Id::MSTATUS] = { "mstatus", 0xF14_csr, "Machine status register." },
        [Id::MTVEC] = { "mtvec", 0xF14_csr, "Machine trap-handler base address." },
        [Id::MSCRATCH] = { "mscratch", 0x340_csr, "Scratch register for machine trap handlers." },
        [Id::MEPC] = { "mepc", 0x341_csr, "Machine exception program counter." },
        [Id::MCAUSE] = { "mcause", 0x342_csr, "Machine trap cause." },
        [Id::MTVAL] = { "mtval", 0x343_csr, "Machine bad address or instruction." },
        [Id::MIP] = { "mip", 0x344_csr, "Machine interrupt pending." },
        [Id::MTINST] = { "mtinsr", 0x34A_csr, "Machine trap instruction (transformed)." },
        [Id::MTVAL2] = { "mtval2", 0x34B_csr, "Machine bad guest physical address." },
        // Machine Counter/Timers
        [Id::MCYCLE] = { "mcycle", 0xB00_csr, "Machine cycle counter." },
        [Id::MINSTRET] = { "minstret", 0xB02_csr, "Machine instructions-retired counter." },
    } };
}} // namespace machine::CSR

#endif // QTRVSIM_REGISTER_DESC_H
