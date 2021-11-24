#include "svghandler.h"

#include "components/groupitem.h"
#include "components/hyperlinkitem.h"
#include "components/simpletextitem.h"
#include "polyfills/qt5/qstringview.h"
#include "svgmetadata.h"
#include "svgspec.h"
#include "utils/logging.h"

#include <QFontMetrics>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QSet>
#include <QStack>
#include <QXmlStreamReader>
#include <QtMath>

LOG_CATEGORY("svgscene.parsing");

namespace svgscene {

SvgDocument parseFromFileName(QGraphicsScene *scene, const QString &filename) {
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    return parseFromFile(scene, &file);
}

SvgDocument parseFromFile(QGraphicsScene *scene, QFile *file) {
    QXmlStreamReader xml(file);
    SvgHandler handler(scene);
    handler.load(&xml);
    return handler.getDocument();
}

// see: https://www.w3.org/TR/SVG11/

// '0' is 0x30 and '9' is 0x39
static inline bool isDigit(ushort ch) {
    static quint16 magic = 0x3ff;
    return ((ch >> 4) == 3) && (magic >> (ch & 15));
}

static qreal toDouble(const QChar *&str) {
    const int maxLen = 255; // technically doubles can go til 308+ but whatever
    char temp[maxLen + 1];
    int pos = 0;
    if (*str == QLatin1Char('-')) {
        temp[pos++] = '-';
        ++str;
    } else if (*str == QLatin1Char('+')) {
        ++str;
    }
    while (isDigit(str->unicode()) && pos < maxLen) {
        temp[pos++] = str->toLatin1();
        ++str;
    }
    if (*str == QLatin1Char('.') && pos < maxLen) {
        temp[pos++] = '.';
        ++str;
    }
    while (isDigit(str->unicode()) && pos < maxLen) {
        temp[pos++] = str->toLatin1();
        ++str;
    }
    bool exponent = false;
    if ((*str == QLatin1Char('e') || *str == QLatin1Char('E')) && pos < maxLen) {
        exponent = true;
        temp[pos++] = 'e';
        ++str;
        if ((*str == QLatin1Char('-') || *str == QLatin1Char('+')) && pos < maxLen) {
            temp[pos++] = str->toLatin1();
            ++str;
        }
        while (isDigit(str->unicode()) && pos < maxLen) {
            temp[pos++] = str->toLatin1();
            ++str;
        }
    }
    temp[pos] = '\0';
    qreal val;
    if (!exponent && pos < 10) {
        int ival = 0;
        const char *t = temp;
        bool neg = false;
        if (*t == '-') {
            neg = true;
            ++t;
        }
        while (*t && *t != '.') {
            ival *= 10;
            ival += (*t) - '0';
            ++t;
        }
        if (*t == '.') {
            ++t;
            int div = 1;
            while (*t) {
                ival *= 10;
                ival += (*t) - '0';
                div *= 10;
                ++t;
            }
            val = ((qreal)ival) / ((qreal)div);
        } else {
            val = ival;
        }
        if (neg) val = -val;
    } else {
        val = QByteArray::fromRawData(temp, pos).toDouble();
    }
    return val;
}

static qreal toDouble(const QString &str, bool *ok = nullptr) {
    const QChar *c = str.constData();
    qreal res = toDouble(c);
    if (ok) { *ok = ((*c) == QLatin1Char('\0')); }
    return res;
}

/*
static qreal toDouble(const QStringView &str, bool *ok = nullptr)
{
    const QChar *c = str.constData();
    qreal res = toDouble(c);
    if (ok) {
        *ok = (c == (str.constData() + str.length()));
    }
    return res;
}
*/
static inline void parseNumbersArray(const QChar *&str, QVarLengthArray<qreal, 8> &points) {
    while (str->isSpace())
        ++str;
    while (isDigit(str->unicode()) || *str == QLatin1Char('-') || *str == QLatin1Char('+')
           || *str == QLatin1Char('.')) {
        points.append(toDouble(str));
        while (str->isSpace())
            ++str;
        if (*str == QLatin1Char(',')) ++str;
        // eat the rest of space
        while (str->isSpace())
            ++str;
    }
}

static QVector<qreal> parseNumbersList(const QChar *&str) {
    QVector<qreal> points;
    if (!str) return points;
    points.reserve(32);
    while (str->isSpace())
        ++str;
    while (isDigit(str->unicode()) || *str == QLatin1Char('-') || *str == QLatin1Char('+')
           || *str == QLatin1Char('.')) {
        points.append(toDouble(str));
        while (str->isSpace())
            ++str;
        if (*str == QLatin1Char(',')) ++str;
        // eat the rest of space
        while (str->isSpace())
            ++str;
    }
    return points;
}

static QVector<qreal> parsePercentageList(const QChar *&str) {
    QVector<qreal> points;
    if (!str) return points;
    while (str->isSpace())
        ++str;
    while ((*str >= QLatin1Char('0') && *str <= QLatin1Char('9')) || *str == QLatin1Char('-')
           || *str == QLatin1Char('+') || *str == QLatin1Char('.')) {
        points.append(toDouble(str));
        while (str->isSpace())
            ++str;
        if (*str == QLatin1Char('%')) ++str;
        while (str->isSpace())
            ++str;
        if (*str == QLatin1Char(',')) ++str;
        // eat the rest of space
        while (str->isSpace())
            ++str;
    }
    return points;
}

static inline int qsvg_h2i(char hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
    if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
    return -1;
}
static inline int qsvg_hex2int(const char *s) {
    return (qsvg_h2i(s[0]) << 4) | qsvg_h2i(s[1]);
}
static inline int qsvg_hex2int(char s) {
    int h = qsvg_h2i(s);
    return (h << 4) | h;
}

bool qsvg_get_hex_rgb(const char *name, QRgb *rgb) {
    if (name[0] != '#') return false;
    name++;
    int len = static_cast<int>(qstrlen(name));
    int r, g, b;
    if (len == 12) {
        r = qsvg_hex2int(name);
        g = qsvg_hex2int(name + 4);
        b = qsvg_hex2int(name + 8);
    } else if (len == 9) {
        r = qsvg_hex2int(name);
        g = qsvg_hex2int(name + 3);
        b = qsvg_hex2int(name + 6);
    } else if (len == 6) {
        r = qsvg_hex2int(name);
        g = qsvg_hex2int(name + 2);
        b = qsvg_hex2int(name + 4);
    } else if (len == 3) {
        r = qsvg_hex2int(name[0]);
        g = qsvg_hex2int(name[1]);
        b = qsvg_hex2int(name[2]);
    } else {
        r = g = b = -1;
    }
    if ((uint)r > 255 || (uint)g > 255 || (uint)b > 255) {
        *rgb = 0;
        return false;
    }
    *rgb = qRgb(r, g, b);
    return true;
}

bool qsvg_get_hex_rgb(const QByteArray &str, int len, QRgb *rgb) {
    if (len > 13) return false;
    char tmp[16];
    for (int i = 0; i < len; ++i)
        tmp[i] = str[i];
    tmp[len] = 0;
    return qsvg_get_hex_rgb(tmp, rgb);
}

static QColor parseColor(const QString &color, const QString &opacity) {
    QColor ret;
    {
        QStringView color_str = QStringView(color).trimmed();
        if (color_str.isEmpty()) return ret;
        switch (color_str.at(0).unicode()) {
        case '#': {
            // #rrggbb is very very common, so let's tackle it here
            // rather than falling back to QColor
            QRgb rgb;
            bool ok = qsvg_get_hex_rgb(color_str.toUtf8(), color_str.length(), &rgb);
            if (ok) ret.setRgb(rgb);
            break;
        }
        case 'r': {
            // starts with "rgb(", ends with ")" and consists of at least 7
            // characters "rgb(,,)"
            if (color_str.length() >= 7 && color_str.at(color_str.length() - 1) == QLatin1Char(')')
                && QStringView(color_str.cbegin(), 4) == QLatin1String("rgb(")) {
                const QChar *s = color_str.cbegin() + 4;
                QVector<qreal> compo = parseNumbersList(s);
                // 1 means that it failed after reaching non-parsable
                // character which is going to be "%"
                if (compo.size() == 1) {
                    s = color_str.cbegin() + 4;
                    compo = parsePercentageList(s);
                    for (double &i : compo)
                        i *= (qreal)2.55;
                }
                if (compo.size() == 3) {
                    ret = QColor(int(compo[0]), int(compo[1]), int(compo[2]));
                }
            }
            break;
        }
        case 'c':
            if (color_str == QLatin1String("currentColor")) {
                // color = handler->currentColor();
                return ret;
            }
            break;
        case 'i':
            if (color_str == QLatin1String("inherit")) return ret;
            break;
        default: ret = QColor(QString(color_str.cbegin(), color_str.length())); break;
        }
    }
    if (!opacity.isEmpty() && ret.isValid()) {
        bool ok = true;
        qreal op = qMin(qreal(1.0), qMax(qreal(0.0), toDouble(opacity, &ok)));
        if (!ok) op = 1.0;
        ret.setAlphaF(op);
    }
    return ret;
}

static QTransform parseTransformationMatrix(const QStringView &value) {
    if (value.isEmpty()) return {};
    QTransform matrix;
    const QChar *str = value.cbegin();
    const QChar *end = str + value.length();
    while (str < end) {
        if (str->isSpace() || *str == QLatin1Char(',')) {
            ++str;
            continue;
        }
        enum State { Matrix, Translate, Rotate, Scale, SkewX, SkewY };
        State state = Matrix;
        if (*str == QLatin1Char('m')) { // matrix
            const char *ident = "atrix";
            for (int i = 0; i < 5; ++i)
                if (*(++str) != QLatin1Char(ident[i])) goto error;
            ++str;
            state = Matrix;
        } else if (*str == QLatin1Char('t')) { // translate
            const char *ident = "ranslate";
            for (int i = 0; i < 8; ++i)
                if (*(++str) != QLatin1Char(ident[i])) goto error;
            ++str;
            state = Translate;
        } else if (*str == QLatin1Char('r')) { // rotate
            const char *ident = "otate";
            for (int i = 0; i < 5; ++i)
                if (*(++str) != QLatin1Char(ident[i])) goto error;
            ++str;
            state = Rotate;
        } else if (*str == QLatin1Char('s')) { // scale, skewX, skewY
            ++str;
            if (*str == QLatin1Char('c')) {
                const char *ident = "ale";
                for (int i = 0; i < 3; ++i)
                    if (*(++str) != QLatin1Char(ident[i])) goto error;
                ++str;
                state = Scale;
            } else if (*str == QLatin1Char('k')) {
                if (*(++str) != QLatin1Char('e')) goto error;
                if (*(++str) != QLatin1Char('w')) goto error;
                ++str;
                if (*str == QLatin1Char('X'))
                    state = SkewX;
                else if (*str == QLatin1Char('Y'))
                    state = SkewY;
                else
                    goto error;
                ++str;
            } else {
                goto error;
            }
        } else {
            goto error;
        }
        while (str < end && str->isSpace())
            ++str;
        if (*str != QLatin1Char('(')) goto error;
        ++str;
        QVarLengthArray<qreal, 8> points;
        parseNumbersArray(str, points);
        if (*str != QLatin1Char(')')) goto error;
        ++str;
        if (state == Matrix) {
            if (points.count() != 6) goto error;
            matrix = QTransform(points[0], points[1], points[2], points[3], points[4], points[5])
                     * matrix;
        } else if (state == Translate) {
            if (points.count() == 1)
                matrix.translate(points[0], 0);
            else if (points.count() == 2)
                matrix.translate(points[0], points[1]);
            else
                goto error;
        } else if (state == Rotate) {
            if (points.count() == 1) {
                matrix.rotate(points[0]);
            } else if (points.count() == 3) {
                matrix.translate(points[1], points[2]);
                matrix.rotate(points[0]);
                matrix.translate(-points[1], -points[2]);
            } else {
                goto error;
            }
        } else if (state == Scale) {
            if (points.count() < 1 || points.count() > 2) goto error;
            qreal sx = points[0];
            qreal sy = sx;
            if (points.count() == 2) sy = points[1];
            matrix.scale(sx, sy);
        } else if (state == SkewX) {
            if (points.count() != 1) goto error;
            const auto deg2rad = qreal(0.017453292519943295769);
            matrix.shear(qTan(points[0] * deg2rad), 0);
        } else if (state == SkewY) {
            if (points.count() != 1) goto error;
            const auto deg2rad = qreal(0.017453292519943295769);
            matrix.shear(0, qTan(points[0] * deg2rad));
        }
    }
error:
    return matrix;
}

// the arc handling code underneath is from XSVG (BSD license)
/*
 * Copyright  2002 USC/Information Sciences Institute
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Information Sciences Institute not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.  Information Sciences Institute
 * makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * INFORMATION SCIENCES INSTITUTE DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL INFORMATION SCIENCES
 * INSTITUTE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
static void pathArcSegment(
    QPainterPath &path,
    qreal xc,
    qreal yc,
    qreal th0,
    qreal th1,
    qreal rx,
    qreal ry,
    qreal xAxisRotation) {
    qreal sinTh, cosTh;
    qreal a00, a01, a10, a11;
    qreal x1, y1, x2, y2, x3, y3;
    qreal t;
    qreal thHalf;
    sinTh = qSin(xAxisRotation * (M_PI / 180.0));
    cosTh = qCos(xAxisRotation * (M_PI / 180.0));
    a00 = cosTh * rx;
    a01 = -sinTh * ry;
    a10 = sinTh * rx;
    a11 = cosTh * ry;
    thHalf = 0.5 * (th1 - th0);
    t = (8.0 / 3.0) * qSin(thHalf * 0.5) * qSin(thHalf * 0.5) / qSin(thHalf);
    x1 = xc + qCos(th0) - t * qSin(th0);
    y1 = yc + qSin(th0) + t * qCos(th0);
    x3 = xc + qCos(th1);
    y3 = yc + qSin(th1);
    x2 = x3 + t * qSin(th1);
    y2 = y3 - t * qCos(th1);
    path.cubicTo(
        a00 * x1 + a01 * y1, a10 * x1 + a11 * y1, a00 * x2 + a01 * y2, a10 * x2 + a11 * y2,
        a00 * x3 + a01 * y3, a10 * x3 + a11 * y3);
}

static void pathArc(
    QPainterPath &path,
    qreal rx,
    qreal ry,
    qreal x_axis_rotation,
    int large_arc_flag,
    int sweep_flag,
    qreal x,
    qreal y,
    qreal curx,
    qreal cury) {
    qreal sin_th, cos_th;
    qreal a00, a01, a10, a11;
    qreal x0, y0, x1, y1, xc, yc;
    qreal d, sfactor, sfactor_sq;
    qreal th0, th1, th_arc;
    int i, n_segs;
    qreal dx, dy, dx1, dy1, Pr1, Pr2, Px, Py, check;
    rx = qAbs(rx);
    ry = qAbs(ry);
    sin_th = qSin(x_axis_rotation * (M_PI / 180.0));
    cos_th = qCos(x_axis_rotation * (M_PI / 180.0));
    dx = (curx - x) / 2.0;
    dy = (cury - y) / 2.0;
    dx1 = cos_th * dx + sin_th * dy;
    dy1 = -sin_th * dx + cos_th * dy;
    Pr1 = rx * rx;
    Pr2 = ry * ry;
    Px = dx1 * dx1;
    Py = dy1 * dy1;
    /* Spec : check if radii are large enough */
    check = Px / Pr1 + Py / Pr2;
    if (check > 1) {
        rx = rx * qSqrt(check);
        ry = ry * qSqrt(check);
    }
    a00 = cos_th / rx;
    a01 = sin_th / rx;
    a10 = -sin_th / ry;
    a11 = cos_th / ry;
    x0 = a00 * curx + a01 * cury;
    y0 = a10 * curx + a11 * cury;
    x1 = a00 * x + a01 * y;
    y1 = a10 * x + a11 * y;
    /* (x0, y0) is current point in transformed coordinate space.
       (x1, y1) is new point in transformed coordinate space.
       The arc fits a unit-radius circle in this space.
    */
    d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
    sfactor_sq = 1.0 / d - 0.25;
    if (sfactor_sq < 0) sfactor_sq = 0;
    sfactor = qSqrt(sfactor_sq);
    if (sweep_flag == large_arc_flag) sfactor = -sfactor;
    xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
    yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);
    /* (xc, yc) is center of the circle. */
    th0 = qAtan2(y0 - yc, x0 - xc);
    th1 = qAtan2(y1 - yc, x1 - xc);
    th_arc = th1 - th0;
    if (th_arc < 0 && sweep_flag)
        th_arc += 2 * M_PI;
    else if (th_arc > 0 && !sweep_flag)
        th_arc -= 2 * M_PI;
    n_segs = qCeil(qAbs(th_arc / (M_PI * 0.5 + 0.001)));
    for (i = 0; i < n_segs; i++) {
        pathArcSegment(
            path, xc, yc, th0 + i * th_arc / n_segs, th0 + (i + 1) * th_arc / n_segs, rx, ry,
            x_axis_rotation);
    }
}

