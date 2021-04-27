#pragma once

#include <QMap>
#include <QString>

namespace svgscene {

struct Types {
    struct DataKey {
        enum {
            XmlAttributes = 1,
            CssAttributes,
            Id,
            ChildId,
            ShvPath,
            ShvType,
            ShvVisuType,
        };
    };

    using XmlAttributes = QMap<QString, QString>;
    using CssAttributes = QMap<QString, QString>;

    static const QString ATTR_ID;
    static const QString ATTR_CHILD_ID;
    static const QString ATTR_SHV_PATH;
    static const QString ATTR_SHV_TYPE;
    static const QString ATTR_SHV_VISU_TYPE;
};

} // namespace svgscene
