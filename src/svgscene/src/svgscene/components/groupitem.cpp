#include "groupitem.h"

#include <QPainter>

namespace svgscene {

GroupItem::GroupItem(QGraphicsItem *parent)
	: Super(parent)
{

}

void GroupItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Super::paint(painter, option, widget);
	//painter->setPen(Qt::red);
	//painter->drawRect(childrenBoundingRect());
}

} // namespace svgscene
