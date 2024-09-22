#ifndef CONTROLSTATE_H
#define CONTROLSTATE_H

#include "common/math/bit_ops.h"
#include "csr/address.h"
#include "machinedefs.h"
#include "register_value.h"
#include "simulator_exception.h"
#include "bitfield.h"
#include "config_isa.h"

#include <QObject>
#include <QString>
#include <cstdint>
#include <unordered_map>

namespace machine { namespace CSR {
    /** CSR register names mapping the registers to continuous locations in internal buffer */
    struct Id {
        enum IdxType{
            // Unprivileged Counter/Timers
            CYCLE,
            // Machine Information Registers
            MVENDORID,
            MARCHID,
            MIMPID,
            MHARTID,
            //            MCONFIGPTR,
            // Machine Trap Setup
            MSTATUS,
            MISA,
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

    struct RegisterFieldDesc {
        uint64_t decode(uint64_t val) const {
            return field.decode(val);
        }
        uint64_t encode(uint64_t val) const {
            return field.encode(val);
        }
        uint64_t mask() const {
            return field.mask();
        }
        uint64_t update(uint64_t orig, uint64_t val) const {
            return field.encode(val) | (orig & ~mask());
        }

        const char *name = "unknown";
        const Id::IdxType regId;
        const BitField field;
        const char *description = "";
    };

    /**
     * This class provides access to state of CSR registers.
     *
     * Registers are externally addressed by 12bit address. To simplify simulation, all existing
     * registers are stored in continuous array and indexed by "internal id" (not stable).
     */
    class ControlState : public QObject {
        Q_OBJECT

    public:
        ControlState(Xlen xlen = Xlen::_32, ConfigIsaWord isa_word = 0);
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

        /** Read CSR register field */
        RegisterValue read_field(const RegisterFieldDesc &field_desc) const {
            return field_desc.decode(read_internal(field_desc.regId).as_u64());
        }

        /** Write CSR register field */
        void write_field(const RegisterFieldDesc &field_desc, uint64_t value) {
            uint64_t u = read_internal(field_desc.regId).as_u64();
            u = field_desc.update(u, value);
            write_internal(field_desc.regId, u);
        }

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
        void exception_initiate(PrivilegeLevel act_privlev, PrivilegeLevel to_privlev);
        PrivilegeLevel exception_return(enum PrivilegeLevel act_privlev);

    private:
        static size_t get_register_internal_id(Address address);

        /** Write CSR register field without write handler, read-only masking and signal */
        void write_field_raw(const RegisterFieldDesc &field_desc, uint64_t value) {
            uint64_t u = register_data[field_desc.regId].as_u64();
            u = field_desc.update(u, value);
            register_data[field_desc.regId] = u;
        }

        Xlen xlen = Xlen::_32; // TODO

        /**
         * Compacted table of existing CSR registers data. Each item is described by table
         * REGISTERS at corresponding indexes.
         */
        std::array<RegisterValue, Id::_COUNT> register_data;

    public:
        void default_wlrl_write_handler(
            const RegisterDesc &desc,
            RegisterValue &reg,
            RegisterValue val);
        void mstatus_wlrl_write_handler(
            const RegisterDesc &desc,
            RegisterValue &reg,
            RegisterValue val);
        void mcycle_wlrl_write_handler(
            const RegisterDesc &desc,
            RegisterValue &reg,
            RegisterValue val);
    };

    struct RegisterDesc {
        using WriteHandlerFn = void (
            ControlState::*)(const RegisterDesc &desc, RegisterValue &reg, RegisterValue val);

        const char *name = "unknown";
        Address address = Address(0);
        const char *description = "";
        RegisterValue initial_value = 0;
        RegisterValue write_mask = (register_storage_t)0xffffffffffffffff;
        WriteHandlerFn write_handler = &ControlState::default_wlrl_write_handler;
        struct {
            const RegisterFieldDesc * const *array;
            const unsigned count;
        } fields = {nullptr, 0};
    };

