#ifndef CONTROLSTATE_H
#define CONTROLSTATE_H

#include "common/math/bit_ops.h"
#include "csr/address.h"
#include "machinedefs.h"
#include "register_value.h"
#include "simulator_exception.h"

#include <QObject>
#include <QString>
#include <cstdint>
#include <unordered_map>

namespace machine { namespace CSR {
    /** CSR register names mapping the registers to continuous locations in internal buffer */
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
            MIE,
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

    /**
     * This class provides access to state of CSR registers.
     *
     * Registers are externally addressed by 12bit address. To simplify simulation, all existing
     * registers are stored in continuous array and indexed by "internal id" (not stable).
     */
    class ControlState : public QObject {
        Q_OBJECT

    public:
        ControlState();
        ControlState(const ControlState &);

        /** Read CSR register with ISA specified address. */
        [[nodiscard]] RegisterValue read(Address address) const;

        /**
         * Read CSR register with an internal id.
         *
         * Internal id can be obtained from enum Id and works as an index to compacted table
         * of existing CSR registers.
         */
        [[nodiscard]] RegisterValue read_internal(size_t internal_id) const;

        /** Write value to CSR register by ISA specified address and receive the previous value. */
        void write(Address address, RegisterValue value);

        /** Used for writes occurring as a side-effect (instruction count update...) and
         * internally by the write method. */
        void write_internal(size_t internal_id, RegisterValue value);

        /** Shorthand for counter incrementing. Counters like time might have different increment
         * amount. */
        void increment_internal(size_t internal_id, uint64_t amount);

        /** Reset data to initial values */
        void reset();

        void update_exception_cause(enum ExceptionCause excause);

        bool operator==(const ControlState &other) const;
        bool operator!=(const ControlState &c) const;

        bool core_interrupt_request();
        machine::Address exception_pc_address();

    signals:
        void write_signal(size_t internal_reg_id, RegisterValue val);
        void read_signal(size_t internal_reg_id, RegisterValue val) const;

    public slots:
        void set_interrupt_signal(uint irq_num, bool active);
        void set_status_exl(bool value);

    private:
        static size_t get_register_internal_id(Address address);

        Xlen xlen = Xlen::_32; // TODO

        /**
         * Compacted table of existing CSR registers data. Each item is described by table
         * REGISTERS at corresponding indexes.
         */
        std::array<RegisterValue, Id::_COUNT> register_data;

    public:
        void default_wlrl_write_handler( // NOLINT(readability-convert-member-functions-to-static)
            const RegisterDesc &desc,
            RegisterValue &reg,
            RegisterValue val) {
            UNUSED(desc)
            reg = val;
        }
    };

    struct RegisterDesc {
        using WriteHandlerFn = void (
            ControlState::*)(const RegisterDesc &desc, RegisterValue &reg, RegisterValue val);

        const char *name = "unknown";
        Address address = Address(0);
        const char *description = "";
        RegisterValue initial_value = 0;
        RegisterValue write_mask = 0;
        WriteHandlerFn write_handler = &ControlState::default_wlrl_write_handler;
    };

    /** Definitions of supported CSR registers */
    inline constexpr std::array<RegisterDesc, Id::_COUNT> REGISTERS { {
        [Id::MVENDORID] = { "mvendorid", 0xF11_csr, "Vendor ID." },
        [Id::MARCHID] = { "marchid", 0xF12_csr, "Architecture ID." },
        [Id::MIMPID] = { "mimpid", 0xF13_csr, "Implementation ID." },
        [Id::MHARTID] = { "mhardid", 0xF14_csr, "Hardware thread ID." },
        [Id::MSTATUS] = { "mstatus", 0x300_csr, "Machine status register." },
        [Id::MIE] = { "mie", 0x304_csr, "Machine interrupt-enable register." },
        [Id::MTVEC] = { "mtvec", 0x305_csr, "Machine trap-handler base address." },
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

    /** Lookup from CSR address (value used in instruction) to internal id (index in continuous
     * memory) */
    class RegisterMap {
        bool initialized = false;
        std::unordered_map<Address, size_t> map;

        void init() {
            for (size_t i = 0; i < REGISTERS.size(); i++) {
                map.emplace(REGISTERS[i].address, i);
            }
            initialized = true;
        }

    public:
        size_t at(Address address) {
            if (!initialized) init();
            return map.at(address);
        }
    };

    static RegisterMap REGISTER_MAP;
}} // namespace machine::CSR

Q_DECLARE_METATYPE(machine::CSR::ControlState)

#endif // CONTROLSTATE_H
