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
    Memory(bool cache_used, const machine::Cache *cache);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void open_mem();
    void open_cache();

private slots:
    void cache_hit_update(unsigned);
    void cache_miss_update(unsigned);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    void set_type(const QString&);

    bool cache;

private:
    QGraphicsSimpleTextItem name, type;
    QGraphicsSimpleTextItem cache_t, cache_hit_t, cache_miss_t;
};

class ProgramMemory : public Memory {
    Q_OBJECT
public:
    ProgramMemory(machine::QtMipsMachine *machine);
    ~ProgramMemory();

    void setPos(qreal x, qreal y);

    const Connector *connector_address() const;
    const Connector *connector_instruction() const;

private:
    Connector *con_address, *con_inst;
};

class DataMemory : public Memory {
    Q_OBJECT
public:
    DataMemory(machine::QtMipsMachine *machine);
    ~DataMemory();

    void setPos(qreal x, qreal y);

    const Connector *connector_address() const;
    const Connector *connector_data_out() const;
    const Connector *connector_data_in() const;
    const Connector *connector_req_write() const;
    const Connector *connector_req_read() const;

private:
    Connector *con_address, *con_data_out, *con_data_in, *con_req_write, *con_req_read;
};

}

#endif // MEMORY_H
