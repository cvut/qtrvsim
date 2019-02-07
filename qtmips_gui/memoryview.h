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

#ifndef MEMORYVIEW_H
#define MEMORYVIEW_H

#include <QWidget>
#include <QAbstractScrollArea>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QList>
#include <QVector>
#include <cstdint>
#include <QResizeEvent>
#include "qtmipsmachine.h"
#include "statictable.h"

class MemoryView : public QWidget {
    Q_OBJECT
public:
    MemoryView(QWidget *parent = nullptr, std::uint32_t addr0 = 0);

    virtual void setup(machine::QtMipsMachine*);

    void set_focus(std::uint32_t address);
    std::uint32_t focus() const;
    std::uint32_t addr0() const;

    void edit_load_focus(); // Set current focus to edit field

public slots:
    void check_for_updates();

protected:
    const machine::Memory *memory;
    const machine::Cache *cache_data;

    virtual QList<QWidget*> row_widget(std::uint32_t address, QWidget *parent) = 0;

    QVBoxLayout *layout;

    void reload_content(); // reload displayed data
    void update_content(int count, int shift); // update content to match given count and shift

    virtual void addr0_save_change(std::uint32_t val);

private slots:
    void go_edit_finish();

private:
    std::uint32_t change_counter;
    std::uint32_t cache_data_change_counter;
    unsigned count;
    std::uint32_t addr_0; // First address in view

    class Frame : public QAbstractScrollArea {
    public:
        Frame(MemoryView *parent);

        StaticTable *widg;
        void focus(unsigned i); // Focus on given item in widget
        unsigned focussed(); // What item is in focus
        void check_update(); // Update widget size and content if needed

    protected:
        MemoryView *mv;
        int content_y;

        bool viewportEvent(QEvent*);
        void resizeEvent(QResizeEvent*);
        void wheelEvent(QWheelEvent *event);
    };
    Frame *memf;

    QWidget *ctl_widg;
    QLineEdit *go_edit;
};

#endif // MEMORYVIEW_H
