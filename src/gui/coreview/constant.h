#ifndef CONSTANT_H
#define CONSTANT_H

#include "connection.h"

#include <QGraphicsObject>
#include <QGraphicsSimpleTextItem>
#include <QPainter>

namespace coreview {

class Constant : public QGraphicsObject {
    Q_OBJECT
public:
    Constant(const Connector *con, const QString &text);
    ~Constant() override;

    QRectF boundingRect() const override;
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

    void set_text(const QString &text);

private slots:
    void ref_con_updated(QPointF);

private:
    QGraphicsSimpleTextItem text;
    Connector *con_our;
    Connection *conn {};

    void set_text_pos();
};

} // namespace coreview

#endif // CONSTANT_H
