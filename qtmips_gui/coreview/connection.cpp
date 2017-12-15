#include "connection.h"

using namespace coreview;

void Connector::setPos(qreal x, qreal y) {
    qx = x;
    qy = y;
    emit updated();
}

qreal Connector::x() const {
    return qx;
}

qreal Connector::y() const {
    return qy;
}

QPointF Connector::point() const {
    return QPointF(qx, qy);
}

Connection::Connection(const Connector *a, const Connector *b) : QGraphicsObject(nullptr) {
    connect(a, SIGNAL(updated()), this, SLOT(moved()));
    connect(b, SIGNAL(updated()), this, SLOT(moved()));
    this->a = a;
    this->b = b;
    update_pos();
}

void Connection::setHasText(bool has) {
    if (has && value == nullptr) {
        value = new QGraphicsSimpleTextItem(this);
        value->setText(text);
    } else if (!has && value != nullptr) {
        delete value;
    }
    update_pos();
}

void Connection::setText(QString val) {
    text = val;
    if (value != nullptr)
        value->setText(val);
}

void Connection::moved() {
    update_pos();
}

QRectF Connection::boundingRect() const {
    QRectF rect;
    for (int i = 0; i < (points.size() - 1); i++) {
        qreal x = points[i].x();
        if (x > points[i+1].x())
            x = points[i+1].x();
        qreal y = points[i].y();
        if (y > points[i+1].y())
            y = points[i+1].y();
        // TODO pen width
        rect.united(QRectF(x - 0.5, y - 0.5, fabs(points[i].x() - points[i+1].x()) + 1, fabs(points[i].y() - points[i+1].y()) + 1));
    }
    //return rect;
    return QRectF(0, 0, 300, 300);
}

void Connection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    for (int i = 0; i < (points.size() - 1); i++)
        painter->drawLine(points[i], points[i+1]);
    // TODO meaby use QPath instead?
}

void Connection::update_pos() {
    points.clear();
    points.append(a->point());
    points.append(b->point());
    // TODO more than one line
    // TODO update position of value
}
