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

#include "adder.h"
#include "fontsize.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define WIDTH 13
#define HEIGHT 40
#define DENT 3
#define PENW 1
//////////////////////

Adder::Adder() : QGraphicsItem(nullptr), plus("+", this) {
    QFont font;
    font.setPointSize(FontSize::SIZE7);
    plus.setFont(font);
    QRectF plus_box = plus.boundingRect();
    plus.setPos(DENT + (WIDTH-DENT)/2 - plus_box.width()/2, HEIGHT/2 - plus_box.height()/2);

    con_in_a = new Connector(Connector::AX_X);
    con_in_b = new Connector(Connector::AX_X);
    con_out = new Connector(Connector::AX_X);

    setPos(x(), y()); // set connector's position
}

Adder::~Adder() {
    delete con_in_a;
    delete con_in_b;
    delete con_out;
}

QRectF Adder::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW);
}

void Adder::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
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

void Adder::setPos(qreal x, qreal y) {
    QGraphicsItem::setPos(x, y);

    qreal off = ((HEIGHT/2) - DENT) / 2;
    con_in_a->setPos(x, y + off);
    con_in_b->setPos(x, y + HEIGHT - off);
    con_out->setPos(x + WIDTH, y + HEIGHT/2);
}

const Connector *Adder::connector_in_a() const {
    return con_in_a;
}

const Connector *Adder::connector_in_b() const {
    return con_in_b;
}

const Connector *Adder::connector_out() const {
    return con_out;
}
