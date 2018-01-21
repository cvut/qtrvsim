#include "alu.h"
#include <cmath>

//////////////////////
#define WIDTH 40
#define HEIGHT 120
#define DENT 10
#define PENW 1
//////////////////////

coreview::Alu::Alu() : QGraphicsItem(nullptr), name("ALU", this) {
    name.setPos(3, 25);
    QFont font;
    font.setPointSize(7);
    name.setFont(font);

    con_in_a = new Connector(Connector::AX_X);
    con_in_b = new Connector(Connector::AX_X);
    con_out = new Connector(Connector::AX_X);
    con_ctl = new Connector(Connector::AX_Y);

    setPos(x(), y()); // set connector's position
}

QRectF coreview::Alu::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW);
}

void coreview::Alu::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    const QPointF poly[] = {
        QPointF(0, 0),
        QPointF(WIDTH, WIDTH),
        QPointF(WIDTH, HEIGHT - WIDTH),
        QPointF(0, HEIGHT),
        QPointF(0, (HEIGHT/2) + DENT),
        QPointF(DENT, HEIGHT / 2),
        QPointF(0, (HEIGHT / 2) - DENT)
    };
    painter->drawPolygon(poly, sizeof(poly) / sizeof(QPointF));
}

void coreview::Alu::setPos(qreal x, qreal y) {
    QGraphicsItem::setPos(x, y);
    qreal off = ((HEIGHT/2) - DENT) / 2;
    con_in_a->setPos(x, y + off);
    con_in_b->setPos(x, y + HEIGHT - off);
    con_out->setPos(x + WIDTH, y + HEIGHT/2);
    con_ctl->setPos(x + WIDTH/2, y + HEIGHT - (WIDTH/2));
}

const coreview::Connector *coreview::Alu::connector_in_a() const {
    return con_in_a;
}

const coreview::Connector *coreview::Alu::connector_in_b() const {
    return con_in_b;
}

const coreview::Connector *coreview::Alu::connector_out() const {
    return con_out;
}

const coreview::Connector *coreview::Alu::connector_ctl() const {
    return con_ctl;
}
