#ifndef MACHINE_TEST_H
#define MACHINE_TEST_H

#include <QtTest>

class TestMachine : public QObject {
    Q_OBJECT

private slots:
    void restart_restores_elf_debug_info();
    void restart_retains_internal_debug_info();
    void keep_memory_preserves_debug_info();
};

#endif // MACHINE_TEST_H
