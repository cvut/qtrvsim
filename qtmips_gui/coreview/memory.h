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

#ifndef COREVIEW_MEMORY_H
#define COREVIEW_MEMORY_H

#include <QGraphicsObject>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSimpleTextItem>
#include <qtmipsmachine.h>
#include "connection.h"

namespace coreview {

class Memory : public QGraphicsObject  {
    Q_OBJECT
public:
    Memory(bool cache_used, const machine::Cache *cache);

    QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

signals:
    void open_mem();
    void open_cache();

private slots:
    void cache_hit_update(unsigned);
    void cache_miss_update(unsigned);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void set_type(const QString&);

    bool cache;

private:
    QGraphicsSimpleTextItem name, type;
    QGraphicsSimpleTextItem cache_t, cache_hit_t, cache_miss_t;
};

class ProgramMemory : public Memory {
    Q_OBJECT
public:
    ProgramMemory(machine::QtMipsMachine *machine);
    ~ProgramMemory();

    void setPos(qreal x, qreal y);

    const Connector *connector_address() const;
    const Connector *connector_instruction() const;

private:
    Connector *con_address, *con_inst;
};

class DataMemory : public Memory {
    Q_OBJECT
public:
    DataMemory(machine::QtMipsMachine *machine);
    ~DataMemory();

    void setPos(qreal x, qreal y);

    const Connector *connector_address() const;
    const Connector *connector_data_out() const;
    const Connector *connector_data_in() const;
    const Connector *connector_req_write() const;
    const Connector *connector_req_read() const;

private:
    Connector *con_address, *con_data_out, *con_data_in, *con_req_write, *con_req_read;
};

}

#endif // MEMORY_H
