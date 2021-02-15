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

#include "serialport.h"

#define SERP_RX_ST_REG_o 0x00
#define SERP_RX_ST_REG_READY_m 0x1
#define SERP_RX_ST_REG_IE_m 0x2

#define SERP_RX_DATA_REG_o 0x04

#define SERP_TX_ST_REG_o 0x08
#define SERP_TX_ST_REG_READY_m 0x1
#define SERP_TX_ST_REG_IE_m 0x2

#define SERP_TX_DATA_REG_o 0x0c

using namespace machine;

SerialPort::SerialPort() {
    change_counter = 0;
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

bool SerialPort::wword(uint32_t address, uint32_t value) {
#if 0
    printf("SerialPort::wword address 0x%08lx data 0x%08lx\n",
           (unsigned long)address, (unsigned long)value);
#endif
    emit write_notification(address, value);

    switch (address & ~3) {
    case SERP_RX_ST_REG_o:
        rx_st_reg &= ~SERP_RX_ST_REG_IE_m;
        rx_st_reg |= value & SERP_RX_ST_REG_IE_m;
        rx_queue_check_internal();
        update_rx_irq();
        break;
    case SERP_TX_ST_REG_o:
        tx_st_reg &= ~SERP_TX_ST_REG_IE_m;
        tx_st_reg |= value & SERP_TX_ST_REG_IE_m;
        update_tx_irq();
        break;
    case SERP_TX_DATA_REG_o:
        emit tx_byte(value & 0xff);
        update_tx_irq();
        break;
    }
    return true;
}

uint32_t SerialPort::rword(uint32_t address, bool debug_access) const {
    (void)debug_access;
    uint32_t value = 0x00000000;
#if 0
    printf("SerialPort::rword address 0x%08lx\n",
           (unsigned long)address);
#endif
    switch (address & ~3) {
    case SERP_RX_ST_REG_o:
        pool_rx_byte();
        value = rx_st_reg;
        break;
    case SERP_RX_DATA_REG_o:
        pool_rx_byte();
        if (rx_st_reg & SERP_RX_ST_REG_READY_m) {
            value = rx_data_reg;
            if (!debug_access) {
                rx_st_reg &= ~SERP_RX_ST_REG_READY_m;
                update_rx_irq();
                emit external_change_notify(
                    this, SERP_RX_ST_REG_o, SERP_RX_DATA_REG_o + 3, true);
            }
        } else {
            value = 0;
        }
        rx_queue_check_internal();
        break;
    case SERP_TX_ST_REG_o: value = tx_st_reg | SERP_TX_ST_REG_READY_m; break;
    }

    emit read_notification(address, &value);

    return value;
}

uint32_t SerialPort::get_change_counter() const {
    return change_counter;
}

void SerialPort::update_rx_irq() const {
    bool active = !!(rx_st_reg & SERP_RX_ST_REG_IE_m);
    active &= !!(rx_st_reg & SERP_RX_ST_REG_READY_m);
    if (active != rx_irq_active) {
        rx_irq_active = active;
        change_counter++;
        emit signal_interrupt(rx_irq_level, active);
    }
}

void SerialPort::update_tx_irq() const {
    bool active = !!(tx_st_reg & SERP_TX_ST_REG_IE_m);
    active &= !!(tx_st_reg & SERP_TX_ST_REG_READY_m);
    if (active != tx_irq_active) {
        tx_irq_active = active;
        change_counter++;
        emit signal_interrupt(tx_irq_level, active);
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
    emit external_change_notify(
        this, SERP_RX_ST_REG_o, SERP_RX_DATA_REG_o + 3, true);
}
