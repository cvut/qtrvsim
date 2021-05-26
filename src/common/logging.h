/**
 * Wrapper for QT logging library.
 *
 * Each source file is expected to declare a log category name what is
 * implicitly used for all logging macros. When logging in header files take
 * precaution not to pollute global scope. Either log manually or declare the
 * log within class scope.
 * Log categories can be structured using dots in name: `machine.core
 * .decode`.
 *
 * @see
 * https://www.kdab.com/wp-content/uploads/stories/slides/Day2/KaiKoehne_Qt%20Logging%20Framework%2016_9_0.pdf
 */
#ifndef LOGGING_H
#define LOGGING_H

#include <QLoggingCategory>

#define LOG_CATEGORY(NAME) static QLoggingCategory _loging_category_(NAME)

#if !defined(QT_NO_QDEBUG_MACRO)
    #define QT_NO_QDEBUG_MACRO                                                                     \
        while (false)                                                                              \
        QMessageLogger().noDebug
#endif

#if defined(QT_NO_DEBUG_OUTPUT)
    #define DEBUG QT_NO_QDEBUG_MACRO
#else
    #define DEBUG(...) qCDebug(_loging_category_, __VA_ARGS__)
#endif
#define LOG(...) qCInfo(_loging_category_, __VA_ARGS__)
#define WARN(...) qCWarning(_loging_category_, __VA_ARGS__)
#define ERROR(...) qCCritical(_loging_category_, __VA_ARGS__)

#endif
