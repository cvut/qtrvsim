#ifndef REGISTERS_TEST_H
#define REGISTERS_TEST_H

#include <QtTest>

class TestRegisters : public QObject {
    Q_OBJECT

public slots:
    static void registers_gp0();
    static void registers_rw_gp();
    static void registers_compare();
};

#endif // REGISTERS_TEST_H
