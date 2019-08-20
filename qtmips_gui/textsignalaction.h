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

#ifndef TEXTSIGNALACTION_H
#define TEXTACTION_H

#include <QObject>
#include <QAction>

class TextSignalAction : public QAction
{
    Q_OBJECT

    using Super = QAction;

public:
    TextSignalAction(QObject *parent = nullptr);
    TextSignalAction(const QString &text, QObject *parent = nullptr);
    TextSignalAction(const QString &text, const QString &signal_text,
                     QObject *parent = nullptr);
    TextSignalAction(const QIcon &icon, const QString &text, QObject *parent = nullptr);
    TextSignalAction(const QIcon &icon, const QString &text, const QString &signal_text,
                     QObject *parent = nullptr);
signals:
    void activated(QString signal_text);

protected slots:
    void  process_triggered(bool checked);

protected:
    QString signal_text;
};

#endif // TEXTSIGNALACTION_H
