#ifndef INSTRUCTIONVIEW_H
#define INSTRUCTIONVIEW_H

#include <QGraphicsSimpleTextItem>
#include "qtmipsmachine.h"

namespace coreview {

class InstructionView : public QObject, public QGraphicsSimpleTextItem {
    Q_OBJECT
public:
    InstructionView();

public slots:
    void instruction_update(const machine::Instruction &i);
};

}

#endif // INSTRUCTIONVIEW_H
