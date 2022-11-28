#include "controlstate.h"

#include "common/logging.h"
#include "machinedefs.h"
#include "simulator_exception.h"

LOG_CATEGORY("machine.csr.control_state");

namespace machine { namespace CSR {

    // TODO this is mips
    enum StatusReg {
        Status_IE = 0x00000001,
        Status_EXL = 0x00000002,
        Status_ERL = 0x00000004,
        Status_IntMask = 0x0000ff00,
    };

    ControlState::ControlState() {
        reset();
    }

    ControlState::ControlState(const ControlState &other) : register_data(other.register_data) {}

    void ControlState::reset() {
        std::transform(
            REGISTERS.begin(), REGISTERS.end(), register_data.begin(),
            [](const RegisterDesc &desc) { return desc.initial_value; });
    }

    size_t ControlState::get_register_internal_id(Address address) {
        if (address.get_privilege_level() != PrivilegeLevel::MACHINE) {
            throw SIMULATOR_EXCEPTION(
                UnsupportedInstruction,
                QString("Only machine level CSR registers are currently implemented. Accessed "
                        "level %1.")
                    .arg(static_cast<unsigned>(address.get_privilege_level())),
                "");
        }

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
        DEBUG("Read CSR[%u] == %lu", address.data, value.as_u64());
        emit read_signal(reg_id, value);
        return value;
    }

    void ControlState::write(Address address, RegisterValue value) {
        DEBUG(
            "Write CSR[%u/%lu] <== %lu", address.data, get_register_internal_id(address),
            value.as_u64());
        // Attempts to write a read-only register also raise illegal instruction exceptions.
        if (!address.is_writable()) {
            throw SIMULATOR_EXCEPTION(
                UnsupportedInstruction,
                QString("CSR address %1 is not writable.").arg(address.data), "");
        }
        write_internal(get_register_internal_id(address), value);
    }

    bool ControlState::operator==(const ControlState &other) const {
        return register_data == other.register_data;
    }

    bool ControlState::operator!=(const ControlState &c) const {
        return !this->operator==(c);
    }

    void ControlState::update_exception_cause(enum ExceptionCause excause) {
        RegisterValue &value = register_data[Id::MCAUSE];
        value = value.as_u32() & ~0x80000000 & ~0x0000007f;
        if (excause != EXCAUSE_INT) {
            value = value.as_u32() | static_cast<unsigned>(excause) << 2;
        }
        // TODO: this is known ahead of time
        emit write_signal(Id::MCAUSE, value);
    }

    // TODO this is mips
    void ControlState::set_interrupt_signal(uint irq_num, bool active) {
        if (irq_num >= 8) { return; }
        uint64_t mask = 1 << irq_num;
        size_t reg_id = Id::MIP;
        RegisterValue value = register_data[reg_id];
        if (active) {
            value = value.as_xlen(xlen) | mask;
        } else {
            value = value.as_xlen(xlen) & ~mask;
        }
        emit write_signal(reg_id, value);
    }

    // TODO this is mips
    bool ControlState::core_interrupt_request() {
        RegisterValue mstatus = register_data[Id::MSTATUS];
        RegisterValue mcause = register_data[Id::MCAUSE];

        uint64_t irqs = mstatus.as_u64() & mcause.as_u64() & Status_IntMask;

        return irqs && mstatus.as_u64() & Status_IntMask && !(mstatus.as_u64() & Status_EXL)
               && !(mstatus.as_u64() & Status_ERL);
    }

    // TODO this is mips
    void ControlState::set_status_exl(bool value) {
        size_t reg_id = Id::MSTATUS;
        RegisterValue &reg = register_data[reg_id];
        if (value) {
            reg = reg.as_xlen(xlen) & Status_EXL;
        } else {
            reg = reg.as_xlen(xlen) & ~Status_EXL;
        }
        emit write_signal(reg_id, reg);
    }

    machine::Address ControlState::exception_pc_address() {
        return machine::Address(register_data[Id::MTVEC].as_u64());
    }

    RegisterValue ControlState::read_internal(size_t internal_id) const {
        return register_data[internal_id];
    }

    void ControlState::write_internal(size_t internal_id, RegisterValue value) {
        RegisterDesc desc = REGISTERS[internal_id];
        (*desc.write_handler)(desc, register_data[internal_id], value);
        write_signal(internal_id, value);
    }
    void ControlState::increment_internal(size_t internal_id, uint64_t amount) {
        auto value = register_data[internal_id];
        write_internal(internal_id, value.as_u64() + amount);
    }
}} // namespace machine::CSR
