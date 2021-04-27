#ifndef SHV_VISU_SVGSCENE_GROUPITEM_H
#define SHV_VISU_SVGSCENE_GROUPITEM_H

#include <QGraphicsItem>

namespace svgscene {

class GroupItem : public QGraphicsRectItem {
    using Super = QGraphicsRectItem;

public:
    GroupItem(QGraphicsItem *parent = nullptr);

    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;
};

} // namespace svgscene

#endif // SHV_VISU_SVGSCENE_GROUPITEM_H
