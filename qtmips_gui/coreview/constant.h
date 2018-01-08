#ifndef CONSTANT_H
#define CONSTANT_H

#include <QGraphicsObject>
#include <QPainter>
#include <QGraphicsSimpleTextItem>
#include "connection.h"

namespace coreview {

class Constant : public QGraphicsObject {
    Q_OBJECT
public:
    Constant(const Connector *con, const QString &text);
    ~Constant();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void set_text(const QString &text);

private slots:
    void ref_con_updated(QPointF);

private:
    QGraphicsSimpleTextItem text;
    Connector *con_our;
    Connection *conn;

    void set_text_pos();
};

}

#endif // CONSTANT_H
