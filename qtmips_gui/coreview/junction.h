#ifndef JUNCTION_H
#define JUNCTION_H

#include <QGraphicsItem>
#include <QPainter>
#include <QList>
#include "connection.h"

namespace coreview {

class Junction : public QGraphicsItem {
public:
    Junction();
    ~Junction();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    Connector *new_connector(qreal angle);

private:
    QList<Connector*> cons;
};

}

#endif // JUNCTION_H
