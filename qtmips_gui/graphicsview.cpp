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

#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent) : Super(parent) {
    prev_height = 0;
    prev_width = 0;
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

void GraphicsView::setScene(QGraphicsScene *scene) {
    Super::setScene(scene);
    update_scale();
}

void GraphicsView::resizeEvent(QResizeEvent *event) {
    Super::resizeEvent(event);
    if ((width() != prev_height) || (height() != prev_width))
        update_scale();
}

void GraphicsView::update_scale() {
    if (scene() == nullptr)
        return; // Skip if we have no scene

    // Note: there is somehow three pixels error when viewing so we have to always compensate
    const int w = scene()->width() + 3;
    const int h = scene()->height() + 3;
    prev_height = width();
    prev_width = height();

    qreal scale = 1;
    if (height() > h && width() > w) {
        if (height() > width()) {
            scale = (qreal)width() / w;
        } else {
            scale = (qreal)height() / h;
        }
    }
    QTransform t;
    t.scale(scale, scale);
    setTransform(t, false);
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        // zoom
        const ViewportAnchor anchor = transformationAnchor();
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        int angle = event->angleDelta().y();
        qreal factor;
        if (angle > 0) {
            factor = 1.1;
        } else {
            factor = 0.9;
        }
        scale(factor, factor);
        setTransformationAnchor(anchor);
    } else {
        Super::wheelEvent(event);
    }
}

void GraphicsView::keyPressEvent(QKeyEvent *event) {
    qreal factor = 1.1;
    if (event->matches(QKeySequence::ZoomIn) ||
        (event->key() == Qt::Key_Equal) ||
        (event->key() == Qt::Key_Plus)) {
        scale(factor, factor);
    } else if (event->matches(QKeySequence::ZoomOut) ||
               (event->key() == Qt::Key_Minus)) {
        scale(1 / factor, 1 / factor);
    } else {
        Super::keyPressEvent(event);
    }
}
