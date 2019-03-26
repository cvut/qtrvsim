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

#include "instructionview.h"
#include "fontsize.h"

#include <QPainter>
#include <QFont>

using namespace coreview;

//////////////////////
#define WIDTH 120
#define HEIGHT 14
#define ROUND 5
#define GAP 2
#define PENW 1
//////////////////////

InstructionView::InstructionView(QColor bgnd) : QGraphicsObject(nullptr), text(this) {
    QFont f;
    f.setPointSize(FontSize::SIZE6);
    text.setFont(f);
    this->bgnd = bgnd;
    // Initialize to NOP
    instruction_update(machine::Instruction(), 0, machine::EXCAUSE_NONE);
}

QRectF InstructionView::boundingRect() const {
    return QRectF(-WIDTH/2 - PENW/2, -PENW/2, WIDTH + PENW, HEIGHT + PENW);
}

void InstructionView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    painter->setPen(QPen(QColor(240, 240, 240)));
    if (excause == machine::EXCAUSE_NONE)
        painter->setBrush(QBrush(bgnd));
    else
        painter->setBrush(QBrush(QColor(255, 100, 100)));
    painter->drawRoundRect(-WIDTH/2, 0, WIDTH, HEIGHT, ROUND, ROUND);
}

void InstructionView::instruction_update(const machine::Instruction &i,
                   std::uint32_t inst_addr, machine::ExceptionCause excause) {

    QRectF prev_box = boundingRect();
    text.setText(i.to_str(inst_addr));
    this->excause = excause;
    QRectF box = text.boundingRect();
    text.setPos(-box.width()/2, GAP);
    update(prev_box.united(boundingRect()));
}
