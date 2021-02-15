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

#include "registers.h"

#include "coreview_colors.h"
#include "fontsize.h"

#include <cmath>

using namespace coreview;

//////////////////////
#define WIDTH 60
#define HEIGHT 80
#define PENW 1
//////////////////////

Registers::Registers() : QGraphicsObject(nullptr), name("Registers", this) {
    con_read1 = new Connector(Connector::AX_X);
    con_read1_reg = new Connector(Connector::AX_X);
    con_read2 = new Connector(Connector::AX_X);
    con_read2_reg = new Connector(Connector::AX_X);
    con_write = new Connector(Connector::AX_X);
    con_write_reg = new Connector(Connector::AX_X);
    con_ctl_write = new Connector(Connector::AX_Y);

    // TODO do we want to have any hooks on real registers?

    QFont font;
    font.setPixelSize(FontSize::SIZE7);
    name.setFont(font);

    QRectF name_box = name.boundingRect();
    name.setPos(
        WIDTH / 2 - name_box.width() / 2, HEIGHT / 2 - name_box.height() / 2);

    setPos(x(), y()); // set connector's position
}

Registers::~Registers() {
    delete con_read1;
    delete con_read1_reg;
    delete con_read2;
    delete con_read2_reg;
    delete con_write;
    delete con_write_reg;
    delete con_ctl_write;
}

QRectF Registers::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW);
}

void Registers::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option __attribute((unused)),
    QWidget *widget __attribute((unused))) {
    QPen pen = painter->pen();
    pen.setColor(BLOCK_OUTLINE_COLOR);
    painter->setPen(pen);

    painter->drawRect(0, 0, WIDTH, HEIGHT);
}

void Registers::setPos(qreal x, qreal y) {
    QGraphicsObject::setPos(x, y);

    con_read1_reg->setPos(x, y + 10);
    con_read2_reg->setPos(x, y + 30);
    con_read1->setPos(x + WIDTH, y + 10);
    con_read2->setPos(x + WIDTH, y + 30);

    con_write_reg->setPos(x, y + HEIGHT - 10);
    con_write->setPos(x, y + HEIGHT - 20);
    con_ctl_write->setPos(x + WIDTH - 10, y);
}

const Connector *Registers::connector_read1() const {
    return con_read1;
}

const Connector *Registers::connector_read1_reg() const {
    return con_read1_reg;
}

const Connector *Registers::connector_read2() const {
    return con_read2;
}

const Connector *Registers::connector_read2_reg() const {
    return con_read2_reg;
}

const Connector *Registers::connector_write() const {
    return con_write;
}

const Connector *Registers::connector_write_reg() const {
    return con_write_reg;
}

const Connector *Registers::connector_ctl_write() const {
    return con_ctl_write;
}

void Registers::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseDoubleClickEvent(event);
    emit open_registers();
}
