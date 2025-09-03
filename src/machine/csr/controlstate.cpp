#include "controlstate.h"

#include "common/logging.h"
#include "machinedefs.h"
#include "simulator_exception.h"

#include <QtAlgorithms>
#include <cinttypes>

LOG_CATEGORY("machine.csr.control_state");

namespace machine { namespace CSR {

    ControlState::ControlState(Xlen xlen, ConfigIsaWord isa_word) : xlen(xlen) {
        reset();
        uint64_t misa = read_internal(CSR::Id::MISA).as_u64();
        misa |= isa_word.toUnsigned();
        register_data[CSR::Id::MISA] = misa;
    }

    ControlState::ControlState(const ControlState &other)
        : QObject(this->parent())
        , xlen(other.xlen)
        , register_data(other.register_data) {}

    void ControlState::reset() {
        std::transform(
            REGISTERS.begin(), REGISTERS.end(), register_data.begin(),
            [](const RegisterDesc &desc) { return desc.initial_value; });

        uint64_t misa = read_internal(CSR::Id::MISA).as_u64();
        misa &= 0x3fffffff;
        if (xlen == Xlen::_32) {
            misa |= (uint64_t)1 << 30;
        } else if (xlen == Xlen::_64) {
            misa |= (uint64_t)2 << 62;
        }
        register_data[CSR::Id::MISA] = misa;

        if (xlen == Xlen::_64) {
            write_field_raw(Field::mstatus::UXL, 2);
            write_field_raw(Field::mstatus::SXL, 2);
        }
    }

    size_t ControlState::get_register_internal_id(Address address) {
        // if (address.get_privilege_level() != PrivilegeLevel::MACHINE)

        try {
            return CSR::REGISTER_MAP.at(address);
        } catch (std::out_of_range &e) {
            throw SIMULATOR_EXCEPTION(
                UnsupportedInstruction,
                QString("Accessed nonexistent CSR register %1").arg(address.data), "");
        }
    }

    RegisterValue ControlState::read(Address address) const {
        // Only machine level privilege is supported so no checking is needed.
        size_t reg_id = get_register_internal_id(address);
        RegisterValue value = register_data[reg_id];
        DEBUG("Read CSR[%u] == 0x%" PRIx64, address.data, value.as_u64());
        emit read_signal(reg_id, value);
        return value;
    }

    void ControlState::write(Address address, RegisterValue value) {
        DEBUG(
            "Write CSR[%u/%zu] <== 0x%zu", address.data, get_register_internal_id(address),
            value.as_u64());
        // Attempts to write a read-only register also raise illegal instruction exceptions.
        if (!address.is_writable()) {
            throw SIMULATOR_EXCEPTION(
                UnsupportedInstruction,
                QString("CSR address %1 is not writable.").arg(address.data), "");
        }
        write_internal(get_register_internal_id(address), value);
    }

    void ControlState::default_wlrl_write_handler(
        const RegisterDesc &desc,
        RegisterValue &reg,
        RegisterValue val) {
        uint64_t u;
        u = val.as_u64() & desc.write_mask.as_u64();
        u |= reg.as_u64() & ~desc.write_mask.as_u64();
        if (xlen == Xlen::_32) u &= 0xffffffff;
        reg = u;
    }
    void ControlState::mstatus_wlrl_write_handler(
        const RegisterDesc &desc,
        RegisterValue &reg,
        RegisterValue val) {
        default_wlrl_write_handler(desc, reg, val);
    }

    void ControlState::mcycle_wlrl_write_handler(
        const RegisterDesc &desc,
        RegisterValue &reg,
        RegisterValue val) {
        Q_UNUSED(desc)
        reg = val;
        register_data[Id::CYCLE] = val;
        write_signal(Id::CYCLE, register_data[Id::CYCLE]);
    }

    void ControlState::sstatus_wlrl_write_handler(
        const RegisterDesc &desc,
        RegisterValue &reg,
        RegisterValue val) {
        uint64_t s_mask
            = Field::mstatus::SIE.mask() | Field::mstatus::SPIE.mask() | Field::mstatus::SPP.mask();

        if (xlen == Xlen::_64) {
            s_mask |= Field::mstatus::UXL.mask();
            s_mask |= Field::mstatus::SXL.mask();
        }
        uint64_t write_val = val.as_u64() & desc.write_mask.as_u64();
        uint64_t mstatus_val = register_data[Id::MSTATUS].as_u64();
        mstatus_val = (mstatus_val & ~s_mask) | (write_val & s_mask);
        register_data[Id::MSTATUS] = mstatus_val;
        uint64_t new_sstatus = mstatus_val & s_mask;
        if (xlen == Xlen::_32) new_sstatus &= 0xffffffff;
        reg = new_sstatus;
        emit write_signal(Id::MSTATUS, register_data[Id::MSTATUS]);
    }

    bool ControlState::operator==(const ControlState &other) const {
        return register_data == other.register_data;
    }

    bool ControlState::operator!=(const ControlState &c) const {
        return !this->operator==(c);
    }

