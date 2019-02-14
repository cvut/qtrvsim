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

#include <QString>
#include <QTextBlock>
#include <QTextCursor>

#include "serialport.h"
#include "terminaldock.h"

TerminalDock::TerminalDock(QWidget *parent, QSettings *settings) : QDockWidget(parent) {
    top_widget = new QWidget(this);
    setWidget(top_widget);
    layout_box = new QVBoxLayout(top_widget);

    terminal_text = new QTextEdit(top_widget);
    terminal_text->setMinimumSize(30, 30);
    layout_box->addWidget(terminal_text);
    append_cursor = new QTextCursor(terminal_text->document());

    setObjectName("Terminal");
    setWindowTitle("Terminal");
}

TerminalDock::~TerminalDock() {
    delete append_cursor;
}

void TerminalDock::setup(const machine::SerialPort *ser_port) {
    if (ser_port == nullptr)
        return;
    connect(ser_port, SIGNAL(tx_byte(uint)), this, SLOT(tx_byte(uint)));
}

void TerminalDock::tx_byte(unsigned int data) {
    if (data == '\n')
        append_cursor->insertBlock();
    else
        append_cursor->insertText(QString(QChar(data)));
}

void TerminalDock::tx_byte(int fd, unsigned int data)
{
    (void)fd;
    tx_byte(data);
}
