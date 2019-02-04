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

#ifndef COREVIEW_CONNECTION_H
#define COREVIEW_CONNECTION_H

#include <QGraphicsObject>
#include <QPainter>
#include <QVector>

namespace coreview {

class Connector : public QObject {
    Q_OBJECT
public:
    enum Axis {
        AX_X, // X axis
        AX_Y, // Y axis
        AX_XY, // X=Y axis (45°)
        AX_MXY // X=-Y axis (-45°)
    };

    Connector(enum Axis axis = AX_X);

    void setPos(qreal x, qreal y);
    void setPos(const QPointF&);

    enum Axis axis() const;
    qreal x() const;
    qreal y() const;

    QPointF point() const;
    QLineF vector() const;

signals:
    void updated(QPointF point);
    void updated(QLineF vector);

private:
    enum Axis ax;
    qreal qx, qy;
};

class Connection : public QGraphicsObject {
    Q_OBJECT
public:
    Connection(const Connector *start, const Connector *end);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setHasText(bool has);
    void setText(QString val);

    virtual void setAxes(QVector<QLineF>);

private slots:
    void moved_start(QLineF);
    void moved_end(QLineF);

protected:
    QGraphicsSimpleTextItem *value;
    QVector<QPointF> points;
    QLineF ax_start, ax_end;
    QVector<QLineF> break_axes;
    QString text;

    int pen_width;
    QColor color;

    void recalc_line();
    bool recalc_line_add_point(const QLineF &l1, const QLineF &l2);
};

class Bus : public Connection {
public:
    Bus(const Connector *start, const Connector *end, unsigned width = 4);
    ~Bus();

    void setAxes(QVector<QLineF>);

    // This creates connector snapped to closes point to x,y that is on bus
    const Connector *new_connector(qreal x, qreal y, enum Connector::Axis = Connector::AX_X);
    const Connector *new_connector(const QPointF&, enum Connector::Axis = Connector::AX_X);

protected:
    struct con_pos {
        Connector *c;
        QPointF p;
    };
    QVector<struct con_pos> conns;
    void conns_update();
    // TODO because of this we have to overload setAxis function and update that in there
};

class Signal : public Connection {
public:
    Signal(const Connector *start, const Connector *end);
};

#define CON_AX_X (coreview::Connector::AX_X)
#define CON_AX_Y (coreview::Connector::AX_Y)
#define CON_AX_XY (coreview::Connector::AX_XY)
#define CON_AX_MXY (coreview::Connector::AX_MXY)

#define CON_AXIS_X(Y) QLineF(QPointF(0, Y), QPointF(1, Y))
#define CON_AXIS_Y(X) QLineF(QPointF(X, 0), QPointF(X, 1))

}

#endif // COREVIEW_CONNECTION_H
