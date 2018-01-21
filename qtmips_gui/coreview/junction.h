#ifndef JUNCTION_H
#define JUNCTION_H

#include <QGraphicsItem>
#include <QPainter>
#include <QList>
#include "connection.h"

namespace coreview {

class Junction : public QGraphicsItem {
public:
    Junction(bool point = true);
    ~Junction();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    Connector *new_connector(enum Connector::Axis axis = Connector::AX_X);

private:
    QList<Connector*> cons;
    bool point;
};

}

#endif // JUNCTION_H
