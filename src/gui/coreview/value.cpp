#include "value.h"

#include "coreview_colors.h"
#include "fontsize.h"

using namespace coreview;

#define HEIGHT 8
#define LETWIDTH 7

// TODO orientation
Value::Value(
    bool vertical,
    unsigned width,
    uint32_t init_val,
    unsigned a_base,
    QChar fillchr,
    bool frame)
    : QGraphicsObject(nullptr) {
    wid = width;
    val = init_val;
    base = a_base;
    this->vertical = vertical;
    this->fillchr = fillchr;
    this->frame = frame;
}

QRectF Value::boundingRect() const {
    if (vertical) {
        return { -LETWIDTH / 2.0 - 1.0, -HEIGHT * (int)wid / 2.0 - 1.0,
                 LETWIDTH + 2, HEIGHT * wid + 2.0 };
    } else {
        return { -(LETWIDTH * (int)wid) / 2.0 - 1.0, -HEIGHT / 2.0 - 1.0,
                 LETWIDTH * wid + 2.0, HEIGHT + 2.0 };
    }
}

void Value::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option __attribute__((unused)),
    QWidget *widget __attribute__((unused))) {
    QFont f;
    f.setPixelSize(FontSize::SIZE7);
    painter->setFont(f);

    QRectF rect;
    if (vertical) {
        rect = QRectF(
            -LETWIDTH / 2 - 0.5, -(HEIGHT * (int)wid) / 2 - 0.5, LETWIDTH + 1,
            HEIGHT * wid + 1);
    } else {
        rect = QRectF(
            -(LETWIDTH * (int)wid) / 2 - 0.5, -HEIGHT / 2 - 0.5,
            LETWIDTH * wid + 1, HEIGHT + 1);
    }
    painter->setBrush(QBrush(QColor(Qt::white)));
    painter->setBackgroundMode(Qt::OpaqueMode);
    if (!frame) {
        painter->setPen(QColor(Qt::white));
    } else {
        painter->setPen(QColor(BLOCK_OUTLINE_COLOR));
    }
    painter->drawRect(rect);
    painter->setPen(QColor(Qt::black));
    painter->setBackgroundMode(Qt::TransparentMode);
    QString str = QString("%1").arg(val, wid, base, fillchr);
    if (vertical) {
        rect.setHeight(HEIGHT + 1);
        for (unsigned i = 0; i < wid; i++) {
            painter->drawText(rect, Qt::AlignCenter, QString(str[i]));
            // TODO this is probably broken (it is offseted)
            rect.translate(0, HEIGHT);
        }
    } else {
        painter->drawText(rect, Qt::AlignCenter, str);
    }
}

void Value::value_update(uint32_t val) {
    this->val = val;
    update();
}