static bool parsePathDataFast(const QStringView &dataStr, QPainterPath &path) {
    qreal x0 = 0, y0 = 0; // starting point
    qreal x = 0, y = 0;   // current point
    char lastMode = 0;
    QPointF ctrlPt;
    const QChar *str = dataStr.cbegin();
    const QChar *end = str + dataStr.size();
    while (str != end) {
        while (str->isSpace())
            ++str;
        QChar pathElem = *str;
        ++str;
        QChar endc = *end;
        *const_cast<QChar *>(end) = QChar('\0'); // parseNumbersArray requires
                                                 // 0-termination that QStringView cannot
                                                 // guarantee
        QVarLengthArray<qreal, 8> arg;
        parseNumbersArray(str, arg);
        *const_cast<QChar *>(end) = endc;
        if (pathElem == QLatin1Char('z') || pathElem == QLatin1Char('Z')) arg.append(0); // dummy
        const qreal *num = arg.constData();
        int count = arg.count();
        while (count > 0) {
            qreal offsetX = x; // correction offsets
            qreal offsetY = y; // for relative commands
            switch (pathElem.unicode()) {
            case 'm': {
                if (count < 2) {
                    num++;
                    count--;
                    break;
                }
                x = x0 = num[0] + offsetX;
                y = y0 = num[1] + offsetY;
                num += 2;
                count -= 2;
                path.moveTo(x0, y0);
                // As per 1.2  spec 8.3.2 The "moveto" commands
                // If a 'moveto' is followed by multiple pairs of coordinates
                // without explicit commands, the subsequent pairs shall be
                // treated as implicit 'lineto' commands.
                pathElem = QLatin1Char('l');
            } break;
            case 'M': {
                if (count < 2) {
                    num++;
                    count--;
                    break;
                }
                x = x0 = num[0];
                y = y0 = num[1];
                num += 2;
                count -= 2;
                path.moveTo(x0, y0);
                // As per 1.2  spec 8.3.2 The "moveto" commands
                // If a 'moveto' is followed by multiple pairs of coordinates
                // without explicit commands, the subsequent pairs shall be
                // treated as implicit 'lineto' commands.
                pathElem = QLatin1Char('L');
            } break;
            case 'z':
            case 'Z': {
                x = x0;
                y = y0;
                count--; // skip dummy
                num++;
                path.closeSubpath();
            } break;
            case 'l': {
                if (count < 2) {
                    num++;
                    count--;
                    break;
                }
                x = num[0] + offsetX;
                y = num[1] + offsetY;
                num += 2;
                count -= 2;
                path.lineTo(x, y);
            } break;
            case 'L': {
                if (count < 2) {
                    num++;
                    count--;
                    break;
                }
                x = num[0];
                y = num[1];
                num += 2;
                count -= 2;
                path.lineTo(x, y);
            } break;
            case 'h': {
                x = num[0] + offsetX;
                num++;
                count--;
                path.lineTo(x, y);
            } break;
            case 'H': {
                x = num[0];
                num++;
                count--;
                path.lineTo(x, y);
            } break;
            case 'v': {
                y = num[0] + offsetY;
                num++;
                count--;
                path.lineTo(x, y);
            } break;
            case 'V': {
                y = num[0];
                num++;
                count--;
                path.lineTo(x, y);
            } break;
            case 'c': {
                if (count < 6) {
                    num += count;
                    count = 0;
                    break;
                }
                QPointF c1(num[0] + offsetX, num[1] + offsetY);
                QPointF c2(num[2] + offsetX, num[3] + offsetY);
                QPointF e(num[4] + offsetX, num[5] + offsetY);
                num += 6;
                count -= 6;
                path.cubicTo(c1, c2, e);
                ctrlPt = c2;
                x = e.x();
                y = e.y();
                break;
            }
            case 'C': {
                if (count < 6) {
                    num += count;
                    count = 0;
                    break;
                }
                QPointF c1(num[0], num[1]);
                QPointF c2(num[2], num[3]);
                QPointF e(num[4], num[5]);
                num += 6;
                count -= 6;
                path.cubicTo(c1, c2, e);
                ctrlPt = c2;
                x = e.x();
                y = e.y();
                break;
            }
            case 's': {
                if (count < 4) {
                    num += count;
                    count = 0;
                    break;
                }
                QPointF c1;
                if (lastMode == 'c' || lastMode == 'C' || lastMode == 's' || lastMode == 'S')
                    c1 = QPointF(2 * x - ctrlPt.x(), 2 * y - ctrlPt.y());
                else
                    c1 = QPointF(x, y);
                QPointF c2(num[0] + offsetX, num[1] + offsetY);
                QPointF e(num[2] + offsetX, num[3] + offsetY);
                num += 4;
                count -= 4;
                path.cubicTo(c1, c2, e);
                ctrlPt = c2;
                x = e.x();
                y = e.y();
                break;
            }
            case 'S': {
                if (count < 4) {
                    num += count;
                    count = 0;
                    break;
                }
                QPointF c1;
                if (lastMode == 'c' || lastMode == 'C' || lastMode == 's' || lastMode == 'S')
                    c1 = QPointF(2 * x - ctrlPt.x(), 2 * y - ctrlPt.y());
                else
                    c1 = QPointF(x, y);
                QPointF c2(num[0], num[1]);
                QPointF e(num[2], num[3]);
                num += 4;
                count -= 4;
                path.cubicTo(c1, c2, e);
                ctrlPt = c2;
                x = e.x();
                y = e.y();
                break;
            }
            case 'q': {
                if (count < 4) {
                    num += count;
                    count = 0;
                    break;
                }
                QPointF c(num[0] + offsetX, num[1] + offsetY);
                QPointF e(num[2] + offsetX, num[3] + offsetY);
                num += 4;
                count -= 4;
                path.quadTo(c, e);
                ctrlPt = c;
                x = e.x();
                y = e.y();
                break;
            }
            case 'Q': {
                if (count < 4) {
                    num += count;
                    count = 0;
                    break;
                }
                QPointF c(num[0], num[1]);
                QPointF e(num[2], num[3]);
                num += 4;
                count -= 4;
                path.quadTo(c, e);
                ctrlPt = c;
                x = e.x();
                y = e.y();
                break;
            }
            case 't': {
                if (count < 2) {
                    num += count;
                    count = 0;
                    break;
                }
                QPointF e(num[0] + offsetX, num[1] + offsetY);
                num += 2;
                count -= 2;
                QPointF c;
                if (lastMode == 'q' || lastMode == 'Q' || lastMode == 't' || lastMode == 'T')
                    c = QPointF(2 * x - ctrlPt.x(), 2 * y - ctrlPt.y());
                else
                    c = QPointF(x, y);
                path.quadTo(c, e);
                ctrlPt = c;
                x = e.x();
                y = e.y();
                break;
            }
            case 'T': {
                if (count < 2) {
                    num += count;
                    count = 0;
                    break;
                }
                QPointF e(num[0], num[1]);
                num += 2;
                count -= 2;
                QPointF c;
                if (lastMode == 'q' || lastMode == 'Q' || lastMode == 't' || lastMode == 'T')
                    c = QPointF(2 * x - ctrlPt.x(), 2 * y - ctrlPt.y());
                else
                    c = QPointF(x, y);
                path.quadTo(c, e);
                ctrlPt = c;
                x = e.x();
                y = e.y();
                break;
            }
            case 'a': {
                if (count < 7) {
                    num += count;
                    count = 0;
                    break;
                }
                qreal rx = (*num++);
                qreal ry = (*num++);
                qreal xAxisRotation = (*num++);
                qreal largeArcFlag = (*num++);
                qreal sweepFlag = (*num++);
                qreal ex = (*num++) + offsetX;
                qreal ey = (*num++) + offsetY;
                count -= 7;
                qreal curx = x;
                qreal cury = y;
                pathArc(
                    path, rx, ry, xAxisRotation, int(largeArcFlag), int(sweepFlag), ex, ey, curx,
                    cury);
                x = ex;
                y = ey;
            } break;
            case 'A': {
                if (count < 7) {
                    num += count;
                    count = 0;
                    break;
                }
                qreal rx = (*num++);
                qreal ry = (*num++);
                qreal xAxisRotation = (*num++);
                qreal largeArcFlag = (*num++);
                qreal sweepFlag = (*num++);
                qreal ex = (*num++);
                qreal ey = (*num++);
                count -= 7;
                qreal curx = x;
                qreal cury = y;
                pathArc(
                    path, rx, ry, xAxisRotation, int(largeArcFlag), int(sweepFlag), ex, ey, curx,
                    cury);
                x = ex;
                y = ey;
            } break;
            default: return false;
            }
            lastMode = pathElem.toLatin1();
        }
    }
    return true;
}

