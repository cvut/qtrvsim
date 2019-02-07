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

#include "value.h"

using namespace coreview;

#define HEIGHT 8
#define LETWIDTH 7

// TODO orientation
Value::Value(bool vertical, unsigned width, std::uint32_t init_val,
             unsigned a_base) : QGraphicsObject(nullptr) {
    wid = width;
    val = init_val;
    base = a_base;
    this->vertical = vertical;
}

QRectF Value::boundingRect() const {
    if (vertical)
        return QRectF(-LETWIDTH/2 - 1, -HEIGHT*(int)wid/2 - 1, LETWIDTH + 2, HEIGHT*wid + 2);
    else
        return QRectF(-(LETWIDTH*(int)wid)/2 - 1, -HEIGHT/2 - 1, LETWIDTH*wid + 2, HEIGHT + 2);
}

void Value::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))){
    QFont f;
    f.setPointSize(7);
    painter->setFont(f);

    QRectF rect;
    if (vertical)
        rect = QRectF(-LETWIDTH/2 - 0.5, -(HEIGHT*(int)wid)/2 - 0.5, LETWIDTH + 1, HEIGHT*wid + 1);
    else
        rect = QRectF(-(LETWIDTH*(int)wid)/2 - 0.5, -HEIGHT/2 - 0.5, LETWIDTH*wid + 1, HEIGHT + 1);
    painter->setBrush(QBrush(QColor(Qt::white)));
    painter->setBackgroundMode(Qt::OpaqueMode);
    painter->drawRect(rect);
    painter->setBackgroundMode(Qt::TransparentMode);
    QString str = QString("%1").arg(val, wid, base, QChar('0'));
    if (vertical) {
        rect.setHeight(HEIGHT + 1);
        for (unsigned i = 0; i < wid; i++) {
            painter->drawText(rect, Qt::AlignCenter, QString(str[i]));
            // TODO this is probably broken (it is offseted)
            rect.setY(rect.y() + HEIGHT + 8);
        }
    } else
        painter->drawText(rect, Qt::AlignCenter, str);
}

void Value::value_update(std::uint32_t val)  {
    this->val = val;
    update();
}
