#ifndef INSTRUCTION_TEST_H
#define INSTRUCTION_TEST_H

#include <QtTest>

class TestInstruction : public QObject {
    Q_OBJECT

private slots:
    void instruction();
    void instruction_access();
    void instruction_to_str();
    void instruction_code_from_str();
};

#endif // INSTRUCTION_TEST_H
