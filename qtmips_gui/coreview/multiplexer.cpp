#include "multiplexer.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define WIDTH 10
#define HEIGHT 20
#define GAP 8
#define PENW 1
//////////////////////

Multiplexer::Multiplexer(unsigned size) {
    this->size = size;
    seton = 0;
    ctlfrom = false;
    con_ctl = new Connector(M_PI_2);
    con_out = new Connector(M_PI);
    con_in = new Connector*[size];
    for (unsigned i = 0; i < size; i++)
        con_in[i] = new Connector(0);
    setPos(x(), y()); // Set connectors possitions
}

Multiplexer::~Multiplexer() {
    delete con_ctl;
    delete con_out;
    for (unsigned i = 0; i < size; i++)
        delete con_in[i];
    delete con_in;
}

#define C_HEIGHT (HEIGHT + (GAP * (size -1 )))

QRectF Multiplexer::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, C_HEIGHT + PENW);
}

void Multiplexer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    painter->setPen(QColor(200, 200, 200));
    if (seton > 0 && seton <= size)
        painter->drawLine(0, (HEIGHT / 2) + ((seton - 1) * GAP), WIDTH, C_HEIGHT / 2);

    painter->setPen(QColor(0, 0, 0));
    const QPointF poly[] = {
        QPointF(0, 0),
        QPointF(WIDTH, WIDTH / 2),
        QPointF(WIDTH, C_HEIGHT - (WIDTH / 2)),
        QPointF(0, C_HEIGHT)
    };
    painter->drawPolygon(poly, sizeof(poly) / sizeof(QPointF));
}

void Multiplexer::setPos(qreal x, qreal y) {
    QGraphicsItem::setPos(x, y);
    if (ctlfrom)
        con_ctl->setPos(x + (WIDTH / 2), y + (WIDTH / 4));
    else
        con_ctl->setPos(x + (WIDTH / 2), y + C_HEIGHT - (WIDTH / 4));
    con_out->setPos(x + WIDTH, y + (C_HEIGHT / 2));
    for (unsigned i = 0; i < size; i++)
        con_in[i]->setPos(x, y + (HEIGHT / 2) + (i * GAP));
}

const Connector *Multiplexer::connector_ctl() const {
    return con_ctl;
}

const Connector *Multiplexer::connector_out() const {
    return con_out;
}

const Connector *Multiplexer::connector_in(unsigned i) const {
    SANITY_ASSERT(i < size, "Multiplexer: requested out of range input connector");
    return con_in[i];
}

void Multiplexer::set(unsigned i) {
    seton = i;
    update();
}

void Multiplexer::setCtl(bool up) {
    ctlfrom = up;
    setPos(x(), y()); // Update connectors
}
