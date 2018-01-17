#ifndef LOGICBLOCK_H
#define LOGICBLOCK_H

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSimpleTextItem>
#include <QVector>
#include "connection.h"

namespace coreview {

class LogicBlock : public QGraphicsItem {
public:
    LogicBlock(QString name);
    LogicBlock(QVector<QString> name);
    ~LogicBlock();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(qreal x, qreal y);
    void setSize(qreal width, qreal height);

    // This creates new connector
    // Position is determined by x and y in 0 to 1 range and is mapped to real size of this block
    // Using x=y and x=-y coordinates is not supported
    const Connector *new_connector(qreal x, qreal y);

private:
    QVector<QGraphicsSimpleTextItem*> text;
    QRectF box;

    struct Con {
        Connector *con;
        qreal x, y;
        QPointF p;
    };
    QVector<struct Con> connectors;
    QPointF con_pos(qreal x, qreal y);
};

}

#endif // LOGICBLOCK_H
