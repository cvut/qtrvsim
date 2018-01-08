#ifndef COREVIEW_MEMORY_H
#define COREVIEW_MEMORY_H

#include <QGraphicsObject>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSimpleTextItem>
#include <qtmipsmachine.h>
#include "connection.h"

namespace coreview {

class Memory : public QGraphicsObject  {
    Q_OBJECT
public:
    Memory(machine::QtMipsMachine *machine);
    ~Memory();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    const Connector *connector_pc() const;
    const Connector *connector_inst() const;
    const Connector *connector_address() const;
    const Connector *connector_data_out() const;
    const Connector *connector_data_in() const;
    const Connector *connector_req_write() const;
    const Connector *connector_req_read() const;
    // TODO integrate cache

signals:
    void open_data_mem();
    void open_program_mem();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    // Connectors for instruction memory
    Connector *con_pc, *con_inst;
    // Connectors for data memory
    Connector *con_address, *con_data_out, *con_data_in, *con_req_write, *con_req_read;

    QGraphicsSimpleTextItem *name, *name_program, *name_data;
};

}

#endif // MEMORY_H
