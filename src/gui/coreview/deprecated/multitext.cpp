#include "multitext.h"

#include "fontsize.h"

#include <QFont>
#include <QPainter>
#include <utility>

using namespace coreview;

//////////////////////
#define WIDTH 100
#define HEIGHT 14
#define ROUND 5
#define GAP 2
#define PENW 1
//////////////////////

MultiText::MultiText(const std::vector<QString> &texts_table, bool show_nonzero_red)
    : QGraphicsObject(nullptr)
    , text(this)
    , texts_table(texts_table)
    , show_nonzero_red(show_nonzero_red) {
    QFont f;
    f.setPointSize(FontSize::SIZE6);
    text.setFont(f);

    multitext_update(0);
}

QRectF MultiText::boundingRect() const {
    return { -WIDTH / 2 - PENW / 2, -PENW / 2, WIDTH + PENW, HEIGHT + PENW };
}

void MultiText::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setPen(QPen(QColor(240, 240, 240)));
    if (value != 0 && show_nonzero_red) {
        painter->setBrush(QBrush(QColor(255, 100, 100)));
    } else {
        painter->setBrush(QBrush(QColor(240, 240, 240)));
    }

    painter->drawRoundedRect(-WIDTH / 2, 0, WIDTH, HEIGHT, ROUND, ROUND);
}

void MultiText::multitext_update(size_t new_value) {
    QRectF prev_box = boundingRect();
    this->value = new_value;
    text.setText(texts_table.at(new_value));
    QRectF box = text.boundingRect();
    text.setPos(-box.width() / 2, GAP);
    update(prev_box.united(boundingRect()));
}
