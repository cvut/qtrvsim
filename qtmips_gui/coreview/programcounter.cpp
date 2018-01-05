#include "programcounter.h"

using namespace coreview;

//////////////////////
#define WIDTH 80
#define HEIGHT 50
#define PENW 1
//////////////////////

ProgramCounter::ProgramCounter(machine::QtMipsMachine *machine) : QGraphicsObject(nullptr), value(this), name(this) {
    value.setText(QString("0x") + QString::number(machine->registers()->read_pc(), 16));
    value.setPos(1, HEIGHT/2 - value.boundingRect().height()/2);
    name.setText(QString("PC"));
    name.setPos(WIDTH/2 - name.boundingRect().width()/2, 0);

    connect(machine->registers(), SIGNAL(pc_update(std::uint32_t)), this, SLOT(pc_update(std::uint32_t)));

    con_in = new Connector();
    con_out = new Connector();
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
    con_in->setPos(x, y + HEIGHT/2);
    con_out->setPos(x + WIDTH, y + HEIGHT/2);
}

const Connector *ProgramCounter::connector_in() const {
    return con_in;
}

const Connector *ProgramCounter::connector_out() const {
    return con_out;
}

void ProgramCounter::pc_update(std::uint32_t val) {
    value.setText(QString("0x") + QString::number(val, 16));
}
