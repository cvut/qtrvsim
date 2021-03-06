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

#include "messagesview.h"

#include "messagesmodel.h"

#include <QApplication>
#include <QClipboard>
#include <QFontMetrics>
#include <QHeaderView>
#include <QKeyEvent>
#include <QScrollBar>

MessagesView::MessagesView(QWidget *parent, QSettings *settings)
    : Super(parent) {
    this->settings = settings;
}

void MessagesView::resizeEvent(QResizeEvent *event) {
    Super::resizeEvent(event);
}

void MessagesView::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Copy)) {
        QString text;
        QItemSelectionRange range = selectionModel()->selection().first();
        for (auto i = range.top(); i <= range.bottom(); ++i) {
            QStringList rowContents;
            for (auto j = range.left(); j <= range.right(); ++j) {
                rowContents << model()->index(i, j).data().toString();
            }
            text += rowContents.join("\t");
            text += "\n";
        }
        QApplication::clipboard()->setText(text);
    } else {
        Super::keyPressEvent(event);
    }
}
