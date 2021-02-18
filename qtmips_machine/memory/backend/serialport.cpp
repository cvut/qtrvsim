// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 * Copyright (c) 2020      Jakub Dupak <dupak.jakub@gmail.com>
 * Copyright (c) 2020      Max Hollmann <hollmmax@fel.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#include "memory/backend/serialport.h"

namespace machine {

constexpr Offset SERP_RX_ST_REG_o = 0x00u;
constexpr Offset SERP_RX_ST_REG_READY_m = 0x1u;
constexpr Offset SERP_RX_ST_REG_IE_m = 0x2u;

constexpr Offset SERP_RX_DATA_REG_o = 0x04u;

constexpr Offset SERP_TX_ST_REG_o = 0x08u;
constexpr Offset SERP_TX_ST_REG_READY_m = 0x1u;
constexpr Offset SERP_TX_ST_REG_IE_m = 0x2u;

constexpr Offset SERP_TX_DATA_REG_o = 0xcu;

SerialPort::SerialPort() {
    rx_st_reg = 0;
    rx_data_reg = 0;
    tx_st_reg = 0;
    tx_irq_level = 2; // HW Interrupt 0
    rx_irq_level = 3; // HW Interrupt 1
    tx_irq_active = false;
    rx_irq_active = false;
}

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
            UNUSED(src);
            return 0;
        },
        [&](Offset src, uint32_t value) {
            return write_reg(src, byteswap(value));
        });
}

ReadResult SerialPort::read(
    void *destination,
    Offset source,
    size_t size,
    ReadOptions options) const {
    return read_by_u32(destination, source, size, [&](Offset src) {
        return byteswap(read_reg(src, options.debug));
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
        this, SERP_RX_ST_REG_o, SERP_RX_DATA_REG_o + 3, true);
}

void SerialPort::update_tx_irq() const {
    bool active = (tx_st_reg & SERP_TX_ST_REG_IE_m) != 0;
    active &= (tx_st_reg & SERP_TX_ST_REG_READY_m) != 0;
    if (active != tx_irq_active) {
        tx_irq_active = active;
        emit signal_interrupt(tx_irq_level, active);
    }
}

uint32_t SerialPort::read_reg(Offset source, bool debug) const {
    Q_ASSERT((source & 3U) == 0); // uint32_t alligned

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
            if (!debug) {
                rx_st_reg &= ~SERP_RX_ST_REG_READY_m;
                update_rx_irq();
                emit external_backend_change_notify(
                    this, SERP_RX_ST_REG_o, SERP_RX_DATA_REG_o + 3, true);
            }
        } else {
            value = 0;
        }
        rx_queue_check_internal();
        break;
    case SERP_TX_ST_REG_o: value = tx_st_reg | SERP_TX_ST_REG_READY_m; break;
    default:
        printf(
            "WARNING: Serial port - read out of range (at 0x%ld).\n", source);
        break;
    }

    emit read_notification(source, value);

    return value;
}

uint32_t SerialPort::get_change_counter() const {
    return change_counter;
}

bool SerialPort::write_reg(Offset destination, uint32_t value) {
    Q_ASSERT((destination & 3U) == 0); // uint32_t alligned

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
        default:
            printf(
                "WARNING: Serial port - write out of range (at 0x%ld).\n",
                destination);
            return false;
        }
    }();

    emit write_notification(destination, value);

    return changed;
}
LocationStatus SerialPort::location_status(Offset offset) const {
    switch (offset & ~3U) {
    case SERP_RX_ST_REG_o: FALLTROUGH
    case SERP_RX_DATA_REG_o: FALLTROUGH
    case SERP_TX_DATA_REG_o: {
        return LOCSTAT_NONE;
    }
    default: {
        return LOCSTAT_ILLEGAL;
    }
    }
}
} // namespace machine