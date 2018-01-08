#include "constant.h"
#include <cmath>

using namespace coreview;

//////////////////////
#define OFFSET 6
//////////////////////

Constant::Constant(const Connector *con, const QString &text) : QGraphicsObject(nullptr), text(text, this) {
    QFont font;
    font.setPointSize(7);
    this->text.setFont(font);

    con_our = new Connector(M_PI);
    conn = new Connection(con_our, con);
    connect(con, SIGNAL(updated(QPointF)), this, SLOT(ref_con_updated(QPointF)));
    ref_con_updated(con->point()); // update initial connector position
}

Constant::~Constant() {
    delete conn;
    delete con_our;
}

QRectF Constant::boundingRect() const {
    return conn->boundingRect();
}

void Constant::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    conn->paint(painter, option, widget);
}

void Constant::set_text(const QString &text) {
    this->text.setText(text);
    set_text_pos(); // update text positioning
}

void Constant::ref_con_updated(QPointF p) {
    con_our->setPos(p.x() - OFFSET, p.y());
    set_text_pos();
}

void Constant::set_text_pos() {
    // We are using here our known position of con_our
    QRectF box = text.boundingRect();
    text.setPos(con_our->x() - box.width() - 2, con_our->y() - box.height()/2);
}
