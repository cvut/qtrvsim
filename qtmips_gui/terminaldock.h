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

#ifndef TERMINALDOCK_H
#define TERMINALDOCK_H

#include <QDockWidget>
#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextCursor>
#include "qtmipsmachine.h"

class TerminalDock : public QDockWidget {
    Q_OBJECT
public:
    TerminalDock(QWidget *parent, QSettings *settings);
    ~TerminalDock();

    void setup(machine::SerialPort *ser_port);

public slots:
    void tx_byte(unsigned int data);
    void tx_byte(int fd, unsigned int data);
    void rx_byte_pool(int fd, unsigned int &data, bool &available);

private:
    QVBoxLayout *layout_box;
    QHBoxLayout *layout_bottom_box;
    QWidget *top_widget, *top_form;
    QFormLayout *layout_top_form;
    QTextEdit *terminal_text;
    QTextCursor *append_cursor;
    QLineEdit *input_edit;
};

#endif // TERMINALDOCK_H
