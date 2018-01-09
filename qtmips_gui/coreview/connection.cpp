#include "connection.h"
#include <cmath>

using namespace coreview;

Connector::Connector(qreal angle) {
    ang = angle;
}

void Connector::setPos(qreal x, qreal y) {
    qx = x;
    qy = y;
    emit updated(QPointF(qx, qy));
}

qreal Connector::x() const {
    return qx;
}

qreal Connector::y() const {
    return qy;
}

QLineF Connector::vector() const {
    return QLineF(point(), QPointF(x() + cos(ang), y() + sin(ang)));
}

QPointF Connector::point() const {
    return QPointF(qx, qy);
}

qreal Connector::angle() const {
    return ang;
}

Connection::Connection(const Connector *a, const Connector *b) : QGraphicsObject(nullptr) {
    pen_width = 1;

    ang_start = a->angle();
    ang_end = b->angle();

    connect(a, SIGNAL(updated(QPointF)), this, SLOT(moved_start(QPointF)));
    connect(b, SIGNAL(updated(QPointF)), this, SLOT(moved_end(QPointF)));
    moved_start(a->point());
    moved_end(b->point());
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

void Connection::moved_start(QPointF p) {
    p_start = p;
    recalc_line();
}

void Connection::moved_end(QPointF p) {
    p_end = p;
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
    // TODO color?
    painter->setPen(pen);

    painter->drawPolyline(QPolygonF(points));
}

void Connection::recalc_line() {
    points.clear();

    points.append(p_start);

    QLineF cur_l(p_start, QPointF(p_start.x() + cos(ang_start), p_start.y() + sin(ang_start)));
    for (int i = 0; i < break_axes.size(); i++) {
        recalc_line_add_point(cur_l, break_axes[i]);
        cur_l = break_axes[i];
    }
    recalc_line_add_point(cur_l, QLineF(QPoint(p_end.x() + cos(ang_end), p_end.y() + sin(ang_end)), p_end));

    points.append(p_end);
}

void Connection::recalc_line_add_point(const QLineF &l1, const QLineF &l2) {
    QPointF intersec;
    if (l1.intersect(l2, &intersec) != QLineF::NoIntersection)
        points.append(intersec);
}
