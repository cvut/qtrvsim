#include "instructionview.h"
#include <QFont>

using namespace coreview;

InstructionView::InstructionView() : QObject(), QGraphicsSimpleTextItem() {
    QFont f;
    f.setPointSize(8);
    setFont(f);
    instruction_update(machine::Instruction());
}

void InstructionView::instruction_update(const machine::Instruction &i) {
    setText(i.to_str());
}
