#pragma once

#include <QGraphicsItem>
#include <QMap>
#include <QString>

namespace svgscene {

/*
 * Fields that can be found on SVG related `QGraphicsItem`s using the `data`
 * method.
 * Data are stored using QVariant.
 */
enum class MetadataType {
    XmlAttributes = 1,
    CssAttributes,
};

constexpr MetadataType LAST_VALUE = MetadataType::CssAttributes;

using XmlAttributes = QMap<QString, QString>;

XmlAttributes getXmlAttributes(const QGraphicsItem *element);
QString getXmlAttribute(const QGraphicsItem *element, const QString &name);
QString getXmlAttributeOr(
    const QGraphicsItem *element,
    const QString &name,
    const QString &defaultValue) noexcept;

using CssAttributes = QMap<QString, QString>;

CssAttributes getCssAttributes(const QGraphicsItem *element);
QString getCssAttribute(const QGraphicsItem *element, const QString &name);
QString getCssAttributeOr(
    const QGraphicsItem *element,
    const QString &name,
    const QString &defaultValue) noexcept;

} // namespace svgscene
