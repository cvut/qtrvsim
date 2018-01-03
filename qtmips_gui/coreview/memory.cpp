#include "memory.h"

using namespace coreview;

//////////////////////
#define WIDTH 80
#define HEIGHT 100
#define PENW 1
//////////////////////

Memory::Memory(machine::QtMipsMachine *machine) : QGraphicsObject(nullptr) {
#define CON_INIT(X) do { \
            X.in = new Connector(); \
            X.out = new Connector(); \
            X.read = new Connector(); \
            X.write = new Connector(); \
        } while(false)
    CON_INIT(con_program);
    CON_INIT(con_data);
#undef CON_INIT
    // TODO cache?
}

QRectF Memory::boundingRect() const {
    // TODO
}

void Memory::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // TODO
}

void Memory::setPos(qreal x, qreal y) {
    QGraphicsObject::setPos(x, y);
    // TODO con
}

const Connector *Memory::connector_in(bool program) const {
    if (program)
        return con_program.in;
    else
        return con_data.in;
}

const Connector *Memory::connector_out(bool program) const {
    if (program)
        return con_program.out;
    else
        return con_data.out;
}

const Connector *Memory::connector_read(bool program) const {
    if (program)
        return con_program.read;
    else
        return con_data.read;
}

const Connector *Memory::connector_write(bool program) const {
    if (program)
        return con_program.write;
    else
        return con_data.write;
}
