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

#ifndef COREVIEW_PROGRAMCOUNTER_H
#define COREVIEW_PROGRAMCOUNTER_H

#include "connection.h"
#include "machine/machine.h"
#include "machine/memory/address.h"

#include <QGraphicsObject>
#include <QPainter>

namespace coreview {

class ProgramCounter : public QGraphicsObject {
    Q_OBJECT
public:
    ProgramCounter(machine::Machine *machine);
    ~ProgramCounter() override;

    QRectF boundingRect() const override;
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

    void setPos(qreal x, qreal y);
    const Connector *connector_in() const;
    const Connector *connector_out() const;

signals:
    void open_program();
    void jump_to_pc(machine::Address addr);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private slots:
    void pc_update(machine::Address val);

private:
    const machine::Registers *registers;

    QGraphicsSimpleTextItem name;
    QGraphicsSimpleTextItem value;

    Connector *con_in, *con_out;
};

} // namespace coreview

#endif // COREVIEW_PROGRAMCOUNTER_H
