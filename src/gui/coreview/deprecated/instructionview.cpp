#include "instructionview.h"

#include "fontsize.h"
#include "machine/instruction.h"
#include "machine/machinedefs.h"
#include "machine/memory/address.h"

#include <QFont>
#include <QPainter>
#include <utility>

using namespace coreview;

//////////////////////
#define WIDTH 120
#define HEIGHT 14
#define ROUND 5
#define GAP 2
#define PENW 1
//////////////////////

InstructionView::InstructionView(QColor bgnd) : QGraphicsObject(nullptr), text(this) {
    QFont f;
    f.setPointSize(FontSize::SIZE6);
    text.setFont(f);
    this->bgnd = std::move(bgnd);
    valid = false;
    // Initialize to NOP
    instruction_update(
        machine::Instruction(), machine::Address::null(), machine::EXCAUSE_NONE, false);
}

QRectF InstructionView::boundingRect() const {
    return { -WIDTH / 2 - PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW };
}

void InstructionView::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option __attribute__((unused)),
    QWidget *widget __attribute__((unused))) {
    painter->setPen(QPen(QColor(240, 240, 240)));
    if (excause == machine::EXCAUSE_NONE) {
        if (valid) {
            painter->setBrush(QBrush(bgnd));
        } else {
            painter->setBrush(QBrush(QColor(240, 240, 240)));
        }
    } else {
        painter->setBrush(QBrush(QColor(255, 100, 100)));
    }
    painter->drawRoundedRect(-WIDTH / 2, 0, WIDTH, HEIGHT, ROUND, ROUND);
}

void InstructionView::instruction_update(
    const machine::Instruction &i,
    machine::Address inst_addr,
    machine::ExceptionCause excause,
    bool valid) {
    this->valid = valid;
    QRectF prev_box = boundingRect();
    text.setText(i.to_str(inst_addr));
    this->excause = excause;
    QRectF box = text.boundingRect();
    text.setPos(-box.width() / 2, GAP);
    update(prev_box.united(boundingRect()));
}
