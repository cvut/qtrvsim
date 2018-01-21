#ifndef COREVIEW_MULTIPLEXER_H
#define COREVIEW_MULTIPLEXER_H

#include <QGraphicsItem>
#include "qtmipsexception.h"
#include "connection.h"

namespace coreview {

class Multiplexer : public QGraphicsItem {
public:
    Multiplexer(unsigned size, bool ctl_up = false);
    ~Multiplexer();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    const Connector *connector_ctl() const; // Control input
    const Connector *connector_out() const; // Output
    const Connector *connector_in(unsigned i) const; // Inputs

    void set(unsigned i); // Set what value should be set as connected (indexing from  1 where 0 is no line)

private:
    bool ctlfrom;
    unsigned size, seton;
    Connector *con_ctl, *con_out, **con_in;
};

}

#endif // COREVIEW_MULTIPLEXER_H
