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

#include "constant.h"

#include "fontsize.h"

#include <cmath>

using namespace coreview;

//////////////////////
#define OFFSET 6
//////////////////////

Constant::Constant(const Connector *con, const QString &text)
    : QGraphicsObject(nullptr)
    , text(text, this) {
    QFont font;
    font.setPixelSize(FontSize::SIZE7);
    this->text.setFont(font);

    con_our = new Connector(Connector::AX_X);
    conn = new Bus(con_our, con, 2);
    connect(
        con, QOverload<QPointF>::of(&Connector::updated), this,
        &Constant::ref_con_updated);
    ref_con_updated(con->point()); // update initial connector position
}

Constant::~Constant() {
    delete conn;
    delete con_our;
}

QRectF Constant::boundingRect() const {
    return conn->boundingRect();
}

void Constant::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget) {
    conn->paint(painter, option, widget);
}

void Constant::set_text(const QString &text) {
    this->text.setText(text);
    set_text_pos(); // update text positioning
}

void Constant::ref_con_updated(QPointF p) {
    con_our->setPos(p.x() - OFFSET, p.y());
    set_text_pos();
}

void Constant::set_text_pos() {
    // We are using here our known position of con_our
    QRectF box = text.boundingRect();
    text.setPos(
        con_our->x() - box.width() - 2, con_our->y() - box.height() / 2);
}
