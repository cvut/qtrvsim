#include "svggraphicsscene.h"

#include "components/hyperlinkitem.h"

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

namespace svgscene {

void SvgGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    // Do not prevent default behavior.
    QGraphicsScene::mouseDoubleClickEvent(event);

    // Hyperlink will usually be obscured, but we need to propagate the click.
    QGraphicsItem *item = this->itemAt(event->pos(), {});
    while (item != nullptr) {
        if (auto hyperlink = dynamic_cast<HyperlinkItem *>(item)) {
            hyperlink->mouseDoubleClickEvent(event);
            break;
        }
        item = item->parentItem();
    }
}

} // namespace svgscene
