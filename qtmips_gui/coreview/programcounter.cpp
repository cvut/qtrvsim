#include "programcounter.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define WIDTH 72
#define HEIGHT 25
#define PENW 1
//////////////////////

ProgramCounter::ProgramCounter(machine::QtMipsMachine *machine) : QGraphicsObject(nullptr), name("PC", this), value(this) {
    registers = machine->registers();

    QFont font;

    font.setPointSize(7);
    name.setPos(WIDTH/2 - name.boundingRect().width()/2, 0);
    name.setFont(font);
    font.setPointSize(8);
    value.setText(QString("0x") + QString::number(machine->registers()->read_pc(), 16));
    value.setPos(1, HEIGHT - value.boundingRect().height());
    value.setFont(font);

    connect(machine->registers(), SIGNAL(pc_update(std::uint32_t)), this, SLOT(pc_update(std::uint32_t)));

    con_in = new Connector(Connector::AX_Y);
    con_out = new Connector(Connector::AX_Y);
    setPos(x(), y()); // To set initial connectors positions
}

QRectF ProgramCounter::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW);
}

void ProgramCounter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    painter->drawRect(0, 0, WIDTH, HEIGHT);
}

void ProgramCounter::setPos(qreal x, qreal y) {
    QGraphicsObject::setPos(x, y);
    con_in->setPos(x + WIDTH/2, y + HEIGHT);
    con_out->setPos(x + WIDTH/2, y);
}

const Connector *ProgramCounter::connector_in() const {
    return con_in;
}

const Connector *ProgramCounter::connector_out() const {
    return con_out;
}

void ProgramCounter::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event __attribute__((unused))) {
    emit open_program();
    emit jump_to_pc(registers->read_pc());
}

void ProgramCounter::pc_update(std::uint32_t val) {
    value.setText(QString("0x") + QString::number(val, 16));
}
