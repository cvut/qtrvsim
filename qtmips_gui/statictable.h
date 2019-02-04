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

#ifndef STATICTABLE_H
#define STATICTABLE_H

#include <QWidget>
#include <QLayout>
#include <QList>
#include <QVector>

/*
 * This implements new layout and widget in the same time.
 * Basic idea is that we need some table view that can also fill in horizontal space.
 * This widget paints simple table under neath the widgets and lays them out in to them.
 * It shows more than one column when there is enought horizontal space.
 */

class StaticTableLayout : public QLayout {
public:
    StaticTableLayout(QWidget *parent, int margin = 4, int horizontal_big_spacing = 4, int horizontal_small_spacing = 8, int vertical_spacing = 4);
    ~StaticTableLayout();

    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect &rect) override;
    QSize sizeHint() const override;

    void addItem(QLayoutItem *item) override;
    QLayoutItem *itemAt(int index) const override;
    QLayoutItem *takeAt(int index) override;
    int count() const override; // This returns number of item blocks

    void addRow(QList<QWidget*>); // This adds row of widgets
    void insertRow(QList<QWidget*>, int i); // Insert row to given position while shifting all other up
    void removeRow(int i); // Remove row
    void clearRows(); // Clear all rows from table

    void itemRect(QRect &rect, QVector<int> &separators, int i); // This returns single item rectable (if expad_margin and it's on edge also count in margin)
    int columns();
    int real_row_height();

protected:
    int shspace, bhspace, vspace;
    QVector<QVector<QLayoutItem*>> items;

    int row_height;
    QList<QList<int>> row_widths;

    int layout_count_approx(const QRect &rect) const;
    int layout_size(int &row_h, QList<QList<int>> &row_w, int count) const;
    void layout_parms(QRect &rect, int &row_h, QList<QList<int>> &row_w, int &count) const;
    void do_layout(const QRect &rect);
    int layout_height(int width) const;

    QVector<QLayoutItem*> list2vec(QList<QWidget*>);

    struct {
        QSize size;
        int count;
    } cch_do_layout;
    mutable struct {
        int w, count;
        int width;
    } cch_heightForWidth;
    mutable struct {
        int count;
        QSize size;
    } cch_minSize;

};

class StaticTable : public QWidget {
public:
    StaticTable(QWidget *parent = nullptr);

    int count();
    void addRow(QList<QWidget*>);
    void insertRow(QList<QWidget*>, int i);
    void removeRow(int i);
    void clearRows();

    int columns();
    int row_size(); // return real row size (height) including spacing

protected:
    void paintEvent(QPaintEvent*);

    StaticTableLayout layout;
};

#endif // STATICTABLE_H
