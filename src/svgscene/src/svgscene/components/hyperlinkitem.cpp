#include "hyperlinkitem.h"

#include "svgmetadata.h"
#include "utils/logging.h"

LOG_CATEGORY("svgscene.hyperlink");

namespace svgscene {

HyperlinkItem::HyperlinkItem() = default;

QString svgscene::HyperlinkItem::getTargetName() const {
    // href attribute is mandatory, therefore using default value
    return getXmlAttributeOr(this, "href", "");
}

void HyperlinkItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
    DEBUG("Link triggered (href: %s).", qPrintable(getTargetName()));
    emit triggered();
}

} // namespace svgscene
