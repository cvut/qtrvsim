#ifndef CONTROLSTATE_H
#define CONTROLSTATE_H

#include "common/math/bit_ops.h"
#include "csr/register_desc.h"
#include "machinedefs.h"
#include "memory/address.h"
#include "register_value.h"
#include "simulator_exception.h"

#include <QObject>
#include <QString>
#include <cstdint>

namespace machine { namespace CSR {
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
        RegisterValue read(Address address) const;

        /**
         * Read CSR register with an internal id.
         *
         * Internal id can be obtained from enum Id and works as an index to compacted table
         * of existing CSR registers.
         */
        RegisterValue read_internal(size_t internal_id) const;

        /** Write value to CSR register by ISA specified address and receive the previous value. */
        RegisterValue write_swap(Address address, RegisterValue value);

        RegisterValue write_swap_internal(size_t internal_id, RegisterValue value);

        /**
         * Shorthand for counter incrementing. Counters like time might have different increment
         * amount.
         */
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

    protected:
        void write_csr_count_compare(Address address, RegisterValue value);

    private:
        static size_t get_register_internal_id(Address address);

        Xlen xlen = Xlen::_32; // TODO

        /** Maps ISA specified address to compacted internal id. */
        static std::map<const Address, size_t> address_to_register_map;

        /**
         * Compacted table of existing CSR registers data. Each item is described by table
         * REGISTERS at corresponding indexes.
         */
        std::array<RegisterValue, REGISTERS.size()> register_data;
    };
}} // namespace machine::CSR

Q_DECLARE_METATYPE(machine::CSR::ControlState)

#endif // CONTROLSTATE_H
