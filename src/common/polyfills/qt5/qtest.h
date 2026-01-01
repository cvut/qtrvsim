#ifndef POLYFILLS_QTEST_H
#define POLYFILLS_QTEST_H

#include <QtTest>

// Qt5/Qt6 compatibility: QVERIFY_THROWS_EXCEPTION was added in Qt 6,
// Qt 5 used QVERIFY_EXCEPTION_THROWN instead (with swapped argument order)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #define QVERIFY_THROWS_EXCEPTION(exceptiontype, expression) \
        QVERIFY_EXCEPTION_THROWN(expression, exceptiontype)
#endif

#endif // POLYFILLS_QTEST_H