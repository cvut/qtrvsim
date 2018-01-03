#ifndef COREVIEW_MEMORY_H
#define COREVIEW_MEMORY_H

#include <QGraphicsObject>
#include <QPainter>
#include <qtmipsmachine.h>
#include "../coreview.h"
#include "connection.h"

namespace coreview{

class Memory : public QGraphicsObject  {
    Q_OBJECT
public:
    Memory(machine::QtMipsMachine *machine);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    const Connector *connector_in(bool program) const;
    const Connector *connector_out(bool program) const;
    const Connector *connector_read(bool program) const;
    const Connector *connector_write(bool program) const;
    // TODO integrate cache

private:
    struct {
        Connector *in, *out, *read, *write;
    } con_program, con_data;
};

}

#else

namespace coreview {
    class Memory;
}

#endif // MEMORY_H
