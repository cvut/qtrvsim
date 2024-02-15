#include "memory/backend/serialport.h"

#include "common/endian.h"

#include <common/logging.h>

LOG_CATEGORY("machine.memory.serialport");

using ae = machine::AccessEffects; // For enum values, type is obvious from
                                   // context.

namespace machine {

constexpr Offset SERP_RX_ST_REG_o = 0x00u;
constexpr uint32_t SERP_RX_ST_REG_READY_m = 0x1u;
constexpr uint32_t SERP_RX_ST_REG_IE_m = 0x2u;

constexpr Offset SERP_RX_DATA_REG_o = 0x04u;

constexpr Offset SERP_TX_ST_REG_o = 0x08u;
constexpr uint32_t SERP_TX_ST_REG_READY_m = 0x1u;
constexpr uint32_t SERP_TX_ST_REG_IE_m = 0x2u;

constexpr Offset SERP_TX_DATA_REG_o = 0xcu;

SerialPort::SerialPort(Endian simulated_machine_endian)
    : BackendMemory(simulated_machine_endian)
    , tx_irq_level(17) // The second platform HW interrupt
    , rx_irq_level(16) // The first platform HW interrupt
    , tx_st_reg(SERP_TX_ST_REG_READY_m)
{}

SerialPort::~SerialPort() = default;

void SerialPort::pool_rx_byte() const {
    unsigned int byte = 0;
    bool available = false;
    if (!(rx_st_reg & SERP_RX_ST_REG_READY_m)) {
        rx_st_reg |= SERP_RX_ST_REG_READY_m;
        emit rx_byte_pool(0, byte, available);
        if (available) {
            change_counter++;
            rx_data_reg = byte;
        } else {
            rx_st_reg &= ~SERP_RX_ST_REG_READY_m;
        }
    }
}

WriteResult SerialPort::write(
    Offset destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    UNUSED(options)
    return write_by_u32(
        destination, source, size,
        [&](Offset src) {
            UNUSED(src)
            return 0;
        },
        [&](Offset src, uint32_t value) {
            return write_reg(
                src, byteswap_if(
                         value, internal_endian != simulated_machine_endian));
        });
}

ReadResult SerialPort::read(
    void *destination,
    Offset source,
    size_t size,
    ReadOptions options) const {
    return read_by_u32(destination, source, size, [&](Offset src) {
        return byteswap_if(
            read_reg(src, options.type),
            internal_endian != simulated_machine_endian);
    });
}

void SerialPort::update_rx_irq() const {
    bool active = (rx_st_reg & SERP_RX_ST_REG_IE_m) != 0;
    active &= (rx_st_reg & SERP_RX_ST_REG_READY_m) != 0;
    if (active != rx_irq_active) {
        rx_irq_active = active;
        emit signal_interrupt(rx_irq_level, active);
    }
}

void SerialPort::rx_queue_check_internal() const {
    if (rx_st_reg & SERP_RX_ST_REG_IE_m) {
        pool_rx_byte();
    }
    update_rx_irq();
}

void SerialPort::rx_queue_check() const {
    rx_queue_check_internal();
    emit external_backend_change_notify(
        this, SERP_RX_ST_REG_o, SERP_RX_DATA_REG_o + 3, ae::EXTERNAL_ASYNC);
}

void SerialPort::update_tx_irq() const {
    bool active = (tx_st_reg & SERP_TX_ST_REG_IE_m) != 0;
    active &= (tx_st_reg & SERP_TX_ST_REG_READY_m) != 0;
    if (active != tx_irq_active) {
        tx_irq_active = active;
        emit signal_interrupt(tx_irq_level, active);
    }
}

uint32_t SerialPort::read_reg(Offset source, AccessEffects type) const {
    Q_ASSERT((source & 3U) == 0); // uint32_t aligned

    uint32_t value = 0;

    switch (source) {
    case SERP_RX_ST_REG_o:
        pool_rx_byte();
        value = rx_st_reg;
        break;
    case SERP_RX_DATA_REG_o:
        pool_rx_byte();
        if (rx_st_reg & SERP_RX_ST_REG_READY_m) {
            value = rx_data_reg;
            if (type == ae::REGULAR) {
                rx_st_reg &= ~SERP_RX_ST_REG_READY_m;
                update_rx_irq();
                emit external_backend_change_notify(
                    this, SERP_RX_ST_REG_o, SERP_RX_DATA_REG_o + 3, ae::EXTERNAL_ASYNC);
            }
        } else {
            value = 0;
        }
        rx_queue_check_internal();
        break;
    case SERP_TX_ST_REG_o: value = tx_st_reg; break;
    default: WARN("Serial port - read out of range (at 0x%zu).\n", source); break;
    }

    emit read_notification(source, value);

    return value;
}

bool SerialPort::write_reg(Offset destination, uint32_t value) {
    Q_ASSERT((destination & 3U) == 0); // uint32_t aligned

    bool changed = [&]() {
        switch (destination & ~3U) {
        case SERP_RX_ST_REG_o:
            rx_st_reg &= ~SERP_RX_ST_REG_IE_m;
            rx_st_reg |= value & SERP_RX_ST_REG_IE_m;
            rx_queue_check_internal();
            update_rx_irq();
            return true;
        case SERP_TX_ST_REG_o:
            tx_st_reg &= ~SERP_TX_ST_REG_IE_m;
            tx_st_reg |= value & SERP_TX_ST_REG_IE_m;
            update_tx_irq();
            return true;
        case SERP_TX_DATA_REG_o:
            emit tx_byte(value & 0xffu);
            update_tx_irq();
            return true;
        default: WARN("Serial port - write out of range (at 0x%zu).\n", destination); return false;
        }
    }();

    emit write_notification(destination, value);

    return changed;
}
LocationStatus SerialPort::location_status(Offset offset) const {
    switch (offset & ~3U) {
    case SERP_RX_ST_REG_o: FALLTROUGH
    case SERP_TX_ST_REG_o: FALLTROUGH
    case SERP_TX_DATA_REG_o: // This is actually write only, but there is no
                             // enum for that.
    {
        return LOCSTAT_NONE;
    }
    case SERP_RX_DATA_REG_o: {
        return LOCSTAT_READ_ONLY;
    }
    default: {
        return LOCSTAT_ILLEGAL;
    }
    }
}

uint32_t SerialPort::get_change_counter() const {
    return change_counter;
}
} // namespace machine
