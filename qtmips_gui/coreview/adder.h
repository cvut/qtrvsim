#ifndef ADDER_H
#define ADDER_H

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSimpleTextItem>
#include "connection.h"

namespace coreview {

class Adder : public QGraphicsItem {
public:
    Adder();
    ~Adder();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    const Connector *connector_in_a() const;
    const Connector *connector_in_b() const;
    const Connector *connector_out() const;

private:
    QGraphicsSimpleTextItem plus;

    Connector *con_in_a, *con_in_b, *con_out;
};

}

#endif // ADDER_H
