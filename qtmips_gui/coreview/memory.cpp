#include "memory.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define WIDTH 60
#define HEIGHT 80
#define CACHE_HEIGHT 50
#define PENW 1
//////////////////////

Memory::Memory(machine::QtMipsMachine *machine) : QGraphicsObject(nullptr), name("Memory", this), type(this) {
    cache = false;

    QFont font;
    font.setPointSize(7);
    name.setFont(font);
    type.setFont(font);

    const QRectF &name_box = name.boundingRect();
    name.setPos(WIDTH/2 - name_box.width()/2, HEIGHT - (HEIGHT - CACHE_HEIGHT)/2);

    setPos(x(), y()); // set connector's position
}

QRectF Memory::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW);
}

void Memory::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    painter->drawRect(0, 0, WIDTH, HEIGHT);
    if (cache)
        painter->drawLine(0, CACHE_HEIGHT, WIDTH, CACHE_HEIGHT);
}

void Memory::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseDoubleClickEvent(event);

    if (cache && event->pos().y() < HEIGHT/2)
        emit open_cache();
    else
        emit open_mem();
}

void Memory::set_type(const QString &text) {
    type.setText(text);
    const QRectF &box = type.boundingRect();
    type.setPos(WIDTH/2 - box.width()/2, HEIGHT - (HEIGHT - CACHE_HEIGHT)/2 - box.height());
}

ProgramMemory::ProgramMemory(machine::QtMipsMachine *machine) : Memory(machine) {
    cache = machine->config().cache_program().enabled();
    set_type("Program");

    con_address = new Connector(0);
    con_inst = new Connector(M_PI);
}

ProgramMemory::~ProgramMemory() {
    delete con_address;
    delete con_inst;
}

void ProgramMemory::setPos(qreal x, qreal y) {
    QGraphicsObject::setPos(x, y);

    con_address->setPos(x, y + 20);
    con_inst->setPos(x + WIDTH, y + 20);
}

const Connector *ProgramMemory::connector_address() const {
    return con_address;
}

const Connector *ProgramMemory::connector_instruction() const {
    return con_inst;
}

DataMemory::DataMemory(machine::QtMipsMachine *machine) : Memory(machine) {
    cache = machine->config().cache_data().enabled();
    set_type("Data");

    con_address = new Connector(0);
    con_data_out = new Connector(M_PI);
    con_data_in	= new Connector(0);
    con_req_write = new Connector(M_PI_2);
    con_req_read = new Connector(M_PI_2);
}

DataMemory::~DataMemory() {
    delete con_address;
    delete con_data_out;
    delete con_data_in;
    delete con_req_write;
    delete con_req_read;
}

void DataMemory::setPos(qreal x, qreal y) {
    QGraphicsObject::setPos(x, y);

    con_address->setPos(x, y + 20);
    con_data_out->setPos(x + WIDTH, y + 20);
    if (cache)
        con_data_in->setPos(x, y + 40);
    else
        con_data_in->setPos(x, y + 60);
    con_req_write->setPos(x + 40, y);
    con_req_read->setPos(x + 50, y);
}

const Connector *DataMemory::connector_address() const {
    return con_address;
}

const Connector *DataMemory::connector_data_out() const {
    return con_data_out;
}

const Connector *DataMemory::connector_data_in() const {
    return con_data_in;
}

const Connector *DataMemory::connector_req_write() const {
    return con_req_write;
}

const Connector *DataMemory::connector_req_read() const {
    return con_req_read;
}
