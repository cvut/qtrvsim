#ifndef COREVIEW_CONNECTION_H
#define COREVIEW_CONNECTION_H

#include <QGraphicsObject>
#include <QPainter>
#include <QVector>

namespace coreview {

class Connector : public QObject {
    Q_OBJECT
public:
    Connector(qreal angle);

    void setPos(qreal x, qreal y);
    qreal x() const;
    qreal y() const;
    QPointF point() const;
    QLineF vector() const;

    qreal angle() const;

signals:
    void updated(QPointF);

private:
    qreal ang;
    qreal qx, qy;
};

class Connection : public QGraphicsObject {
    Q_OBJECT
public:
    Connection(const Connector *start, const Connector *end);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setHasText(bool has);
    void setText(QString val);

    void setAxes(QVector<QLineF>);

private slots:
    void moved_start(QPointF);
    void moved_end(QPointF);

private:
    QGraphicsSimpleTextItem *value;
    QVector<QPointF> points;
    QPointF p_start, p_end;
    qreal ang_start, ang_end;
    QVector<QLineF> break_axes;
    QString text;

    int pen_width;

    // TODO line width and possibly bus width
    void recalc_line();
    void recalc_line_add_point(const QLineF &l1, const QLineF &l2);
};

#define CON_AXIS_X(Y) QLineF(QPointF(0, Y), QPointF(1, Y))
#define CON_AXIS_Y(X) QLineF(QPointF(X, 0), QPointF(X, 1))

}

#endif // COREVIEW_CONNECTION_H
