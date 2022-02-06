#ifndef CACHE_TEST_H
#define CACHE_TEST_H

#include <QtTest>

class TestCache : public QObject {
    Q_OBJECT
private slots:
    static void cache_data();
    static void cache();
    static void cache_correctness_data();
    static void cache_correctness();
};

#endif // CACHE_TEST_H
