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

#ifndef LOGICBLOCK_H
#define LOGICBLOCK_H

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSimpleTextItem>
#include <QVector>
#include "connection.h"

namespace coreview {

class LogicBlock : public QGraphicsItem {
public:
    LogicBlock(QString name);
    LogicBlock(QVector<QString> name);
    ~LogicBlock();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    void setSize(qreal width, qreal height);

    // This creates new connector
    // Position is determined by x and y in 0 to 1 range and is mapped to real size of this block
    // Using x=y and x=-y coordinates is not supported
    const Connector *new_connector(qreal x, qreal y);

private:
    QVector<QGraphicsSimpleTextItem*> text;
    QRectF box;

    struct Con {
        Connector *con;
        qreal x, y;
        QPointF p;
    };
    QVector<struct Con> connectors;
    QPointF con_pos(qreal x, qreal y);
};

}

#endif // LOGICBLOCK_H
