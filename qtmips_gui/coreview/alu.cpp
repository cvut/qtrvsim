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

#include "alu.h"
#include <cmath>

//////////////////////
#define WIDTH 25
#define HEIGHT 90
#define DENT 10
#define PENW 1
//////////////////////

coreview::Alu::Alu() : QGraphicsItem(nullptr), name("ALU", this) {
    name.setPos(3, 25);
    QFont font;
    font.setPointSize(7);
    name.setFont(font);

    con_in_a = new Connector(Connector::AX_X);
    con_in_b = new Connector(Connector::AX_X);
    con_out = new Connector(Connector::AX_X);
    con_ctl = new Connector(Connector::AX_Y);

    setPos(x(), y()); // set connector's position
}

QRectF coreview::Alu::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW);
}

void coreview::Alu::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    const QPointF poly[] = {
        QPointF(0, 0),
        QPointF(WIDTH, WIDTH),
        QPointF(WIDTH, HEIGHT - WIDTH),
        QPointF(0, HEIGHT),
        QPointF(0, (HEIGHT/2) + DENT),
        QPointF(DENT, HEIGHT / 2),
        QPointF(0, (HEIGHT / 2) - DENT)
    };
    painter->drawPolygon(poly, sizeof(poly) / sizeof(QPointF));
}

void coreview::Alu::setPos(qreal x, qreal y) {
    QGraphicsItem::setPos(x, y);
    qreal off = ((HEIGHT/2) - DENT) / 2;
    con_in_a->setPos(x, y + off);
    con_in_b->setPos(x, y + HEIGHT - off);
    con_out->setPos(x + WIDTH, y + HEIGHT/2);
    con_ctl->setPos(x + WIDTH/2, y + (WIDTH/2));
}

const coreview::Connector *coreview::Alu::connector_in_a() const {
    return con_in_a;
}

const coreview::Connector *coreview::Alu::connector_in_b() const {
    return con_in_b;
}

const coreview::Connector *coreview::Alu::connector_out() const {
    return con_out;
}

const coreview::Connector *coreview::Alu::connector_ctl() const {
    return con_ctl;
}
