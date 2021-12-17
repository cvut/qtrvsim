#ifndef INSTRUCTION_TEST_H
#define INSTRUCTION_TEST_H

#include <QtTest>

class TestInstruction : public QObject {
    Q_OBJECT

public slots:
    void instruction();
    void instruction_access();
};

#endif // INSTRUCTION_TEST_H
