#ifndef MULH64_TEST_H
#define MULH64_TEST_H

#include <QtTest/QTest>
#include <cstdint>

class TestMULH64 : public QObject {
    Q_OBJECT
private slots:
    static void test_mulh64();
    static void test_mulhu64();
    static void test_mulhsu64();
    static void test_mulh64_fallback();
    static void test_mulhu64_fallback();
    static void test_mulhsu64_fallback();
};

#endif // MULH64_TEST_H
