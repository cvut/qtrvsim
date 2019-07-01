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

#include "lcddisplaydock.h"
#include "lcddisplayview.h"

LcdDisplayDock::LcdDisplayDock(QWidget *parent, QSettings *settings) : Super(parent) {
    (void)settings;
    lcd_display_widget = new LcdDisplayView(this);
    QWidget *fill_widget = new QWidget(this);

    layout = new QBoxLayout(QBoxLayout::LeftToRight, fill_widget);
    // add spacer, then your widget, then spacer
    layout->addItem(new QSpacerItem(0, 0));
    layout->addWidget(lcd_display_widget);
    layout->addItem(new QSpacerItem(0, 0));
    fill_widget->setLayout(layout);
    setWidget(fill_widget);

    setObjectName("LCD Display");
    setWindowTitle("LCD Display");
}

LcdDisplayDock::~LcdDisplayDock() {
    delete lcd_display_widget;
}

void LcdDisplayDock::setup(machine::LcdDisplay *lcd_display) {
    lcd_display_widget->setup(lcd_display);
    update_layout(width(), height());
}

void LcdDisplayDock::update_layout(int w, int h) {
    // Keeping aspect ratio based on
    // https://stackoverflow.com/questions/30005540/keeping-the-aspect-ratio-of-a-sub-classed-qwidget-during-resize

    float thisAspectRatio = (float)w / h;
    int widgetStretch, outerStretch;
    float arWidth = lcd_display_widget->fb_width(); // aspect ratio width
    float arHeight = lcd_display_widget->fb_height(); // aspect ratio height

    if ((arWidth == 0) || (arHeight == 0)) {
        outerStretch = 0;
        widgetStretch = 1;
    }  else if (thisAspectRatio > (arWidth/arHeight)) { // too wide
        layout->setDirection(QBoxLayout::LeftToRight);
        widgetStretch = height() * (arWidth/arHeight); // i.e., my width
        outerStretch = (width() - widgetStretch) / 2 + 0.5;
    } else { // too tall
        layout->setDirection(QBoxLayout::TopToBottom);
        widgetStretch = width() * (arHeight/arWidth); // i.e., my height
        outerStretch = (height() - widgetStretch) / 2 + 0.5;
    }

    layout->setStretch(0, outerStretch);
    layout->setStretch(1, widgetStretch);
    layout->setStretch(2, outerStretch);
}

void LcdDisplayDock::resizeEvent(QResizeEvent *event) {
    // Keeping aspect ratio based on
    // https://stackoverflow.com/questions/30005540/keeping-the-aspect-ratio-of-a-sub-classed-qwidget-during-resize

    update_layout(event->size().width(), event->size().height());

    Super::resizeEvent(event);
}
