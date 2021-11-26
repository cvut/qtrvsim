#ifndef POLYFILLS_QFONTMETRICS_H
#define POLYFILLS_QFONTMETRICS_H

int QFontMetrics_horizontalAdvance(const QFontMetrics &self, const QString &str, int len = -1) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    return self.horizontalAdvance(str, len);
#else
    return self.width(str, len);
#endif
}

#endif // QTMIPS_QFONTMETRICS_H
