#include "mulh64.h"

#include "mulh64.test.h"

void TestMULH64::test_mulh64() {
    QCOMPARE(mulh64(15, 10), (uint64_t)0);
    QCOMPARE(mulh64(15, -10), (uint64_t)0xffffffffffffffffULL);
    QCOMPARE(mulh64(-10, 15), (uint64_t)0xffffffffffffffffULL);
    QCOMPARE(mulh64(-15, -10), (uint64_t)0);
}

void TestMULH64::test_mulhu64() {
    QCOMPARE(mulhu64(15, 10), (uint64_t)0);
    QCOMPARE(mulhu64(15, -10), (uint64_t)14);
    QCOMPARE(mulhu64(-10, 15), (uint64_t)14);
    QCOMPARE(mulhu64(-10, -15), (uint64_t)0xffffffffffffffe7ULL);
}

void TestMULH64::test_mulhsu64() {
    QCOMPARE(mulhsu64(15, 10), (uint64_t)0);
    QCOMPARE(mulhsu64(15, -10), (uint64_t)14);
    QCOMPARE(mulhsu64(-10, 15), (uint64_t)0xffffffffffffffffULL);
    QCOMPARE(mulhsu64(-15, -10), (uint64_t)0xfffffffffffffff1ULL);
}

void TestMULH64::test_mulh64_fallback() {
    QCOMPARE(mulh64_fallback(15, 10), (uint64_t)0);
    QCOMPARE(mulh64_fallback(15, -10), (uint64_t)0xffffffffffffffffULL);
    QCOMPARE(mulh64_fallback(-10, 15), (uint64_t)0xffffffffffffffffULL);
    QCOMPARE(mulh64_fallback(-15, -10), (uint64_t)0);
}

void TestMULH64::test_mulhu64_fallback() {
    QCOMPARE(mulhu64_fallback(15, 10), (uint64_t)0);
    QCOMPARE(mulhu64_fallback(15, -10), (uint64_t)14);
    QCOMPARE(mulhu64_fallback(-10, 15), (uint64_t)14);
    QCOMPARE(mulhu64_fallback(-10, -15), (uint64_t)0xffffffffffffffe7ULL);
}

void TestMULH64::test_mulhsu64_fallback() {
    QCOMPARE(mulhsu64_fallback(15, 10), (uint64_t)0);
    QCOMPARE(mulhsu64_fallback(15, -10), (uint64_t)14);
    QCOMPARE(mulhsu64_fallback(-10, 15), (uint64_t)0xffffffffffffffffULL);
    QCOMPARE(mulhsu64_fallback(-15, -10), (uint64_t)0xfffffffffffffff1ULL);
}

QTEST_APPLESS_MAIN(TestMULH64)