SvgHandler::SvgHandler(QGraphicsScene *scene) : m_scene(scene) {}

SvgHandler::~SvgHandler() = default;

void SvgHandler::load(QXmlStreamReader *data, bool skip_definitions) {
    m_skipDefinitions = skip_definitions;
    m_xml = data;
    m_defaultPen = QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::SvgMiterJoin);
    m_defaultPen.setMiterLimit(4);
    parse();
    /*
    QGraphicsRectItem *it = new QGraphicsRectItem();
    it->setRect(m_scene->sceneRect());
    it->setPen(QPen(Qt::blue));
    m_scene->addItem(it);
    */
}

void SvgHandler::parse() {
    m_xml->setNamespaceProcessing(false);
    bool done = false;

    m_elementStack.push(SvgElement::initial_element());

    while (!m_xml->atEnd() && !done) {
        switch (m_xml->readNext()) {
        case QXmlStreamReader::StartElement: {
            SvgElement el(m_xml->name().toString());
            if (el.name == QLatin1String("defs")) {
                if (m_skipDefinitions) { m_xml->skipCurrentElement(); }
            }
            el.styleAttributes = m_elementStack.last().styleAttributes;
            el.xmlAttributes = parseXmlAttributes(m_xml->attributes(), el.styleAttributes);
            DEBUG() << QString(m_elementStack.count(), '-') << ">"
                    << "+ start element:" << el.name << "id:" << el.xmlAttributes.value("id");
            mergeCSSAttributes(el.styleAttributes, QStringLiteral("style"), el.xmlAttributes);
            m_elementStack.push(el);
            bool is_item_created = startElement();
            m_elementStack.last().itemCreated = is_item_created;
            break;
        }
        case QXmlStreamReader::EndElement: {
            SvgElement svg_element = m_elementStack.pop();
            DEBUG() << QString(m_elementStack.count(), '-') << ">"
                    << "- end element:" << m_xml->name()
                    << "item created:" << svg_element.itemCreated;
            if (svg_element.itemCreated && m_topLevelItem) {
                // logSvgI() << "m_topLevelItem:" << m_topLevelItem << typeid
                // (*m_topLevelItem).name() << svg_element.name;
                installVisuController(m_topLevelItem, svg_element);
                m_topLevelItem = m_topLevelItem->parentItem();
            }
            break;
        }
        case QXmlStreamReader::Characters: {
            DEBUG() << "characters element:" << m_xml->text(); // << typeid
                                                               // (*m_topLevelItem).name();
            if (auto *text_item = dynamic_cast<SimpleTextItem *>(m_topLevelItem)) {
                QString text = text_item->text();
                if (!text.isEmpty()) text += '\n';
                DEBUG() << text_item->text() << "+" << m_xml->text().toString();
                text_item->setText(text + m_xml->text().toString());
            } else if (auto *text_item = dynamic_cast<QGraphicsTextItem *>(m_topLevelItem)) {
                QString text = text_item->toPlainText();
                if (!text.isEmpty()) text += '\n';
                text += m_xml->text();
                text_item->setPlainText(text);
                // nInfo() << text_item->toPlainText();
            } else {
                DEBUG() << "characters are not part of text item, will be ignored";
                // nWarning() << "top:" << m_topLevelItem << (m_topLevelItem?
                // typeid (*m_topLevelItem).name(): "NULL");
            }
            break;
        }
        case QXmlStreamReader::ProcessingInstruction:
            DEBUG() << "ProcessingInstruction:" << m_xml->processingInstructionTarget()
                    << m_xml->processingInstructionData();
            // processingInstruction(xml->processingInstructionTarget().toString(),
            // xml->processingInstructionData().toString());
            break;
        default: break;
        }
    }
}

