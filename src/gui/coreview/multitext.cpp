#include "multitext.h"

#include "fontsize.h"

#include <QFont>
#include <QPainter>
#include <utility>

using namespace coreview;

//////////////////////
#define WIDTH 100
#define HEIGHT 14
#define ROUND 5
#define GAP 2
#define PENW 1
//////////////////////

MultiText::MultiText(QMap<uint32_t, QString> value_map, bool nonzero_red)
    : QGraphicsObject(nullptr)
    , text(this) {
    QFont f;
    f.setPointSize(FontSize::SIZE6);
    text.setFont(f);

    this->value_map = std::move(value_map);
    this->nonzero_red = nonzero_red;

    multitext_update(0);
}

QRectF MultiText::boundingRect() const {
    return { -WIDTH / 2 - PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW };
}

void MultiText::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option __attribute__((unused)),
    QWidget *widget __attribute__((unused))) {
    painter->setPen(QPen(QColor(240, 240, 240)));
    if (value != 0 && nonzero_red) {
        painter->setBrush(QBrush(QColor(255, 100, 100)));
    } else {
        painter->setBrush(QBrush(QColor(240, 240, 240)));
    }

    painter->drawRoundedRect(-WIDTH / 2, 0, WIDTH, HEIGHT, ROUND, ROUND);
}

void MultiText::multitext_update(uint32_t value) {
    QRectF prev_box = boundingRect();
    this->value = value;
    QString str = value_map.value(value);
    text.setText(str);
    QRectF box = text.boundingRect();
    text.setPos(-box.width() / 2, GAP);
    update(prev_box.united(boundingRect()));
}
