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

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "peripheral.h"
#include "qtmipsexception.h"

#include <QMap>
#include <QObject>
#include <cstdint>

namespace machine {

class SerialPort : public MemoryAccess {
    Q_OBJECT
public:
    SerialPort();
    ~SerialPort() override;

signals:
    void tx_byte(unsigned int data);
    void rx_byte_pool(int fd, unsigned int &data, bool &available) const;
    void write_notification(uint32_t address, uint32_t value);
    void read_notification(uint32_t address, uint32_t *value) const;
    void signal_interrupt(uint irq_level, bool active) const;

public slots:
    void rx_queue_check() const;

public:
    bool wword(uint32_t address, uint32_t value) override;
    uint32_t rword(uint32_t address, bool debug_access = false) const override;
    uint32_t get_change_counter() const override;

private:
    void rx_queue_check_internal() const;
    mutable uint32_t change_counter;
    void pool_rx_byte() const;
    void update_rx_irq() const;
    void update_tx_irq() const;
    mutable uint32_t rx_st_reg;
    mutable uint32_t rx_data_reg;
    uint32_t tx_st_reg;
    uint8_t tx_irq_level;
    uint8_t rx_irq_level;
    mutable bool tx_irq_active;
    mutable bool rx_irq_active;
};

} // namespace machine

#endif // SERIALPORT_H