bool SvgHandler::startElement() {
    const SvgElement &el = m_elementStack.last();
    if (!m_topLevelItem) {
        if (el.name == QLatin1String("svg")) {
            m_topLevelItem = new QGraphicsRectItem();
            m_scene->addItem(m_topLevelItem);
            root = m_topLevelItem;
            return true;
        } else {
            WARN() << "unsupported root element:" << el.name;
        }
        return false;
    } else {
        if (el.name == QLatin1String("g")) {
            QGraphicsItem *item = createGroupItem(el);
            if (item) {
                setXmlAttributes(item, el);
                if (auto *rect_item = dynamic_cast<QGraphicsRectItem *>(item)) {
                    setStyle(rect_item, el.xmlAttributes);
                }
                setTransform(item, el.xmlAttributes.value(QStringLiteral("transform")));
                addItem(item);
                return true;
            }
            return false;
        } else if (el.name == QLatin1String("a")) {
            QGraphicsItem *item = createHyperlinkItem(el);
            if (item) {
                setXmlAttributes(item, el);
                if (auto *rect_item = dynamic_cast<QGraphicsRectItem *>(item)) {
                    setStyle(rect_item, el.xmlAttributes);
                }
                setTransform(item, el.xmlAttributes.value(QStringLiteral("transform")));
                addItem(item);
                return true;
            }
            return false;
        } else if (el.name == QLatin1String("rect")) {
            qreal x = el.xmlAttributes.value(QStringLiteral("x")).toDouble();
            qreal y = el.xmlAttributes.value(QStringLiteral("y")).toDouble();
            qreal w = el.xmlAttributes.value(QStringLiteral("width")).toDouble();
            qreal h = el.xmlAttributes.value(QStringLiteral("height")).toDouble();
            if (auto *text_item = dynamic_cast<QGraphicsTextItem *>(m_topLevelItem)) {
                QTransform t;
                t.translate(x, y);
                text_item->setTransform(t, true);
                text_item->setTextWidth(w);
                return false;
            } else {
                auto *item = new QGraphicsRectItem();
                setXmlAttributes(item, el);
                item->setRect(QRectF(x, y, w, h));
                setStyle(item, el.styleAttributes);
                setTransform(item, el.xmlAttributes.value(QStringLiteral("transform")));
                addItem(item);
                return true;
            }
        } else if (el.name == QLatin1String("circle")) {
            auto *item = new QGraphicsEllipseItem();
            setXmlAttributes(item, el);
            qreal cx = toDouble(el.xmlAttributes.value(QStringLiteral("cx")));
            qreal cy = toDouble(el.xmlAttributes.value(QStringLiteral("cy")));
            qreal rx = toDouble(el.xmlAttributes.value(QStringLiteral("r")));
            QRectF r(0, 0, 2 * rx, 2 * rx);
            r.translate(cx - rx, cy - rx);
            item->setRect(r);
            setStyle(item, el.styleAttributes);
            setTransform(item, el.xmlAttributes.value(QStringLiteral("transform")));
            addItem(item);
            return true;
        } else if (el.name == QLatin1String("ellipse")) {
            auto *item = new QGraphicsEllipseItem();
            setXmlAttributes(item, el);
            qreal cx = toDouble(el.xmlAttributes.value(QStringLiteral("cx")));
            qreal cy = toDouble(el.xmlAttributes.value(QStringLiteral("cy")));
            qreal rx = toDouble(el.xmlAttributes.value(QStringLiteral("rx")));
            qreal ry = toDouble(el.xmlAttributes.value(QStringLiteral("ry")));
            QRectF r(0, 0, 2 * rx, 2 * ry);
            r.translate(cx - rx, cy - ry);
            item->setRect(r);
            setStyle(item, el.styleAttributes);
            setTransform(item, el.xmlAttributes.value(QStringLiteral("transform")));
            addItem(item);
            return true;
        } else if (el.name == QLatin1String("path")) {
            auto *item = new QGraphicsPathItem();
            setXmlAttributes(item, el);
            QString data = el.xmlAttributes.value(QStringLiteral("d"));
            QPainterPath p;
            parsePathDataFast(QStringView(data), p);
            setStyle(item, el.styleAttributes);
            static auto FILL_RULE = QStringLiteral("fill-rule");
            if (el.styleAttributes.value(FILL_RULE) == QLatin1String("evenodd"))
                p.setFillRule(Qt::OddEvenFill);
            else
                p.setFillRule(Qt::WindingFill);
            item->setPath(p);
            setTransform(item, el.xmlAttributes.value(QStringLiteral("transform")));
            addItem(item);
            return true;
        } else if (el.name == QLatin1String("text")) {
            auto *item = new SimpleTextItem(el.styleAttributes);
            setXmlAttributes(item, el);
            qreal x = toDouble(el.xmlAttributes.value(QStringLiteral("x")));
            qreal y = toDouble(el.xmlAttributes.value(QStringLiteral("y")));
            //            item->setPos(x, y);
            setStyle(item, el.styleAttributes);
            setTextStyle(item, el.styleAttributes);
            setTransform(item, el.xmlAttributes.value(QStringLiteral("transform")));
            QFontMetricsF fm(item->font());
            QTransform t;
            t.translate(x, y - fm.ascent());
            item->setTransform(t, true);
            addItem(item);
            return true;
        } else if (el.name == QLatin1String("tspan")) {
            auto *item = new SimpleTextItem(el.styleAttributes);
            setXmlAttributes(item, el);
            qreal x = toDouble(el.xmlAttributes.value(QStringLiteral("x")));
            qreal y = toDouble(el.xmlAttributes.value(QStringLiteral("y")));
            // text->setPos(x, y);
            setStyle(item, el.styleAttributes);
            setTextStyle(item, el.styleAttributes);
            setTransform(item, el.xmlAttributes.value(QStringLiteral("transform")));
            QFontMetricsF fm(item->font());
            QTransform t;
            t.translate(x, y - fm.ascent());
            item->setTransform(t, true);
            addItem(item);
            return true;
        } else if (el.name == QLatin1String("flowRoot")) {
            auto *item = new QGraphicsTextItem();
            setXmlAttributes(item, el);
            // nWarning() << "FlowRoot:" << (QGraphicsItem*)item;
            setTextStyle(item, el.styleAttributes);
            setTransform(item, el.xmlAttributes.value(QStringLiteral("transform")));
            addItem(item);
            return true;
        } else {
            LOG() << "unsupported element:" << el.name;
        }
        return false;
    }
}

