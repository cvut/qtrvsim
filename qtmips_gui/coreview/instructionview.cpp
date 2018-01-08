#include "instructionview.h"

using namespace coreview;

InstructionView::InstructionView() : QObject(), QGraphicsSimpleTextItem() { }

void InstructionView::instruction_update(machine::Instruction &i) {
    setText(i.to_str());
}
