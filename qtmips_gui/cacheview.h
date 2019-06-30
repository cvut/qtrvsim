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

#ifndef CACHEVIEW_H
#define CACHEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include "graphicsview.h"
#include "qtmipsmachine.h"


class CacheAddressBlock : public QGraphicsObject {
    Q_OBJECT
public:
    CacheAddressBlock(const machine::Cache *cache, unsigned width);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    void cache_update(unsigned associat, unsigned set, unsigned col, bool valid, bool dirty,
                      std::uint32_t tag, const std::uint32_t *data, bool write);

private:
    unsigned rows, columns;
    unsigned tag, row, col;
    unsigned s_tag, s_row, s_col;
    unsigned width;
};

class CacheViewBlock : public QGraphicsObject {
    Q_OBJECT
public:
    CacheViewBlock(const machine::Cache *cache, unsigned block, bool last);
    ~CacheViewBlock();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    void cache_update(unsigned associat, unsigned set, unsigned col, bool valid, bool dirty,
                      std::uint32_t tag, const std::uint32_t *data, bool write);

private:
    bool islast;
    unsigned block;
    unsigned rows, columns;
    QGraphicsSimpleTextItem **validity, **dirty, **tag, ***data;
    unsigned curr_row;
    bool last_highlighted;
    unsigned last_set;
    unsigned last_col;
};

class CacheViewScene : public QGraphicsScene {
    Q_OBJECT
public:
    CacheViewScene(const machine::Cache *cache);
    ~CacheViewScene();

private:
    unsigned associativity;
    CacheViewBlock **block;
    CacheAddressBlock *ablock;
};

#endif // CACHEVIEW_H
