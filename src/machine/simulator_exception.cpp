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

#include "simulator_exception.h"

#include <cstring>
#include <iostream>
#include <utility>

using namespace machine;

QtMipsException::QtMipsException(
    QString reason,
    QString ext,
    QString file,
    int line) {
    this->name = "Exception";
    this->reason = std::move(reason);
    this->ext = std::move(ext);
    this->file = std::move(file);
    this->line = line;
}

const char *QtMipsException::what() const noexcept {
    QString message = this->msg(true);
    char *cstr = new char[message.length() + 1];
    std::strcpy(cstr, message.toStdString().c_str());
    return cstr;
}

QString QtMipsException::msg(bool pos) const {
    QString message;
    message += name;
    if (pos) {
        message += QString(" (") + QString(this->file) + QString(":")
                   + QString::number(this->line) + QString(")");
    }
    message += ": " + this->reason;
    if (!this->ext.isEmpty()) {
        message += QString(": ");
        message += this->ext;
    }
    return message;
}

#define EXCEPTION(NAME, PARENT)                                                \
    QtMipsException##NAME::QtMipsException##NAME(                              \
        QString reason, QString ext, QString file, int line)                   \
        : QtMipsException##PARENT(reason, ext, file, line) {                   \
        name = #NAME;                                                          \
    }
QTMIPS_EXCEPTIONS
#undef EXCEPTION
