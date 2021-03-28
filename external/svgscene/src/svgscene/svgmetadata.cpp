#include "svgmetadata.h"

namespace svgscene {

XmlAttributes getXmlAttributes(const QGraphicsItem *element) {
    QVariant raw = element->data(static_cast<int>(MetadataType::XmlAttributes));
    if (!raw.isValid() || !raw.canConvert<XmlAttributes>()) {
        throw std::out_of_range("XmlAttributes not present in the object.\n"
                                "Check whether the object was created by the svgscene parser.");
    }
    return qvariant_cast<XmlAttributes>(raw);
}
QString getXmlAttribute(const QGraphicsItem *element, const QString &name) {
    XmlAttributes attrs = getXmlAttributes(element);
    if (!attrs.contains(name)) {
        throw std::out_of_range("Element does not contain requested XML attribute.");
    }
    return attrs.value(name);
}
QString getXmlAttributeOr(
    const QGraphicsItem *element,
    const QString &name,
    const QString &defaultValue) noexcept {
    XmlAttributes attrs = getXmlAttributes(element);
    return attrs.value(name, defaultValue);
}

CssAttributes getCssAttributes(const QGraphicsItem *element) {
    QVariant raw = element->data(static_cast<int>(MetadataType::CssAttributes));
    if (!raw.isValid() || !raw.canConvert<CssAttributes>()) {
        throw std::out_of_range("CssAttributes not present in the object.\n"
                                "Check whether the object was created by the svgscene parser.");
    }
    return qvariant_cast<CssAttributes>(raw);
}
QString getCssAttribute(const QGraphicsItem *element, const QString &name) {
    CssAttributes attrs = getCssAttributes(element);
    if (!attrs.contains(name)) {
        throw std::out_of_range("Element does not contain requested XML attribute.");
    }
    return attrs.value(name);
}
QString getCssAttributeOr(
    const QGraphicsItem *element,
    const QString &name,
    const QString &defaultValue) noexcept {
    CssAttributes attrs = getCssAttributes(element);
    return attrs.value(name, defaultValue);
}

} // namespace svgscene
