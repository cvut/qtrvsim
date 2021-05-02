#ifndef INSTRUCTIONVIEW_H
#define INSTRUCTIONVIEW_H

#include "machine/machine.h"

#include <QGraphicsObject>
#include <QGraphicsSimpleTextItem>

namespace coreview {

class InstructionView : public QGraphicsObject {
    Q_OBJECT
public:
    InstructionView(QColor bgnd = QColor(240, 240, 240));

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public slots:
    void instruction_update(
        const machine::Instruction &i,
        machine::Address inst_addr,
        machine::ExceptionCause excause,
        bool valid);

private:
    QGraphicsSimpleTextItem text;
    machine::ExceptionCause excause;
    QColor bgnd;
    bool valid;
};

} // namespace coreview

#endif // INSTRUCTIONVIEW_H
