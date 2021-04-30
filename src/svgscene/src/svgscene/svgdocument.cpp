#include "svgdocument.h"

namespace svgscene {

SvgDomTree<QGraphicsItem> SvgDocument::getRoot() const {
    return root;
}

SvgDocument::SvgDocument(QGraphicsItem *root) : root(root) {}

} // namespace svgscene