QGraphicsItem *SvgHandler::createGroupItem(const SvgHandler::SvgElement &el) {
    Q_UNUSED(el)
    QGraphicsItem *item = new GroupItem();
    return item;
}

QGraphicsItem *SvgHandler::createHyperlinkItem(const SvgHandler::SvgElement &el) {
    Q_UNUSED(el)
    QGraphicsItem *item = new HyperlinkItem();
    return item;
}

void SvgHandler::installVisuController(QGraphicsItem *it, const SvgHandler::SvgElement &el) {
    Q_UNUSED(it)
    Q_UNUSED(el)
}

void SvgHandler::setXmlAttributes(QGraphicsItem *git, const SvgHandler::SvgElement &el) {
    git->setData(
        static_cast<int>(MetadataType::XmlAttributes), QVariant::fromValue(el.xmlAttributes));
    git->setData(
        static_cast<int>(MetadataType::CssAttributes), QVariant::fromValue(el.styleAttributes));
}

void SvgHandler::setTransform(QGraphicsItem *it, const QString &str_val) {
    QStringView transform(str_val);
    QTransform mx = parseTransformationMatrix(transform.trimmed());
    if (!mx.isIdentity()) {
        QTransform t(mx);
        // logSvgI() << typeid (*it).name() << "setting matrix:" << t.dx() <<
        // t.dy();
        it->setTransform(t);
    }
}

