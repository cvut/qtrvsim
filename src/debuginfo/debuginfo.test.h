#ifndef DEBUGINFO_TEST_H
#define DEBUGINFO_TEST_H

#include <QtTest>

class TestDebugInfo : public QObject {
    Q_OBJECT

private slots:
    void test_file_id_assignment();
    void test_file_id_deduplication();
    void test_add_and_find_single();
    void test_find_returns_containing_location();
    void test_find_exact_match();
    void test_find_before_all_entries();
    void test_find_after_last_entry();
    void test_find_empty();
    void test_finalize_sorts();
    void test_hint_sequential_access();
    void test_hint_does_not_affect_correctness();
    void test_get_file_bounds_check();
    void test_clear();
};

#endif // DEBUGINFO_TEST_H