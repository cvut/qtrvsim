#ifndef ADDER_H
#define ADDER_H

#include "connection.h"

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QPainter>

namespace coreview {

class Adder : public QGraphicsItem {
public:
    Adder();
    ~Adder() override;

    QRectF boundingRect() const override;
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

    void setPos(qreal x, qreal y);
    const Connector *connector_in_a() const;
    const Connector *connector_in_b() const;
    const Connector *connector_out() const;

private:
    QGraphicsSimpleTextItem plus;

    Connector *con_in_a, *con_in_b, *con_out;
};

} // namespace coreview

#endif // ADDER_H
