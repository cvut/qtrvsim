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

#include "multitext.h"
#include "fontsize.h"
#include <QPainter>
#include <QFont>

using namespace coreview;

//////////////////////
#define WIDTH 100
#define HEIGHT 14
#define ROUND 5
#define GAP 2
#define PENW 1
//////////////////////

MultiText::MultiText(QMap<std::uint32_t, QString> value_map, bool nonzero_red) :
                 QGraphicsObject(nullptr), text(this) {
    QFont f;
    f.setPointSize(FontSize::SIZE6);
    text.setFont(f);

    this->value_map = value_map;
    this->nonzero_red = nonzero_red;

    multitext_update(0);
}

QRectF MultiText::boundingRect() const {
    return QRectF(-WIDTH/2 - PENW/2, -PENW/2, WIDTH + PENW, HEIGHT + PENW);
}

void MultiText::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    painter->setPen(QPen(QColor(240, 240, 240)));
    if (value != 0 && nonzero_red)
        painter->setBrush(QBrush(QColor(255, 100, 100)));
    else
        painter->setBrush(QBrush(QColor(240, 240, 240)));

    painter->drawRoundRect(-WIDTH/2, 0, WIDTH, HEIGHT, ROUND, ROUND);
}

void MultiText::multitext_update(std::uint32_t value) {

    QRectF prev_box = boundingRect();
    this->value = value;
    QString str = value_map.value(value);
    text.setText(str);
    QRectF box = text.boundingRect();
    text.setPos(-box.width()/2, GAP);
    update(prev_box.united(boundingRect()));
}
