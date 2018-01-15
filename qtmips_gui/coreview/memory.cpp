#include "memory.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define WIDTH 680
#define HEIGHT 30
#define PENW 1
//////////////////////

Memory::Memory(machine::QtMipsMachine *machine) : QGraphicsObject(nullptr) {
    con_pc = new Connector(M_PI_2);
    con_inst = new Connector(M_PI_2);
    con_address = new Connector(M_PI_2);
    con_data_in = new Connector(M_PI_2);
    con_data_out = new Connector(M_PI_2);
    con_req_write = new Connector(M_PI_2);
    con_req_read = new Connector(M_PI_2);

    // TODO cache?

    name = new QGraphicsSimpleTextItem("Memory", this);
    QRectF name_box = name->boundingRect();
    name->setPos(WIDTH/2 - name_box.width()/2, HEIGHT/2 - name_box.height()/2);

    // TODO add labels for connections

    QFont font;
    font.setPointSize(7);

    // TODO better placement
    name_program = new QGraphicsSimpleTextItem("Program", this);
    name_box = name_program->boundingRect();
    name_program->setPos(1, HEIGHT - 1 - name_box.height());
    name_program->setFont(font);

    name_data = new QGraphicsSimpleTextItem("Data", this);
    name_box = name_data->boundingRect();
    name_data->setPos(WIDTH - 1 - name_box.width(), HEIGHT - 1 - name_box.height());
    name_data->setFont(font);

    setPos(x(), y()); // set connector's position
}

Memory::~Memory() {
    delete con_pc;
    delete con_inst;
    delete con_address;
    delete con_data_out;
    delete con_data_in;
    delete con_req_write;
    delete con_req_read;

    delete name;
    delete name_program;
    delete name_data;
}

QRectF Memory::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW);
}

void Memory::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    painter->drawRect(0, 0, WIDTH, HEIGHT);
    // TODO cache
}

void Memory::setPos(qreal x, qreal y) {
    QGraphicsObject::setPos(x, y);

    con_pc->setPos(x + 30, y);
    con_inst->setPos(x + 40, y);

    con_address->setPos(x + WIDTH - 70, y);
    con_data_in->setPos(x + WIDTH - 60, y);
    con_req_write->setPos(x + WIDTH- 50, y);
    con_req_read->setPos(x + WIDTH - 40, y);
    con_data_out->setPos(x + WIDTH - 30, y);
}

const Connector *Memory::connector_pc() const {
    return con_pc;
}

const Connector *Memory::connector_inst() const {
    return con_inst;
}

const Connector *Memory::connector_address() const {
    return con_address;
}

const Connector *Memory::connector_data_out() const {
    return con_data_out;
}

const Connector *Memory::connector_data_in() const {
    return con_data_in;
}

const Connector *Memory::connector_req_write() const {
    return con_req_write;
}

const Connector *Memory::connector_req_read() const {
    return con_req_read;
}

void Memory::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseDoubleClickEvent(event);

    if (event->pos().x() < WIDTH/2)
        emit open_program_mem();
    else
        emit open_data_mem();
}
