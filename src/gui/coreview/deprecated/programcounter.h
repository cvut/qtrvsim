#ifndef COREVIEW_PROGRAMCOUNTER_H
#define COREVIEW_PROGRAMCOUNTER_H

#include "connection.h"
#include "machine/machine.h"
#include "machine/memory/address.h"

#include <QGraphicsObject>
#include <QPainter>

namespace coreview {

class ProgramCounter : public QGraphicsObject {
    Q_OBJECT
public:
    ProgramCounter(machine::Machine *machine);
    ~ProgramCounter() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setPos(qreal x, qreal y);
    const Connector *connector_in() const;
    const Connector *connector_out() const;

signals:
    void open_program();
    void jump_to_pc(machine::Address addr);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private slots:
    void pc_update(machine::Address val);

private:
    const machine::Registers *registers;

    QGraphicsSimpleTextItem name;
    QGraphicsSimpleTextItem value;

    Connector *con_in, *con_out;
};

} // namespace coreview

#endif // COREVIEW_PROGRAMCOUNTER_H
