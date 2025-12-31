#ifndef PROGRAMLOADER_TEST_H
#define PROGRAMLOADER_TEST_H

#include <QtTest>

class TestProgramLoader : public QObject {
    Q_OBJECT

public slots:
    void program_loader();

private slots:
    void debug_info_sequences();
    void debug_info_load_failure();
    void debug_info_dwarf5_data();
    void debug_info_dwarf5();
    void debug_info_header_failure_data();
    void debug_info_header_failure();
};

#endif // PROGRAMLOADER_TEST_H
