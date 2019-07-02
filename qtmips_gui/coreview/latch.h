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

#ifndef COREVIEW_LATCH_H
#define COREVIEW_LATCH_H

#include <QGraphicsObject>
#include <QPropertyAnimation>
#include <QVector>
#include "qtmipsexception.h"
#include "qtmipsmachine.h"
#include "connection.h"

namespace coreview {

class Latch : public QGraphicsObject {
    Q_OBJECT
    Q_PROPERTY(QColor wedge_clr READ wedge_color WRITE set_wedge_color)
public:
    Latch(machine::QtMipsMachine *machine, qreal height);
    ~Latch();

    QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QColor wedge_color();
    void set_wedge_color(QColor &c);

    void setTitle(const QString &str);

    void setPos(qreal x, qreal y);

    struct ConnectorPair { Connector *in, *out; };

    struct ConnectorPair new_connector(qreal y); // Create new connectors pair that is given y from top of latch

protected:
    void updateCurrentValue(const QColor &color);

private slots:
    void tick();

private:
    qreal height;
    QVector<ConnectorPair> connectors;
    QVector<qreal> connectors_off;

    QGraphicsSimpleTextItem *title;

    QPropertyAnimation *wedge_animation;
    QColor wedge_clr;
};

}

#endif // COREVIEW_LATCH_H
