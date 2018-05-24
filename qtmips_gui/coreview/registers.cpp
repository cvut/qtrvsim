#include "registers.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define WIDTH 60
#define HEIGHT 80
#define PENW 1
//////////////////////

Registers::Registers() : QGraphicsObject(nullptr), name("Registers", this) {
    con_read1 = new Connector(Connector::AX_X);
    con_read1_reg = new Connector(Connector::AX_X);
    con_read2 = new Connector(Connector::AX_X);
    con_read2_reg = new Connector(Connector::AX_X);
    con_write = new Connector(Connector::AX_X);
    con_write_reg = new Connector(Connector::AX_X);
    con_ctl_write = new Connector(Connector::AX_Y);

    // TODO do we want to have any hooks on real registers?

    QFont font;
    font.setPointSize(7);
    name.setFont(font);

    QRectF name_box = name.boundingRect();
    name.setPos(WIDTH/2 - name_box.width()/2, HEIGHT/2 - name_box.height()/2);

    setPos(x(), y()); // set connector's position
}

Registers::~Registers() {
    delete con_read1;
    delete con_read1_reg;
    delete con_read2;
    delete con_read2_reg;
    delete con_write;
    delete con_write_reg;
    delete con_ctl_write;
}

QRectF Registers::boundingRect() const {
    return QRectF(-PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW);
}

void Registers::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute((unused)), QWidget *widget __attribute((unused))) {
    painter->drawRect(0, 0, WIDTH, HEIGHT);
}

void Registers::setPos(qreal x, qreal y) {
    QGraphicsObject::setPos(x, y);

    con_read1_reg->setPos(x, y + 10);
    con_read2_reg->setPos(x, y + 30);
    con_read1->setPos(x + WIDTH, y + 10);
    con_read2->setPos(x + WIDTH, y + 30);

    con_write_reg->setPos(x, y + HEIGHT - 10);
    con_write->setPos(x, y + HEIGHT - 20);
    con_ctl_write->setPos(x + WIDTH - 10, y);
}

const Connector *Registers::connector_read1() const {
    return con_read1;
}

const Connector *Registers::connector_read1_reg() const {
    return con_read1_reg;
}

const Connector *Registers::connector_read2() const {
    return con_read2;
}

const Connector *Registers::connector_read2_reg() const {
    return con_read2_reg;
}

const Connector *Registers::connector_write() const {
    return con_write;
}

const Connector *Registers::connector_write_reg() const {
    return con_write_reg;
}

const Connector *Registers::connector_ctl_write() const {
    return con_ctl_write;
}

void Registers::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseDoubleClickEvent(event);
    emit open_registers();
}