    namespace Field {
        namespace mstatus {
            static constexpr RegisterFieldDesc SIE = { "SIE", Id::MSTATUS, {1, 1}, "System global interrupt-enable"};
            static constexpr RegisterFieldDesc MIE = { "MIE", Id::MSTATUS, {1, 3}, "Machine global interrupt-enable"};
            static constexpr RegisterFieldDesc SPIE = { "SPIE", Id::MSTATUS, {1, 5}, "Previous SIE before the trap"};
            static constexpr RegisterFieldDesc MPIE = { "MPIE", Id::MSTATUS, {1, 7}, "Previous MIE before the trap"};
            static constexpr RegisterFieldDesc SPP = { "SPP", Id::MSTATUS, {1, 8}, "System previous privilege mode"};
            static constexpr RegisterFieldDesc MPP = { "MPP", Id::MSTATUS, {2, 11}, "Machine previous privilege mode"};
            static constexpr RegisterFieldDesc UXL = { "UXL", Id::MSTATUS, {2, 32}, "User mode XLEN (RV64 only)"};
            static constexpr RegisterFieldDesc SXL = { "SXL", Id::MSTATUS, {2, 34}, "Supervisor mode XLEN (RV64 only)"};
            static constexpr const RegisterFieldDesc *fields[] = { &SIE, &MIE, &SPIE, &MPIE, &SPP, &MPP, &UXL, &SXL};
            static constexpr unsigned count = sizeof(fields) / sizeof(fields[0]);
        }
    }

    /** Definitions of supported CSR registers */
    inline constexpr std::array<RegisterDesc, Id::_COUNT> REGISTERS { {
        // Unprivileged Counter/Timers
        [Id::CYCLE] = { "cycle", 0xC00_csr, "Cycle counter for RDCYCLE instruction.", 0, 0},
        // Priviledged Machine Mode Registers
        [Id::MVENDORID] = { "mvendorid", 0xF11_csr, "Vendor ID.", 0, 0},
        [Id::MARCHID] = { "marchid", 0xF12_csr, "Architecture ID.", 0, 0},
        [Id::MIMPID] = { "mimpid", 0xF13_csr, "Implementation ID.", 0, 0},
        [Id::MHARTID] = { "mhardid", 0xF14_csr, "Hardware thread ID." },
        [Id::MSTATUS] = { "mstatus", 0x300_csr, "Machine status register.",
                        0, 0x007FFFEA, &ControlState::mstatus_wlrl_write_handler,
                        {Field::mstatus::fields, Field::mstatus::count} },
        [Id::MISA] = { "misa", 0x301_csr, "Machine ISA Register.", 0, 0},
        [Id::MIE] = { "mie", 0x304_csr, "Machine interrupt-enable register.",
                        0, 0x00ff0AAA},
        [Id::MTVEC] = { "mtvec", 0x305_csr, "Machine trap-handler base address."},
        [Id::MSCRATCH] = { "mscratch", 0x340_csr, "Scratch register for machine trap handlers." },
        [Id::MEPC] = { "mepc", 0x341_csr, "Machine exception program counter." },
        [Id::MCAUSE] = { "mcause", 0x342_csr, "Machine trap cause." },
        [Id::MTVAL] = { "mtval", 0x343_csr, "Machine bad address or instruction." },
        [Id::MIP] = { "mip", 0x344_csr, "Machine interrupt pending.",
                        0, 0x00000222},
        [Id::MTINST] = { "mtinst", 0x34A_csr, "Machine trap instruction (transformed)." },
        [Id::MTVAL2] = { "mtval2", 0x34B_csr, "Machine bad guest physical address." },
        // Machine Counter/Timers
        [Id::MCYCLE] = { "mcycle", 0xB00_csr, "Machine cycle counter.",
                        0, (register_storage_t)0xffffffffffffffff, &ControlState::mcycle_wlrl_write_handler},
        [Id::MINSTRET] = { "minstret", 0xB02_csr, "Machine instructions-retired counter."},
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

    class RegisterMapByName {
        bool initialized = false;
        std::unordered_map<std::string, size_t> map;

        void init() {
            for (size_t i = 0; i < REGISTERS.size(); i++) {
                map.emplace(std::string(REGISTERS[i].name), i);
            }
            initialized = true;
        }
    public:
        size_t at(std::string name) {
            if (!initialized) init();
            return map.at(name);
        }
    };

    static RegisterMap REGISTER_MAP;
    static RegisterMapByName REGISTER_MAP_BY_NAME;
}} // namespace machine::CSR

Q_DECLARE_METATYPE(machine::CSR::ControlState)

#endif // CONTROLSTATE_H
