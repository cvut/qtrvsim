#include "lcddisplayview.h"

#include <QPaintEvent>
#include <QPainter>
#include <QStyle>

LcdDisplayView::LcdDisplayView(QWidget *parent) : Super(parent) {
    setMinimumSize(100, 100);
    scale_x = 1.0;
    scale_y = 1.0;
}

void LcdDisplayView::setup(machine::LcdDisplay *lcd_display) {
    if (lcd_display == nullptr) { return; }
    connect(lcd_display, &machine::LcdDisplay::pixel_update, this, &LcdDisplayView::pixel_update);
    fb_pixels.reset(
        new QImage(lcd_display->get_width(), lcd_display->get_height(), QImage::Format_RGB32));
    fb_pixels->fill(qRgb(0, 0, 0));
    update_scale();
    update();
}

void LcdDisplayView::pixel_update(size_t x, size_t y, uint r, uint g, uint b) {
    int x1, y1, x2, y2;
    if (fb_pixels != nullptr) {
        fb_pixels->setPixel(x, y, qRgb(r, g, b));
        x1 = x * scale_x - 2;
        if (x1 < 0) { x1 = 0; }
        x2 = x * scale_x + 2;
        if (x2 > width()) { x2 = width(); }
        y1 = y * scale_y - 2;
        if (y1 < 0) { y1 = 0; }
        y2 = y * scale_y + 2;
        if (y2 > height()) { y2 = height(); }
        update(x1, y1, x2 - x1, y2 - y1);
    }
}

void LcdDisplayView::update_scale() {
    if (fb_pixels != nullptr) {
        if ((fb_pixels->width() != 0) && (fb_pixels->height() != 0)) {
            scale_x = (float)width() / fb_pixels->width();
            scale_y = (float)height() / fb_pixels->height();
            return;
        }
    }
    scale_x = 1.0;
    scale_y = 1.0;
}

void LcdDisplayView::paintEvent(QPaintEvent *event) {
    if (fb_pixels == nullptr) { return Super::paintEvent(event); }
    if (fb_pixels->width() == 0) { return Super::paintEvent(event); }

    QPainter painter(this);
    painter.drawImage(rect(), *fb_pixels);
#if 0
    painter.setPen(QPen(QColor(255, 255, 0)));
    painter.drawLine(event->rect().topLeft(),event->rect().topRight());
    painter.drawLine(event->rect().topLeft(),event->rect().bottomLeft());
    painter.drawLine(event->rect().topLeft(),event->rect().bottomRight());
#endif
}

void LcdDisplayView::resizeEvent(QResizeEvent *event) {
    Super::resizeEvent(event);
    update_scale();
}

uint LcdDisplayView::fb_width() {
    if (fb_pixels == nullptr) { return 0; }
    return fb_pixels->width();
}

uint LcdDisplayView::fb_height() {
    if (fb_pixels == nullptr) { return 0; }
    return fb_pixels->height();
}
