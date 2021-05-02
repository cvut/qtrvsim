#ifndef LOGICBLOCK_H
#define LOGICBLOCK_H

#include "connection.h"

#include <QGraphicsObject>
#include <QGraphicsSimpleTextItem>
#include <QObject>
#include <QPainter>
#include <QVector>

namespace coreview {

class LogicBlock : public QGraphicsObject {
    Q_OBJECT
public:
    LogicBlock(QString name);
    LogicBlock(QVector<QString> name);
    ~LogicBlock() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setPos(qreal x, qreal y);
    void setSize(qreal width, qreal height);

    // This creates new connector
    // Position is determined by x and y in 0 to 1 range and is mapped to real
    // size of this block Using x=y and x=-y coordinates is not supported
    const Connector *new_connector(qreal x, qreal y);

signals:
    void open_block();

private:
    QVector<QGraphicsSimpleTextItem *> text;
    QRectF box;

    struct Con {
        Connector *con {};
        qreal x {}, y {};
        QPointF p;
    };
    QVector<struct Con> connectors;
    QPointF con_pos(qreal x, qreal y);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

} // namespace coreview

#endif // LOGICBLOCK_H
