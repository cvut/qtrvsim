#include "instructionview.h"
#include <QPainter>
#include <QFont>

using namespace coreview;

//////////////////////
#define WIDTH 120
#define HEIGHT 14
#define ROUND 5
#define GAP 2
#define PENW 1
//////////////////////

InstructionView::InstructionView() : QGraphicsObject(nullptr), text(this) {
    QFont f;
    f.setPointSize(6);
    text.setFont(f);

    instruction_update(machine::Instruction()); // Initialize to NOP
}

QRectF InstructionView::boundingRect() const {
    return QRectF(-WIDTH/2 - PENW/2, -PENW/2, WIDTH + PENW, HEIGHT + PENW);
}

void InstructionView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    painter->setPen(QPen(QColor(240, 240, 240)));
    painter->setBrush(QBrush(QColor(240, 240, 240)));
    painter->drawRoundRect(-WIDTH/2, 0, WIDTH, HEIGHT, ROUND, ROUND);
}

void InstructionView::instruction_update(const machine::Instruction &i) {
    QRectF prev_box = boundingRect();
    text.setText(i.to_str());
    QRectF box = text.boundingRect();
    text.setPos(-box.width()/2, GAP);
    update(prev_box.united(boundingRect()));
}
