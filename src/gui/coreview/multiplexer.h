#ifndef COREVIEW_MULTIPLEXER_H
#define COREVIEW_MULTIPLEXER_H

#include "connection.h"
#include "machine/simulator_exception.h"

#include <QGraphicsObject>

namespace coreview {

class Multiplexer : public QGraphicsObject {
    Q_OBJECT
public:
    Multiplexer(unsigned size, bool ctl_up = false);
    ~Multiplexer() override;

    QRectF boundingRect() const override;
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

    void setPos(qreal x, qreal y);
    const Connector *connector_ctl() const;          // Control input
    const Connector *connector_out() const;          // Output
    const Connector *connector_in(unsigned i) const; // Inputs

public slots:
    void set(uint32_t i); // Set what value should be set as connected
                          // (indexing from  1 where 0 is no line)

private:
    bool ctlfrom;
    unsigned size;
    int seton;
    Connector *con_ctl, *con_out, **con_in;
};

} // namespace coreview

#endif // COREVIEW_MULTIPLEXER_H
