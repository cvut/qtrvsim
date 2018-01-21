#include "connection.h"
#include <cmath>

using namespace coreview;

Connector::Connector(enum Axis ax) {
    this->ax = ax;
}

void Connector::setPos(qreal x, qreal y) {
    qx = x;
    qy = y;
    emit updated(point());
    emit updated(vector());
}

enum Connector::Axis Connector::axis() const {
    return ax;
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

QLineF Connector::vector() const {
    QPointF p = point();
    switch (ax) {
    case AX_X:
        return QLineF(p, p + QPointF(1, 0));
    case AX_Y:
        return QLineF(p, p + QPointF(0, 1));
    case AX_XY:
        return QLineF(p, p + QPointF(1, 1));
    case AX_MXY:
        return QLineF(p, p + QPoint(1, -1));
    }
}

Connection::Connection(const Connector *a, const Connector *b) : QGraphicsObject(nullptr) {
    pen_width = 1;

    ax_start = a->vector();
    ax_end = a->vector();

    connect(a, SIGNAL(updated(QLineF)), this, SLOT(moved_start(QLineF)));
    connect(b, SIGNAL(updated(QLineF)), this, SLOT(moved_end(QLineF)));
    moved_start(a->vector());
    moved_end(b->vector());
}

void Connection::setHasText(bool has) {
    if (has && value == nullptr) {
        value = new QGraphicsSimpleTextItem(this);
        value->setText(text);
    } else if (!has && value != nullptr) {
        delete value;
    }
}

void Connection::setText(QString val) {
    text = val;
    if (value != nullptr)
        value->setText(val);
}

void Connection::setAxes(QVector<QLineF> axes) {
    break_axes = axes;
    recalc_line();
}

void Connection::moved_start(QLineF p) {
    ax_start = p;
    recalc_line();
}

void Connection::moved_end(QLineF p) {
    ax_end = p;
    recalc_line();
}

QRectF Connection::boundingRect() const {
    QRectF rect;
    for (int i = 0; i < (points.size() - 1); i++) {
        qreal x = points[i].x() > points[i+1].x() ? points[i].x() : points[i+1].x();
        qreal y = points[i].y() > points[i+1].y() ? points[i].y() : points[i+1].y();
        rect |= QRectF(x - pen_width/2.0, y - pen_width/2.0, fabs(points[i].x() - points[i+1].x()) + pen_width, fabs(points[i].y() - points[i+1].y()) + pen_width);
    }
    return rect;
}

void Connection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    QPen pen;
    pen.setWidth(pen_width);
    pen.setColor(color);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::BevelJoin);
    painter->setPen(pen);

    painter->drawPolyline(QPolygonF(points));
}

void Connection::recalc_line() {
    points.clear();

    points.append(ax_start.p1());

    QLineF cur_l = ax_start;
    for (int i = 0; i < break_axes.size(); i++) {
        if (recalc_line_add_point(cur_l, break_axes[i]))
            cur_l = break_axes[i];
    }
    recalc_line_add_point(cur_l, ax_end);

    points.append(ax_end.p1());
}

bool Connection::recalc_line_add_point(const QLineF &l1, const QLineF &l2) {
    QPointF intersec;
    if (l1.intersect(l2, &intersec) == QLineF::NoIntersection)
        return false;
    points.append(intersec);
    return true;
}

Bus::Bus(const Connector *start, const Connector *end, unsigned width) : Connection(start, end) {
    pen_width = width;
}

const Connector *Bus::new_connector(qreal x, qreal y, enum Connector::Axis axis) {
    Connector *c = new Connector(axis);
    conns.append({
        .c = c,
        .p = QPoint(x, y)
     });
    // TODO update positions
    return c;
}

const Connector *Bus::new_connector(const QPointF &p, enum Connector::Axis axis) {
    return new_connector(p.x(), p.y(), axis);
}

Signal::Signal(const Connector *start, const Connector *end) : Connection(start, end) {
    color = QColor(0, 0, 255);
}
