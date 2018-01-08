#include "junction.h"

using namespace coreview;

//////////////////////
#define DOT_SIZE 4
//////////////////////

Junction::Junction() : QGraphicsItem(nullptr) { }

Junction::~Junction() {
    for (int i = 0; i < cons.size(); i++)
        delete cons[i];
}

QRectF Junction::boundingRect() const {
    return QRectF(-DOT_SIZE/2, -DOT_SIZE/2, DOT_SIZE, DOT_SIZE);
}

void Junction::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    painter->setBrush(QBrush(QColor(0, 0, 0)));
    painter->drawEllipse(-DOT_SIZE/2, -DOT_SIZE/2, DOT_SIZE, DOT_SIZE);
}

void Junction::setPos(qreal x, qreal y) {
   QGraphicsItem::setPos(x, y);
   foreach (Connector *con, cons)
       con->setPos(x, y);
}

Connector *Junction::new_connector(qreal angle) {
    Connector*n = new Connector(angle);
    cons.append(n);
    setPos(x(), y()); // set connector's position
    return n;
}
