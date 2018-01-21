#ifndef COREVIEW_AND_H
#define COREVIEW_AND_H

#include <QGraphicsItem>
#include <QVector>
#include "connection.h"

namespace coreview {

class And : public QGraphicsItem {
public:
    And(unsigned size = 2);
    ~And();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    const Connector *connector_in(unsigned) const;
    const Connector *connector_out() const;

private:
    QVector<Connector*> connectors;
    Connector* con_out;
};

}

#endif // COREVIEW_AND_H
