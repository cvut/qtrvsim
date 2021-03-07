#include "latch.h"

#include "coreview_colors.h"

using namespace coreview;

//////////////////////
#define WIDTH 7
#define PENW 1
//////////////////////

Latch::Latch(machine::Machine *machine, qreal height)
    : QGraphicsObject(nullptr) {
    this->height = height;

    title = new QGraphicsSimpleTextItem(this);
    QFont font;
    font.setPointSize(6);
    title->setFont(font);

    wedge_animation = new QPropertyAnimation(this, "wedge_clr");
    wedge_animation->setDuration(100);
    wedge_animation->setStartValue(QColor(0, 0, 0));
    wedge_animation->setEndValue(QColor(255, 255, 255));
    wedge_clr = QColor(255, 255, 255);

    connect(machine, &machine::Machine::tick, this, &Latch::tick);
}

Latch::~Latch() {
    while (!connectors.isEmpty()) {
        ConnectorPair cp = connectors.takeFirst();
        delete cp.in;
        delete cp.out;
    }
    delete wedge_animation;
}

QRectF Latch::boundingRect() const {
    QRectF b(-PENW / 2, -PENW / 2, WIDTH + PENW, height + PENW);
    b |= title->boundingRect();
    return b;
}

void Latch::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option __attribute__((unused)),
    QWidget *widget __attribute__((unused))) {
    QPen pen = painter->pen();
    pen.setColor(BLOCK_OUTLINE_COLOR);
    painter->setPen(pen);

    painter->drawRect(0, 0, WIDTH, height);
    // Now tick rectangle
    const QPointF tickPolygon[]
        = { QPointF(0, 0), QPointF(WIDTH, 0), QPointF(WIDTH / 2, WIDTH / 2) };
    painter->setBrush(QBrush(wedge_color()));
    painter->drawPolygon(tickPolygon, sizeof(tickPolygon) / sizeof(QPointF));
}

QColor Latch::wedge_color() {
    return wedge_clr;
}

void Latch::set_wedge_color(QColor &c) {
    wedge_clr = c;
    update();
}

void Latch::setTitle(const QString &str) {
    title->setText(str);
    QRectF box = title->boundingRect();
    title->setPos(WIDTH / 2 - box.width() / 2, -box.height() - 1);
}

void Latch::setPos(qreal x, qreal y) {
    QGraphicsObject::setPos(x, y);
    for (int i = 0; i < connectors.size(); i++) {
        connectors[i].in->setPos(x, y + connectors_off[i]);
        connectors[i].out->setPos(x + WIDTH, y + connectors_off[i]);
    }
}

struct Latch::ConnectorPair Latch::new_connector(qreal cy) {
    SANITY_ASSERT(
        cy < height,
        "Latch: Trying to create connector outside of latch height");
    ConnectorPair cp {};
    cp.in = new Connector(Connector::AX_X);
    cp.out = new Connector(Connector::AX_X);
    connectors.append(cp);
    connectors_off.append(cy);
    setPos(x(), y()); // Update connectors position
    return cp;
}

void Latch::tick() {
    wedge_clr = QColor(0, 0, 0);
    wedge_animation->start();
    update();
}