    void ControlState::update_exception_cause(enum ExceptionCause excause) {
        RegisterValue &value = register_data[Id::MCAUSE];
        if (excause != EXCAUSE_INT) {
            value = static_cast<unsigned>(excause);
        } else {
            RegisterValue mie = register_data[Id::MIE];
            RegisterValue mip = register_data[Id::MIP];
            int irq_to_signal = 0;

            quint64 irqs = mie.as_u64() & mip.as_u64() & 0xffffffff;

            if (irqs != 0) {
                // Find the first (leas significant) set bit
                irq_to_signal = 63 - qCountLeadingZeroBits(irqs & (~irqs + 1));
            }

            value = (uint64_t)(irq_to_signal | ((uint64_t)1 << ((xlen == Xlen::_32) ? 31 : 63)));
        }
        emit write_signal(Id::MCAUSE, value);
    }

    void ControlState::set_interrupt_signal(uint irq_num, bool active) {
        if (irq_num >= 32) { return; }
        uint64_t mask = 1 << irq_num;
        size_t reg_id = Id::MIP;
        RegisterValue &value = register_data[reg_id];
        if (active) {
            value = value.as_xlen(xlen) | mask;
        } else {
            value = value.as_xlen(xlen) & ~mask;
        }
        emit write_signal(reg_id, value);
    }

    bool ControlState::core_interrupt_request() {
        RegisterValue mie = register_data[Id::MIE];
        RegisterValue mip = register_data[Id::MIP];

        uint64_t irqs = mie.as_u64() & mip.as_u64() & 0xffffffff;

        return irqs && read_field(Field::mstatus::MIE).as_u64();
    }

    void ControlState::exception_initiate(PrivilegeLevel act_privlev, PrivilegeLevel to_privlev) {
        size_t reg_id = Id::MSTATUS;
        RegisterValue &reg = register_data[reg_id];
        Q_UNUSED(act_privlev)
        Q_UNUSED(to_privlev)

        write_field(Field::mstatus::MPIE, read_field(Field::mstatus::MIE).as_u32());
        write_field(Field::mstatus::MIE, (uint64_t)0);

        write_field(Field::mstatus::MPP, static_cast<uint64_t>(act_privlev));

        emit write_signal(reg_id, reg);
    }

    PrivilegeLevel ControlState::exception_return(enum PrivilegeLevel act_privlev) {
        size_t reg_id = Id::MSTATUS;
        RegisterValue &reg = register_data[reg_id];
        PrivilegeLevel restored_privlev = PrivilegeLevel::MACHINE;
        if (act_privlev == PrivilegeLevel::MACHINE) {
            // MRET semantics:
            //   MIE  <- MPIE
            //   MPIE <- 1
            //   restored_privlev <- MPP
            //   MPP  <- 0
            write_field(Field::mstatus::MIE, read_field(Field::mstatus::MPIE).as_u32());
            write_field(Field::mstatus::MPIE, (uint64_t)1);
            uint32_t raw_mpp
                = static_cast<uint32_t>(read_field(Field::mstatus::MPP).as_u32()) & 0x3;
            switch (raw_mpp) {
            case 0: restored_privlev = PrivilegeLevel::UNPRIVILEGED; break;
            case 1: restored_privlev = PrivilegeLevel::SUPERVISOR; break;
            case 2: restored_privlev = PrivilegeLevel::HYPERVISOR; break;
            case 3: restored_privlev = PrivilegeLevel::MACHINE; break;
            default: restored_privlev = PrivilegeLevel::UNPRIVILEGED; break;
            }
            write_field(Field::mstatus::MPP, (uint64_t)0); // clear MPP per spec
        } else if (act_privlev == PrivilegeLevel::SUPERVISOR) {
            // SRET semantics:
            //   SIE  <- SPIE
            //   SPIE <- 1
            //   restored_privlev <- SPP
            //   SPP  <- 0
            write_field(Field::mstatus::SIE, read_field(Field::mstatus::SPIE).as_u32());
            write_field(Field::mstatus::SPIE, (uint64_t)1);
            uint32_t raw_spp
                = static_cast<uint32_t>(read_field(Field::mstatus::SPP).as_u32()) & 0x1;
            restored_privlev
                = (raw_spp == 1) ? PrivilegeLevel::SUPERVISOR : PrivilegeLevel::UNPRIVILEGED;
            write_field(Field::mstatus::SPP, (uint64_t)0);
        } else {
            restored_privlev = PrivilegeLevel::UNPRIVILEGED;
        }

        emit write_signal(reg_id, reg);

        return restored_privlev;
    }

    machine::Address ControlState::exception_pc_address() {
        return machine::Address(register_data[Id::MTVEC].as_u64());
    }

    RegisterValue ControlState::read_internal(size_t internal_id) const {
        return register_data[internal_id];
    }

    void ControlState::write_internal(size_t internal_id, RegisterValue value) {
        RegisterDesc desc = REGISTERS[internal_id];
        RegisterValue &reg = register_data[internal_id];
        (this->*desc.write_handler)(desc, reg, value);
        write_signal(internal_id, reg);
    }
    void ControlState::increment_internal(size_t internal_id, uint64_t amount) {
        auto value = register_data[internal_id];
        write_internal(internal_id, value.as_u64() + amount);
    }
}} // namespace machine::CSR
