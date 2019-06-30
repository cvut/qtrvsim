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

#include "programcounter.h"
#include "fontsize.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define WIDTH 72
#define HEIGHT 25
#define PENW 1
//////////////////////

ProgramCounter::ProgramCounter(machine::QtMipsMachine *machine) : QGraphicsObject(nullptr), name("PC", this), value(this) {
    registers = machine->registers();

    QFont font;

    font.setPixelSize(FontSize::SIZE7);
    name.setPos(WIDTH/2 - name.boundingRect().width()/2, 0);
    name.setFont(font);
    font.setPointSize(FontSize::SIZE8);
    value.setText(QString("0x") + QString::number(machine->registers()->read_pc(), 16));
    value.setPos(1, HEIGHT - value.boundingRect().height());
    value.setFont(font);

    connect(machine->registers(), SIGNAL(pc_update(std::uint32_t)), this, SLOT(pc_update(std::uint32_t)));

    con_in = new Connector(Connector::AX_Y);
    con_out = new Connector(Connector::AX_Y);
    setPos(x(), y()); // To set initial connectors positions
}

ProgramCounter::~ProgramCounter() {
    delete con_in;
    delete con_out;
}

QRectF ProgramCounter::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW);
}

void ProgramCounter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    painter->drawRect(0, 0, WIDTH, HEIGHT);
}

void ProgramCounter::setPos(qreal x, qreal y) {
    QGraphicsObject::setPos(x, y);
    con_in->setPos(x + WIDTH/2, y + HEIGHT);
    con_out->setPos(x + WIDTH/2, y);
}

const Connector *ProgramCounter::connector_in() const {
    return con_in;
}

const Connector *ProgramCounter::connector_out() const {
    return con_out;
}

void ProgramCounter::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event __attribute__((unused))) {
    emit open_program();
    emit jump_to_pc(registers->read_pc());
}

void ProgramCounter::pc_update(std::uint32_t val) {
    value.setText(QString("0x") + QString::number(val, 16));
}
