#include "logicblock.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define GAP 3
#define RADIUS GAP
#define LINE_OFFSET 1
#define PENW 1
//////////////////////

LogicBlock::LogicBlock(QString name) : LogicBlock(QVector<QString>({name})) { }

LogicBlock::LogicBlock(QVector<QString> name) : QGraphicsItem(nullptr) {
    QFont font;
    font.setPointSize(7);

    qreal h = 0, w = 0;
    for (int i = 0; i < name.size(); i++) {
        QGraphicsSimpleTextItem *t = new QGraphicsSimpleTextItem(name[i], this);
        t->setFont(font);
        text.append(t);
        QRectF t_box = t->boundingRect();
        t->setPos(-t_box.width()/2, h + LINE_OFFSET);
        h += t_box.height() + LINE_OFFSET;
        if (w < t_box.width())
            w = t_box.width();
    }

    box = QRectF(-w/2 - GAP, -GAP, w + (2*GAP), h + (2*GAP));
}

LogicBlock::~LogicBlock() {
    for (int i = 0; i < connectors.size(); i++)
        delete connectors[i].con;
    for (int i = 0; i < text.size(); i++)
        delete text[i];
}

QRectF LogicBlock::boundingRect() const {
    return QRectF(box.x() - PENW/2, box.y() - PENW/2, box.width() + PENW, box.height() + PENW);
}

void LogicBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused))) {
    painter->drawRoundedRect(box, RADIUS, RADIUS);
}

void LogicBlock::setPos(qreal x, qreal y) {
    QGraphicsItem::setPos(x, y);
    for (int i = 0; i < connectors.size(); i++) {
        struct Con &c = connectors[i];
        c.con->setPos(x + c.p.x(), y + c.p.y());
    }
}

void LogicBlock::setSize(qreal width, qreal height) {
    box.setX(-width/2 - GAP);
    box.setWidth(width + (2*GAP));
    box.setHeight(height + (2*GAP));
    for (int i = 0; i < connectors.size(); i++) { // Update point for all connectors
        struct Con &c = connectors[i];
        c.p = con_pos(c.x, c.y);
    }
    setPos(x(), y());
}

static qreal sign(qreal v) {
    // This intentionally doesn't return 0 on v == 0
    return (0 <= v) - (0 > v);
}

const Connector *LogicBlock::new_connector(qreal x, qreal y) {
    // stick to closest edge
    if (fabs(x) > fabs(y))
        x = sign(x);
    else
        y = sign(y);

    // Note: we are using here that 0 and M_PI is same angle but different orientation (but we ignore orientation for now)
    Connector *c = new Connector(fabs(x) > fabs(y) ? 0 : M_PI_2);
    connectors.append({
        .con = c,
        .x = x,
        .y = y,
        .p = con_pos(x, y)
    });
    setPos(this->x(), this->y()); // Update connector position
    return c;
}

QPointF LogicBlock::con_pos(qreal x, qreal y) {
    qreal px, py;
    px = (box.right() - GAP) * x + (GAP * sign(x));
    py = (box.bottom()/2 - GAP) * (y + 1) + (GAP * sign(y));
    return QPointF(px, py);
}
