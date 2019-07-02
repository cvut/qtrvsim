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

#ifndef COREVIEW_MULTIPLEXER_H
#define COREVIEW_MULTIPLEXER_H

#include <QGraphicsObject>
#include "qtmipsexception.h"
#include "connection.h"

namespace coreview {

class Multiplexer : public  QGraphicsObject {
    Q_OBJECT
public:
    Multiplexer(unsigned size, bool ctl_up = false);
    ~Multiplexer();

    QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setPos(qreal x, qreal y);
    const Connector *connector_ctl() const; // Control input
    const Connector *connector_out() const; // Output
    const Connector *connector_in(unsigned i) const; // Inputs

public slots:
    void set(std::uint32_t i); // Set what value should be set as connected (indexing from  1 where 0 is no line)

private:
    bool ctlfrom;
    unsigned size;
    int seton;
    Connector *con_ctl, *con_out, **con_in;
};

}

#endif // COREVIEW_MULTIPLEXER_H