CssAttributes
SvgHandler::parseXmlAttributes(const QXmlStreamAttributes &attributes, CssAttributes &css) {
    XmlAttributes xml;
    for (const QXmlStreamAttribute &attr : attributes) {
        const auto name = attr.name().toString();
        const auto value = attr.value().toString();
        xml[name] = value;

        /*
         * """
         * The presentation attributes thus will participate in the CSS2 cascade
         * as if they were replaced by corresponding CSS style rules placed at
         * the start of the author style sheet with a specificity of zero. In
         * general, this means that the presentation attributes have lower
         * priority than other CSS style rules specified in author style sheets
         * or ‘style’ attributes.
         * """
         * https://www.w3.org/TR/SVG/styling.html#UsingPresentationAttributes
         */
        if (svgspec::presentation_attributes.contains(name)) { css[name] = value; }
    }
    return xml;
}

void SvgHandler::mergeCSSAttributes(
    CssAttributes &css_attributes,
    const QString &attr_name,
    const XmlAttributes &xml_attributes) {
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QStringList css = xml_attributes.value(attr_name).split(';', QString::SkipEmptyParts);
#else
    QStringList css = xml_attributes.value(attr_name).split(';', Qt::SkipEmptyParts);
#endif
    for (const QString &ss : css) {
        int ix = ss.indexOf(':');
        if (ix > 0) { css_attributes[ss.mid(0, ix).trimmed()] = ss.mid(ix + 1).trimmed(); }
    }
}

