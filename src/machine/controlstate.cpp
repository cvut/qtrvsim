#include "controlstate.h"

#include "common/logging.h"
#include "machinedefs.h"
#include "simulator_exception.h"

LOG_CATEGORY("machine.csr.control_state");

namespace machine { namespace CSR {

    enum StatusReg {
        Status_IE = 0x00000001,
        Status_EXL = 0x00000002,
        Status_ERL = 0x00000004,
        Status_IntMask = 0x0000ff00,
        Status_Int0 = 0x00000100,
    };

#define COUNTER_IRQ_LEVEL 7

    std::map<const Address, size_t> ControlState::address_to_register_map {};

    ControlState::ControlState() {
        // TODO make this constexpr map with frozen
        if (address_to_register_map.empty()) {
            for (size_t i = 0; i < REGISTERS.size(); i++) {
                address_to_register_map.emplace(REGISTERS[i].address, i);
            }
        }
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
            WARN("Accessed unsupported CSR privilege level %d.", address.get_privilege_level());
            throw IllegalInstruction();
        }

        try {
            return address_to_register_map.at(address);
        } catch (std::out_of_range &e) {
            WARN("Accessed nonexistent CSR register at address %d.", address.data);
            throw IllegalInstruction();
        }
    }

    RegisterValue ControlState::read(Address address) const {
        // Only machine level privilege is supported so no checking is needed.
        size_t reg_id = get_register_internal_id(address);
        RegisterValue val = register_data[reg_id];
        emit read_signal(reg_id, val);
        return val;
    }

    RegisterValue ControlState::write_swap(Address address, RegisterValue value) {
        // Attempts to write a read-only register also raise illegal instruction exceptions.
        if (!address.is_writable()) { throw IllegalInstruction(); }
        return write_swap_internal(get_register_internal_id(address), value);
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

    void ControlState::set_interrupt_signal(uint irq_num, bool active) {
        if (irq_num >= 8) { return; }
        uint64_t mask = Status_Int0 << irq_num;
        size_t reg_id = Id::MCAUSE;
        RegisterValue value = register_data[reg_id];
        if (active) {
            value = value.as_xlen(xlen) | mask;
        } else {
            value = value.as_xlen(xlen) & ~mask;
        }
        emit write_signal(reg_id, value);
    }

    bool ControlState::core_interrupt_request() {
        RegisterValue mstatus = register_data[Id::MSTATUS];
        RegisterValue mcause = register_data[Id::MCAUSE];

        uint64_t irqs = mstatus.as_u64() & mcause.as_u64() & Status_IntMask;

        return irqs && mstatus.as_u64() & Status_IntMask && !(mstatus.as_u64() & Status_EXL)
               && !(mstatus.as_u64() & Status_ERL);
    }

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

    void ControlState::write_csr_count_compare(Address address, RegisterValue value) {
        set_interrupt_signal(COUNTER_IRQ_LEVEL, false);
        write_swap(address, value);
    }

    RegisterValue ControlState::read_internal(size_t internal_id) const {
        return register_data[internal_id];
    }

    RegisterValue ControlState::write_swap_internal(size_t internal_id, RegisterValue value) {
        RegisterValue old_val = register_data[internal_id];
        RegisterDesc desc = REGISTERS[internal_id];
        (*desc.write_handler)(desc, register_data[internal_id], value);
        write_signal(internal_id, value);
        return old_val;
    }
    void ControlState::increment_internal(size_t internal_id, uint64_t amount) {
        auto value = register_data[internal_id];
        write_swap_internal(internal_id, value.as_u64() + amount);
    }
}} // namespace machine::CSR
