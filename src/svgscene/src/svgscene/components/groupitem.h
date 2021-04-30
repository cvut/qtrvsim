#pragma once

#include <QGraphicsItem>

namespace svgscene {

class GroupItem : public QGraphicsRectItem {
    using Super = QGraphicsRectItem;

public:
    explicit GroupItem(QGraphicsItem *parent = nullptr);

    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;
};

} // namespace svgscene