void SvgHandler::setStyle(QAbstractGraphicsShapeItem *it, const CssAttributes &attributes) {
    QString fill = attributes.value(QStringLiteral("fill"));
    if (fill.isEmpty()) {
        // default fill
    } else if (fill == QLatin1String("none")) {
        it->setBrush(Qt::NoBrush);
    } else {
        QString opacity = attributes.value(QStringLiteral("fill-opacity"));
        it->setBrush(parseColor(fill, opacity));
    }
    QString stroke = attributes.value(QStringLiteral("stroke"));
    if (stroke.isEmpty() || stroke == QLatin1String("none")) {
        it->setPen(Qt::NoPen);
    } else {
        QString opacity = attributes.value(QStringLiteral("stroke-opacity"));
        QPen pen(parseColor(stroke, opacity));
        pen.setWidthF(toDouble(attributes.value(QStringLiteral("stroke-width"))));
        QString linecap = attributes.value(QStringLiteral("stroke-linecap"));
        if (linecap == QLatin1String("round"))
            pen.setCapStyle(Qt::RoundCap);
        else if (linecap == QLatin1String("square"))
            pen.setCapStyle(Qt::SquareCap);
        else // if(linecap == QLatin1String("butt"))
            pen.setCapStyle(Qt::FlatCap);
        QString join = attributes.value(QStringLiteral("stroke-linejoin"));
        if (join == QLatin1String("round"))
            pen.setJoinStyle(Qt::RoundJoin);
        else if (join == QLatin1String("bevel"))
            pen.setJoinStyle(Qt::BevelJoin);
        else // if( join == QLatin1String("miter"))
            pen.setJoinStyle(Qt::MiterJoin);
        QString dash_pattern = attributes.value(QStringLiteral("stroke-dasharray"));
        if (!(dash_pattern.isEmpty() || dash_pattern == QLatin1String("none"))) {
            QStringList array = dash_pattern.split(',');
            QVector<qreal> arr;
            for (const auto &s : array) {
                bool ok;
                double d = s.toDouble(&ok);
                if (!ok) {
                    LOG() << "Invalid stroke dash definition:" << dash_pattern;
                    arr.clear();
                    break;
                }
                arr << d;
            }
            if (!arr.isEmpty()) { pen.setDashPattern(arr); }
        }
        QString dash_offset = attributes.value(QStringLiteral("stroke-dashoffset"));
        if (!(dash_offset.isEmpty() || dash_offset == QLatin1String("none"))) {
            bool ok;
            double d = dash_offset.toDouble(&ok);
            if (ok) {
                pen.setDashOffset(d);
            } else {
                LOG() << "Invalid stroke dash offset:" << dash_offset;
            }
        }
        it->setPen(pen);
    }
}

