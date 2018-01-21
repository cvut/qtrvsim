#include "and.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define GAP 5
#define PENW 1
//////////////////////

And::And(unsigned size) : QGraphicsItem(nullptr) {
    for (unsigned i = 0; i < size; i++)
        connectors.append(new Connector(Connector::AX_X));
    con_out = new Connector(Connector::AX_X);

    setPos(x(), y()); // update connectors positions
}

And::~And() {
    delete con_out;
    for (int i = 0; i < connectors.size(); i++)
        delete connectors[i];
}

QRectF And::boundingRect() const {
    qreal side = GAP * connectors.size();
    return QRectF(-PENW, -PENW, side + PENW, side + PENW);
}

void And::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    qreal size = GAP * connectors.size();
    painter->drawLine(0, 0, 0, size);
    painter->drawLine(0, 0, size/2, 0);
    painter->drawLine(0, size, size/2, size);
    painter->drawArc(0, 0, size, size, 270 * 16, 180 * 16);
}

void And::setPos(qreal x, qreal y) {
    QGraphicsItem::setPos(x, y);

    qreal size = GAP * connectors.size();
    con_out->setPos(x + size, y + size/2);
    for (int i = 0; i < connectors.size(); i++)
        connectors[i]->setPos(x, y + GAP/2 + GAP*i);
}

const Connector *And::connector_in(unsigned i) const {
    return connectors[i];
}

const Connector *And::connector_out() const {
    return con_out;
}
