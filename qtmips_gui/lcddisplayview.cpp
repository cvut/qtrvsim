#include <QPainter>
#include <QStyle>
#include "lcddisplay.h"
#include "lcddisplayview.h"

LcdDisplayView::LcdDisplayView(QWidget *parent) : Super(parent) {
    setMinimumSize(100, 100);
    fb_pixels = nullptr;
}

LcdDisplayView::~LcdDisplayView() {
    if (fb_pixels != nullptr)
        delete fb_pixels;
}

void LcdDisplayView::setup(machine::LcdDisplay *lcd_display) {
    if (lcd_display == nullptr)
        return;
    connect(lcd_display, SIGNAL(pixel_update(uint,uint,uint,uint,uint)),
            this, SLOT(pixel_update(uint,uint,uint,uint,uint)));
    if (fb_pixels != nullptr)
        delete fb_pixels;
    fb_pixels = nullptr;
    fb_pixels = new QImage(lcd_display->height(),
                           lcd_display->width(), QImage::Format_RGB32);
    fb_pixels->fill(qRgb(0, 0, 0));
    update();
}

void LcdDisplayView::pixel_update(uint x, uint y, uint r, uint g, uint b) {
    if (fb_pixels != nullptr) {
        fb_pixels->setPixel(x, y, qRgb(r, g, b));
        update();
    }
}

void LcdDisplayView::paintEvent(QPaintEvent *event) {
    if (fb_pixels == nullptr)
        return Super::paintEvent(event);
    if (fb_pixels->width() == 0)
        return Super::paintEvent(event);

    QPainter painter(this);
    QSize widgetSize = rect().size();
    const auto newHeight = widgetSize.width() * fb_pixels->height() / fb_pixels->width();
    if(newHeight <= widgetSize.height())
        widgetSize.setHeight(newHeight);
    else
       widgetSize.setWidth(widgetSize.height() * fb_pixels->width() / fb_pixels->height());
    painter.drawImage(rect(), fb_pixels->scaled(widgetSize));
}

