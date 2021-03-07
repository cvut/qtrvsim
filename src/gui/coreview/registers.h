#ifndef COREVIEW_REGISTERS_H
#define COREVIEW_REGISTERS_H

#include "connection.h"

#include <QGraphicsObject>
#include <QGraphicsSimpleTextItem>
#include <QPainter>

namespace coreview {

class Registers : public QGraphicsObject {
    Q_OBJECT
public:
    Registers();
    ~Registers() override;

    QRectF boundingRect() const override;
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

    void setPos(qreal x, qreal y);
    const Connector *connector_read1() const;
    const Connector *connector_read1_reg() const;
    const Connector *connector_read2() const;
    const Connector *connector_read2_reg() const;
    const Connector *connector_write() const;
    const Connector *connector_write_reg() const;
    const Connector *connector_ctl_write() const;

signals:
    void open_registers();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    Connector *con_read1, *con_read1_reg, *con_read2, *con_read2_reg;
    Connector *con_write, *con_write_reg, *con_ctl_write;

    QGraphicsSimpleTextItem name;
};

} // namespace coreview

#endif // COREVIEW_REGISTERS_H
