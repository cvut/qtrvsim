#include "adder.h"

#include "coreview_colors.h"
#include "fontsize.h"

using namespace coreview;

//////////////////////
#define WIDTH 13
#define HEIGHT 40
#define DENT 3
#define PENW 1
//////////////////////

Adder::Adder() : QGraphicsItem(nullptr), plus("+", this) {
    QFont font;
    font.setPixelSize(FontSize::SIZE7);
    plus.setFont(font);
    QRectF plus_box = plus.boundingRect();
    plus.setPos(
        DENT + (WIDTH - DENT) / 2 - plus_box.width() / 2,
        HEIGHT / 2 - plus_box.height() / 2);

    con_in_a = new Connector(Connector::AX_X);
    con_in_b = new Connector(Connector::AX_X);
    con_out = new Connector(Connector::AX_X);

    setPos(x(), y()); // set connector's position
}

Adder::~Adder() {
    delete con_in_a;
    delete con_in_b;
    delete con_out;
}

QRectF Adder::boundingRect() const {
    return { -PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW };
}

void Adder::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option __attribute__((unused)),
    QWidget *widget __attribute__((unused))) {
    const QPointF poly[] = { QPointF(0, 0),
                             QPointF(WIDTH, WIDTH),
                             QPointF(WIDTH, HEIGHT - WIDTH),
                             QPointF(0, HEIGHT),
                             QPointF(0, (HEIGHT / 2) + DENT),
                             QPointF(DENT, HEIGHT / 2),
                             QPointF(0, (HEIGHT / 2) - DENT) };

    QPen pen = painter->pen();
    pen.setColor(BLOCK_OUTLINE_COLOR);
    painter->setPen(pen);

    painter->drawPolygon(poly, sizeof(poly) / sizeof(QPointF));
}

void Adder::setPos(qreal x, qreal y) {
    QGraphicsItem::setPos(x, y);

    qreal off = ((HEIGHT / 2) - DENT) / 2;
    con_in_a->setPos(x, y + off);
    con_in_b->setPos(x, y + HEIGHT - off);
    con_out->setPos(x + WIDTH, y + HEIGHT / 2);
}

const Connector *Adder::connector_in_a() const {
    return con_in_a;
}

const Connector *Adder::connector_in_b() const {
    return con_in_b;
}

const Connector *Adder::connector_out() const {
    return con_out;
}
