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

#include "junction.h"

using namespace coreview;

//////////////////////
#define DOT_SIZE 4
//////////////////////

Junction::Junction(bool point) : QGraphicsItem(nullptr) {
    this->point = point;
}

Junction::~Junction() {
    for (int i = 0; i < cons.size(); i++)
        delete cons[i];
}

QRectF Junction::boundingRect() const {
    if (point)
        return QRectF(-DOT_SIZE/2, -DOT_SIZE/2, DOT_SIZE, DOT_SIZE);
    else
        return QRectF();
}

void Junction::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    if (!point)
        return;
    painter->setBrush(QBrush(QColor(0, 0, 0)));
    painter->setPen(QPen(Qt::NoPen)); // Disable pen (render only brush)
    painter->drawEllipse(-DOT_SIZE/2, -DOT_SIZE/2, DOT_SIZE, DOT_SIZE);
}

void Junction::setPos(qreal x, qreal y) {
   QGraphicsItem::setPos(x, y);
   foreach (Connector *con, cons)
       con->setPos(x, y);
}

Connector *Junction::new_connector(enum Connector::Axis axis) {
    Connector*n = new Connector(axis);
    cons.append(n);
    setPos(x(), y()); // set connector's position
    return n;
}
