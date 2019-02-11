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

#include "peripheral.h"

using namespace machine;

SimplePeripheral::SimplePeripheral() {

}

SimplePeripheral::~SimplePeripheral() {

}

bool SimplePeripheral::wword(std::uint32_t address, std::uint32_t value) {
#if 0
    printf("SimplePeripheral::wword address 0x%08lx data 0x%08lx\n",
           (unsigned long)address, (unsigned long)value);
#endif
    emit write_notification(address, value);

    return true;
}

std::uint32_t SimplePeripheral::rword(std::uint32_t address, bool debug_access) const {
    (void)debug_access;
    std::uint32_t value = 0x12345678;
#if 0
    printf("SimplePeripheral::rword address 0x%08lx\n",
           (unsigned long)address);
#endif

    emit read_notification(address, &value);

    return value;
}
