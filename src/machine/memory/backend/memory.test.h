#ifndef MEMORY_TEST_H
#define MEMORY_TEST_H

#include <QtTest>

class TestMemory : public QObject {
    Q_OBJECT

private:
    static void integer_decomposition();

public slots:
    static void memory();
    static void memory_data();
    static void memory_section();
    static void memory_section_data();
    void memory_compare();
    void memory_compare_data();
    static void memory_write_ctl_data();
    static void memory_write_ctl();
    static void memory_read_ctl_data();
    static void memory_read_ctl();
};

#endif // MEMORY_TEST_H