void SvgHandler::setTextStyle(QFont &font, const CssAttributes &attributes) {
    DEBUG() << "orig font" << font.toString();
    // font.setStyleName(QString());
    font.setStyleName(QStringLiteral("Normal"));
    QString font_size = attributes.value(QStringLiteral("font-size"));
    if (!font_size.isEmpty()) {
        DEBUG() << "font_size:" << font_size;
        if (font_size.endsWith(QLatin1String("px")))
            font.setPixelSize((int)toDouble(font_size.mid(0, font_size.size() - 2)));
        else if (font_size.endsWith(QLatin1String("pt")))
            font.setPointSizeF(toDouble(font_size.mid(0, font_size.size() - 2)));
    }
    QString font_family = attributes.value(QStringLiteral("font-family"));
    if (!font_family.isEmpty()) {
        DEBUG() << "font_family:" << font_family;
        font.setFamily(font_family);
    }
    font.setWeight(QFont::Normal);
    QString font_weight = attributes.value(QStringLiteral("font-weight"));
    if (!font_weight.isEmpty()) {
        DEBUG() << "font_weight:" << font_weight;
        if (font_weight == QLatin1String("thin"))
            font.setWeight(QFont::Thin);
        else if (font_weight == QLatin1String("light"))
            font.setWeight(QFont::Light);
        else if (font_weight == QLatin1String("normal"))
            font.setWeight(QFont::Normal);
        else if (font_weight == QLatin1String("medium"))
            font.setWeight(QFont::Medium);
        else if (font_weight == QLatin1String("bold"))
            font.setWeight(QFont::Bold);
        else if (font_weight == QLatin1String("black"))
            font.setWeight(QFont::Black);
    }
    font.setStretch(QFont::Unstretched);
    QString font_stretch = attributes.value(QStringLiteral("font-stretch"));
    if (!font_stretch.isEmpty()) {
        DEBUG() << "font_stretch:" << font_stretch;
        if (font_stretch == QLatin1String("ultra-condensed"))
            font.setStretch(QFont::UltraCondensed);
        else if (font_stretch == QLatin1String("extra-condensed"))
            font.setStretch(QFont::ExtraCondensed);
        else if (font_stretch == QLatin1String("condensed"))
            font.setStretch(QFont::Condensed);
        else if (font_stretch == QLatin1String("semi-condensed"))
            font.setStretch(QFont::SemiCondensed);
        else if (font_stretch == QLatin1String("semi-expanded"))
            font.setStretch(QFont::SemiExpanded);
        else if (font_stretch == QLatin1String("expanded"))
            font.setStretch(QFont::Expanded);
        else if (font_stretch == QLatin1String("extra-expanded"))
            font.setStretch(QFont::ExtraExpanded);
        else if (font_stretch == QLatin1String("ultra-expanded"))
            font.setStretch(QFont::UltraExpanded);
        else // if(font_stretch == QLatin1String("normal"))
            font.setStretch(QFont::Unstretched);
    }
    font.setStyle(QFont::StyleNormal);
    QString font_style = attributes.value(QStringLiteral("font-style"));
    if (!font_style.isEmpty()) {
        if (font_style == QLatin1String("normal"))
            font.setStyle(QFont::StyleNormal);
        else if (font_style == QLatin1String("italic"))
            font.setStyle(QFont::StyleItalic);
        else if (font_style == QLatin1String("oblique"))
            font.setStyle(QFont::StyleOblique);
    }
    DEBUG() << "font"
            << "px size:" << font.pixelSize() << "pt size:" << font.pointSize()
            << "stretch:" << font.stretch() << "weight:" << font.weight();
    DEBUG() << "new font" << font.toString();
}

void SvgHandler::setTextStyle(QGraphicsSimpleTextItem *text, const CssAttributes &attributes) {
    QFont f = text->font();
    setTextStyle(f, attributes);
    text->setFont(f);
}

void SvgHandler::setTextStyle(QGraphicsTextItem *text, const CssAttributes &attributes) {
    QFont f = text->font();
    setTextStyle(f, attributes);
    text->setFont(f);
    static auto FILL = QStringLiteral("fill");
    QString fill = attributes.value(FILL);
    if (fill.isEmpty() || fill == QLatin1String("none")) {
        // it->setBrush(Qt::NoBrush);
    } else {
        static auto FILL_OPACITY = QStringLiteral("fill-opacity");
        QString opacity = attributes.value(FILL_OPACITY);
        text->setDefaultTextColor(parseColor(fill, opacity));
    }
}

QString SvgHandler::point2str(QPointF r) {
    auto ret = QStringLiteral("Point(%1, %2)");
    return ret.arg(r.x()).arg(r.y());
}

QString SvgHandler::rect2str(QRectF r) {
    auto ret = QStringLiteral("Rect(%1, %2 %3 x %4)");
    return ret.arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height());
}

void SvgHandler::addItem(QGraphicsItem *it) {
    if (!m_topLevelItem) return;
    DEBUG() << "adding item:" << typeid(*it).name() << "pos:" << point2str(it->pos())
            << "bounding rect:" << rect2str(it->boundingRect());
    if (auto *grp = dynamic_cast<QGraphicsItemGroup *>(m_topLevelItem)) {
        grp->addToGroup(it);
    } else {
        it->setParentItem(m_topLevelItem);
    }
    m_topLevelItem = it;
}

SvgDocument SvgHandler::getDocument() const {
    return SvgDocument(root);
}

SvgHandler::SvgElement SvgHandler::SvgElement::initial_element() {
    auto el = SvgHandler::SvgElement();
    el.styleAttributes = {
        // TODO Choose some reasonable set.
        { "stroke-width", "1" },
    };
    return el;
}
} // namespace svgscene