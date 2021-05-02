#ifndef ALU_TEST_H
#define ALU_TEST_H

#include <QtTest>

class TestAlu : public QObject {
    Q_OBJECT
private slots:
    static void test_alu64_operate_data();
    static void test_alu64_operate();
    static void test_alu32_operate_data();
    static void test_alu32_operate();
    static void test_mul64_operate_data();
    static void test_mul64_operate();
    static void test_mul32_operate_data();
    static void test_mul32_operate();
};

#endif // ALU_TEST_H
