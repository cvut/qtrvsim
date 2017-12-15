#ifndef CONNECTION_H
#define CONNECTION_H

#include <QGraphicsObject>
#include <QList>
#include <cmath>
#include "../coreview.h"

namespace coreview {

class Connector : public QObject {
    Q_OBJECT
public:
    void setPos(qreal x, qreal y);
    qreal x() const;
    qreal y() const;
    QPointF point() const;

signals:
    void updated();

private:
    qreal qx, qy;
};

class Connection : public QGraphicsObject {
    Q_OBJECT
public:
    Connection(const Connector *a, const Connector *b);

    void setHasText(bool has);
    void setText(QString val);

private slots:
    void moved();

private:
    QGraphicsSimpleTextItem *value;
    QList<QPointF> points;
    const Connector *a, *b;
    QString text;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // TODO line width and possibly bus width

    void update_pos();
};

}

#else

namespace coreview {
    class Connector;
    class Connection;
};

#endif // CONNECTION_H
