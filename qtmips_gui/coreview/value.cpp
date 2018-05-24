#include "value.h"

using namespace coreview;

#define HEIGHT 8
#define LETWIDTH 7

// TODO orientation
Value::Value(bool vertical, unsigned width, std::uint32_t init_val) : QGraphicsObject(nullptr) {
    wid = width;
    val = init_val;
    this->vertical = vertical;
}

QRectF Value::boundingRect() const {
    if (vertical)
        return QRectF(-LETWIDTH/2 - 1, -HEIGHT*(int)wid/2 - 1, LETWIDTH + 2, HEIGHT*wid + 2);
    else
        return QRectF(-(LETWIDTH*(int)wid)/2 - 1, -HEIGHT/2 - 1, LETWIDTH*wid + 2, HEIGHT + 2);
}

void Value::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))){
    QFont f;
    f.setPointSize(7);
    painter->setFont(f);

    QRectF rect;
    if (vertical)
        rect = QRectF(-LETWIDTH/2 - 0.5, -(HEIGHT*(int)wid)/2 - 0.5, LETWIDTH + 1, HEIGHT*wid + 1);
    else
        rect = QRectF(-(LETWIDTH*(int)wid)/2 - 0.5, -HEIGHT/2 - 0.5, LETWIDTH*wid + 1, HEIGHT + 1);
    painter->setBrush(QBrush(QColor(Qt::white)));
    painter->setBackgroundMode(Qt::OpaqueMode);
    painter->drawRect(rect);
    painter->setBackgroundMode(Qt::TransparentMode);
    QString str = QString("%1").arg(val, wid, 16, QChar('0'));
    if (vertical) {
        rect.setHeight(HEIGHT + 1);
        for (unsigned i = 0; i < wid; i++) {
            painter->drawText(rect, Qt::AlignCenter, QString(str[i]));
            // TODO this is probably broken (it is offseted)
            rect.setY(rect.y() + HEIGHT + 8);
        }
    } else
        painter->drawText(rect, Qt::AlignCenter, str);
}

void Value::value_update(std::uint32_t val)  {
    this->val = val;
    update();
}
