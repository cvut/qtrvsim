#include "alu.h"

//////////////////////
#define WIDTH 40
#define HEIGHT 120
#define DENT 10
#define PENW 1
//////////////////////

coreview::Alu::Alu() : QGraphicsObject(nullptr), name(this) {
    name.setText("ALU");
    name.setPos(3, 25);

    con_in_a = new Connector();
    con_in_b = new Connector();
    con_out = new Connector();
    con_ctl = new Connector();

    setPos(x(), y()); // set connector's position
}

QRectF coreview::Alu::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW);
}

void coreview::Alu::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
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
    QGraphicsObject::setPos(x, y);
    qreal off = ((HEIGHT/2) - DENT) / 2;
    con_in_a->setPos(0, off);
    con_in_b->setPos(0, HEIGHT - off);
    con_out->setPos(WIDTH, HEIGHT/2);
    con_ctl->setPos(WIDTH/2, HEIGHT - (WIDTH/2));
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
