#pragma once

#include <QMap>
#include <QString>

namespace svgscene {

struct Types {
    struct DataKey {
        enum {
            XmlAttributes = 1,
            CssAttributes,
        };
    };
};

using XmlAttributes = QMap<QString, QString>;
using CssAttributes = QMap<QString, QString>;

} // namespace svgscene
