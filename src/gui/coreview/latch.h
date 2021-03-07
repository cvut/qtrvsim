#ifndef COREVIEW_LATCH_H
#define COREVIEW_LATCH_H

#include "connection.h"
#include "machine/machine.h"
#include "machine/simulator_exception.h"

#include <QGraphicsObject>
#include <QPropertyAnimation>
#include <QVector>

namespace coreview {

class Latch : public QGraphicsObject {
    Q_OBJECT
    Q_PROPERTY(QColor wedge_clr READ wedge_color WRITE set_wedge_color)
public:
    Latch(machine::Machine *machine, qreal height);
    ~Latch() override;

    QRectF boundingRect() const override;
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

    QColor wedge_color();
    void set_wedge_color(QColor &c);

    void setTitle(const QString &str);

    void setPos(qreal x, qreal y);

    struct ConnectorPair {
        Connector *in, *out;
    };

    struct ConnectorPair new_connector(qreal y); // Create new connectors pair
                                                 // that is given y from top of
                                                 // latch

protected:
    void updateCurrentValue(const QColor &color);

private slots:
    void tick();

private:
    qreal height;
    QVector<ConnectorPair> connectors;
    QVector<qreal> connectors_off;

    QGraphicsSimpleTextItem *title {};

    QPropertyAnimation *wedge_animation {};
    QColor wedge_clr;
};

} // namespace coreview

#endif // COREVIEW_LATCH_H
