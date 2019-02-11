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

#include "hexlineedit.h"

HexLineEdit::HexLineEdit(QWidget *parent, int digits, int base, QString prefix):
    Super(parent) {
    this->base = base;
    this->digits = digits;
    this->prefix = prefix;
    last_set = 0;
    QChar dmask;
    QString t = "";
    QString mask = "";

    for (int i = 0; i <prefix.count(); i++)
        mask += "\\" + QString(prefix.at(i));
    switch (base) {
    case 10:
        mask += "D";
        dmask = 'd';
        break;
    case 2:
        mask += "B";
        dmask = 'b';
        break;
    case 16:
    case 0:
    default:
        mask += "H";
        dmask = 'h';
        break;
    }
    if (digits > 1)
        t.fill(dmask, digits  - 1);

    mask += t;

    setInputMask(mask);

    connect(this, SIGNAL(editingFinished()), this, SLOT(on_edit_finished()));

    set_value(0);
}

void HexLineEdit::set_value(std::int32_t value) {
    QString s, t = "";
    last_set = value;
    s = QString::number(value, base);
    if (s.count() < digits)
        t.fill('0', digits - s.count());
    setText(prefix + t +s);

}

void HexLineEdit::on_edit_finished() {
    bool ok;
    std::uint32_t val;
    val = text().toLong(&ok, 16);
    if (!ok) {
        set_value(last_set);
        return;
    }
    last_set = val;
    emit value_edit_finished(val);
}
