#ifndef INSTRUCTIONVIEW_H
#define INSTRUCTIONVIEW_H

#include <QGraphicsObject>
#include <QGraphicsSimpleTextItem>
#include "qtmipsmachine.h"

namespace coreview {

class InstructionView : public QGraphicsObject {
    Q_OBJECT
public:
    InstructionView();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:
    void instruction_update(const machine::Instruction &i);

private:
    QGraphicsSimpleTextItem text;
};

}

#endif // INSTRUCTIONVIEW_H
