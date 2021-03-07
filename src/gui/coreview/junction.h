#ifndef JUNCTION_H
#define JUNCTION_H

#include "connection.h"

#include <QGraphicsItem>
#include <QList>
#include <QPainter>

namespace coreview {

class Junction : public QGraphicsItem {
public:
    Junction(bool point = true, int dot_size = -1);
    ~Junction() override;

    QRectF boundingRect() const override;
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

    void setPos(qreal x, qreal y);
    Connector *new_connector(enum Connector::Axis axis = Connector::AX_X);

private:
    QList<Connector *> cons;
    bool point;
    int dot_size;
};

} // namespace coreview

#endif // JUNCTION_H
