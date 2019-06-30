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

#ifndef PROGRAMTABLEVIEW_H
#define PROGRAMTABLEVIEW_H

#include <QObject>
#include <QSettings>
#include <QTableView>
#include <QSharedPointer>

class ProgramTableView : public QTableView
{
    Q_OBJECT

    using Super = QTableView;

public:
    ProgramTableView(QWidget *parent, QSettings *settings);

    void resizeEvent(QResizeEvent *event) override;
signals:
    void address_changed(std::uint32_t address);
    void adjust_scroll_pos_queue();
public slots:
    void go_to_address(std::uint32_t address);
    void focus_address(std::uint32_t address);
    void focus_address_with_save(std::uint32_t address);
protected:
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void adjust_scroll_pos_check();
    void adjust_scroll_pos_process();
private:
    void go_to_address_priv(std::uint32_t address);
    void addr0_save_change(std::uint32_t val);
    void adjustColumnCount();
    QSettings *settings;

    std::uint32_t initial_address;
    bool adjust_scroll_pos_in_progress;
    bool need_addr0_save;
};

#endif // PROGRAMTABLEVIEW_H
