#ifndef COREVIEW_LATCH_H
#define COREVIEW_LATCH_H

#include <QGraphicsObject>
#include <QPropertyAnimation>
#include <QVector>
#include "qtmipsexception.h"
#include "qtmipsmachine.h"
#include "connection.h"

namespace coreview {

class Latch : public QGraphicsObject {
    Q_OBJECT
    Q_PROPERTY(QColor wedge_clr READ wedge_color WRITE set_wedge_color)
public:
    Latch(machine::QtMipsMachine *machine, qreal height);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QColor wedge_color();
    void set_wedge_color(QColor &c);

    void setTitle(const QString &str);

    void setPos(qreal x, qreal y);

    struct ConnectorPair { Connector *in, *out; };

    struct ConnectorPair new_connector(qreal y); // Create new connectors pair that is given y from top of latch

protected:
    void updateCurrentValue(const QColor &color);

private slots:
    void tick();

private:
    qreal height;
    QVector<ConnectorPair> connectors;
    QVector<qreal> connectors_off;

    QGraphicsSimpleTextItem *title;

    QPropertyAnimation *wedge_animation;
    QColor wedge_clr;
};

}

#endif // COREVIEW_LATCH_H
