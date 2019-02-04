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

#include "and.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define GAP 5
#define PENW 1
//////////////////////

And::And(unsigned size) : QGraphicsItem(nullptr) {
    for (unsigned i = 0; i < size; i++)
        connectors.append(new Connector(Connector::AX_X));
    con_out = new Connector(Connector::AX_X);

    setPos(x(), y()); // update connectors positions
}

And::~And() {
    delete con_out;
    for (int i = 0; i < connectors.size(); i++)
        delete connectors[i];
}

QRectF And::boundingRect() const {
    qreal side = GAP * connectors.size();
    return QRectF(-PENW, -PENW, side + PENW, side + PENW);
}

void And::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    qreal size = GAP * connectors.size();
    painter->drawLine(0, 0, 0, size);
    painter->drawLine(0, 0, size/2, 0);
    painter->drawLine(0, size, size/2, size);
    painter->drawArc(0, 0, size, size, 270 * 16, 180 * 16);
}

void And::setPos(qreal x, qreal y) {
    QGraphicsItem::setPos(x, y);

    qreal size = GAP * connectors.size();
    con_out->setPos(x + size, y + size/2);
    for (int i = 0; i < connectors.size(); i++)
        connectors[i]->setPos(x, y + GAP/2 + GAP*i);
}

const Connector *And::connector_in(unsigned i) const {
    return connectors[i];
}

const Connector *And::connector_out() const {
    return con_out;
}
