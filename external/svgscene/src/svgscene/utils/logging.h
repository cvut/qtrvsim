#pragma once

/**
 * Wrapper for QT logging library.
 *
 * @see
 * https://www.kdab.com/wp-content/uploads/stories/slides/Day2/KaiKoehne_Qt%20Logging%20Framework%2016_9_0.pdf
 */

#include <QLoggingCategory>

#define LOG_CATEGORY(NAME) static QLoggingCategory _loging_category_(NAME)

#define LOG(...) qCInfo(_loging_category_, __VA_ARGS__)
#define DEBUG(...) qCDebug(_loging_category_, __VA_ARGS__)
#define WARN(...) qCWarning(_loging_category_, __VA_ARGS__)
#define ERROR(...) qCCritical(_loging_category_, __VA_ARGS__)