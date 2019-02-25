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

#define SERP_RX_ST_REG_o           0x00
#define SERP_RX_ST_REG_READY_m      0x1
#define SERP_RX_ST_REG_IE_m         0x2

#define SERP_RX_DATA_REG_o         0x04

#define SERP_TX_ST_REG_o           0x08
#define SERP_TX_ST_REG_READY_m      0x1
#define SERP_TX_ST_REG_IE_m         0x2

#define SERP_TX_DATA_REG_o         0x0c

using namespace machine;

SerialPort::SerialPort() {
    rx_st_reg = 0;
    rx_data_reg = 0;
    tx_st_reg = 0;
}

SerialPort::~SerialPort() {

}

void SerialPort::pool_rx_byte() const {
    unsigned int byte = 0;
    bool available = false;
    if (!(rx_st_reg & SERP_RX_ST_REG_READY_m)) {
        emit rx_byte_pool(0, byte, available);
        if (available) {
            rx_data_reg = byte;
            rx_st_reg |= SERP_RX_ST_REG_READY_m;
        }
    }
}

bool SerialPort::wword(std::uint32_t address, std::uint32_t value) {
#if 0
    printf("SerialPort::wword address 0x%08lx data 0x%08lx\n",
           (unsigned long)address, (unsigned long)value);
#endif
    emit write_notification(address, value);

    switch (address) {
    case SERP_RX_ST_REG_o:
        rx_st_reg &= ~SERP_RX_ST_REG_IE_m;
        rx_st_reg |= value & SERP_RX_ST_REG_IE_m;
        break;
    case SERP_TX_ST_REG_o:
        tx_st_reg &= ~SERP_TX_ST_REG_IE_m;
        tx_st_reg |= value & SERP_TX_ST_REG_IE_m;
        break;
    case SERP_TX_DATA_REG_o:
        emit tx_byte(value & 0xff);
        break;
    }
    return true;
}

std::uint32_t SerialPort::rword(std::uint32_t address, bool debug_access) const {
    (void)debug_access;
    std::uint32_t value = 0x00000000;
#if 0
    printf("SerialPort::rword address 0x%08lx\n",
           (unsigned long)address);
#endif
    switch (address) {
    case SERP_RX_ST_REG_o:
        pool_rx_byte();
        value = rx_st_reg;
        break;
    case SERP_RX_DATA_REG_o:
        pool_rx_byte();
        if (rx_st_reg & SERP_RX_ST_REG_READY_m) {
            value = rx_data_reg;
            rx_st_reg &= ~SERP_RX_ST_REG_READY_m;
        } else {
            value = 0;
        }
        break;
    case SERP_TX_ST_REG_o:
        value = tx_st_reg | SERP_TX_ST_REG_READY_m;
        break;
    }

    emit read_notification(address, &value);

    return value;
}
