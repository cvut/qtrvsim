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

#ifndef CACHEDOCK_H
#define CACHEDOCK_H

#include <QDockWidget>
#include <QLabel>
#include <QFormLayout>
#include "cacheview.h"
#include "graphicsview.h"
#include "qtmipsmachine.h"

class CacheDock : public QDockWidget {
    Q_OBJECT
public:
    CacheDock(QWidget *parent, const QString &type);

    void setup(const machine::Cache *cache);

private slots:
    void hit_update(unsigned);
    void miss_update(unsigned);
    void memory_reads_update(unsigned val);
    void memory_writes_update(unsigned val);
    void statistics_update(unsigned stalled_cycles, double speed_improv, double hit_rate);

private:
    QVBoxLayout *layout_box;
    QWidget *top_widget, *top_form;
    QFormLayout *layout_top_form;
    QLabel *l_hit, *l_miss, *l_stalled, *l_speed, *l_hit_rate;
    QLabel *no_cache;
    QLabel *l_m_reads, *l_m_writes;
    GraphicsView *graphicsview;
    CacheViewScene *cachescene;
};

#endif // CACHEDOCK_H
