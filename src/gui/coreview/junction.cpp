#include "junction.h"

using namespace coreview;

//////////////////////
#define DOT_SIZE 4
//////////////////////

Junction::Junction(bool point, int dot_size) : QGraphicsItem(nullptr) {
    this->point = point;
    if (dot_size < 0) {
        dot_size = DOT_SIZE;
    }
    this->dot_size = dot_size;
}

Junction::~Junction() {
    for (auto &con : cons) {
        delete con;
    }
}

QRectF Junction::boundingRect() const {
    if (point) {
        return { -this->dot_size / 2.0, -this->dot_size / 2.0,
                 static_cast<qreal>(this->dot_size),
                 static_cast<qreal>(this->dot_size) };
    } else {
        return {};
    }
}

void Junction::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option __attribute__((unused)),
    QWidget *widget __attribute__((unused))) {
    if (!point) {
        return;
    }
    painter->setBrush(QBrush(QColor(0, 0, 0)));
    painter->setPen(QPen(Qt::NoPen)); // Disable pen (render only brush)
    painter->drawEllipse(
        -this->dot_size / 2, -this->dot_size / 2, this->dot_size,
        this->dot_size);
}

void Junction::setPos(qreal x, qreal y) {
    QGraphicsItem::setPos(x, y);
    foreach (Connector *con, cons)
        con->setPos(x, y);
}

Connector *Junction::new_connector(enum Connector::Axis axis) {
    Connector *n = new Connector(axis);
    cons.append(n);
    setPos(x(), y()); // set connector's position
    return n;
}
