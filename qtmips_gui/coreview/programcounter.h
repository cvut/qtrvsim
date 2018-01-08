#ifndef COREVIEW_PROGRAMCOUNTER_H
#define COREVIEW_PROGRAMCOUNTER_H

#include <QGraphicsObject>
#include <QPainter>
#include "qtmipsmachine.h"
#include "connection.h"

namespace coreview {

class ProgramCounter : public QGraphicsObject {
        Q_OBJECT
public:
    ProgramCounter(machine::QtMipsMachine *machine);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    const Connector *connector_in() const;
    const Connector *connector_out() const;

private slots:
    void pc_update(std::uint32_t val);

private:
    QGraphicsSimpleTextItem name;
    QGraphicsSimpleTextItem value;

    Connector *con_in, *con_out;
};

}


#endif // COREVIEW_PROGRAMCOUNTER_H
