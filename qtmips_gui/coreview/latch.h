#ifndef LATCH_H
#define LATCH_H

#include <QGraphicsObject>
#include <QList>
#include "qtmipsexception.h"
#include "qtmipsmachine.h"
#include "../coreview.h"
#include "connection.h"

namespace coreview {

class Latch : public QGraphicsObject {
    Q_OBJECT
public:
    Latch(machine::QtMipsMachine *machine, qreal height);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);

    struct ConnectorPair { Connector *in, *out; };

    struct ConnectorPair new_connector(qreal y); // Create new connectors pair that is given y from top of latch

private slots:
    void tick();

private:
    qreal height;
    QList<ConnectorPair> connectors;
    QList<qreal> connectors_off;

};

}

#else

namespace coreview {
    class Latch;
};

#endif // LATCH_H
