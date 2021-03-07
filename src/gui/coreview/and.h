#ifndef COREVIEW_AND_H
#define COREVIEW_AND_H

#include "connection.h"

#include <QGraphicsItem>
#include <QVector>

namespace coreview {

class And : public QGraphicsItem {
public:
    And(unsigned size = 2);
    ~And() override;

    QRectF boundingRect() const override;
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

    void setPos(qreal x, qreal y);
    const Connector *connector_in(unsigned) const;
    const Connector *connector_out() const;

private:
    QVector<Connector *> connectors;
    Connector *con_out;
};

} // namespace coreview

#endif // COREVIEW_AND_H
