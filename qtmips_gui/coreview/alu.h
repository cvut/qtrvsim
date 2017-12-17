#ifndef COREVIEW_ALU_H
#define COREVIEW_ALU_H

#include <QGraphicsObject>
#include <qtmipsmachine.h>
#include "../coreview.h"
#include "connection.h"

namespace coreview {

class Alu : public QGraphicsObject {
public:
    Alu();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    const Connector *connector_in_a() const;
    const Connector *connector_in_b() const;
    const Connector *connector_out() const;
    const Connector *connector_ctl() const;

private:
    QGraphicsSimpleTextItem name;
    Connector *con_in_a, *con_in_b, *con_out, *con_ctl;
};

}

#else

namespace coreview {
    class Alu;
}

#endif // COREVIEW_ALU_H
