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

#include <QApplication>
#include "textsignalaction.h"

TextSignalAction::TextSignalAction(QObject *parent) : Super(parent) {
  connect(this, &TextSignalAction::triggered, this,
          &TextSignalAction::process_triggered);
}

TextSignalAction::TextSignalAction(const QString &text, QObject *parent) :
    Super(text, parent), signal_text(text) {
  connect(this, &TextSignalAction::triggered, this,
          &TextSignalAction::process_triggered);
}

TextSignalAction::TextSignalAction(const QString &text, const QString &signal_text,
                 QObject *parent) :
    Super(text, parent), signal_text(signal_text) {
  connect(this, &TextSignalAction::triggered, this,
          &TextSignalAction::process_triggered);
}

TextSignalAction::TextSignalAction(const QIcon &icon, const QString &text, QObject *parent) :
    Super(icon, text, parent), signal_text(text) {
  connect(this, &TextSignalAction::triggered, this,
          &TextSignalAction::process_triggered);
}

TextSignalAction::TextSignalAction(const QIcon &icon, const QString &text, const QString &signal_text,
                 QObject *parent) :
    Super(icon, text, parent), signal_text(signal_text) {
  connect(this, &TextSignalAction::triggered, this,
          &TextSignalAction::process_triggered);
}

void TextSignalAction::process_triggered(bool checked) {
    (void)checked;
    emit activated(signal_text);
}
