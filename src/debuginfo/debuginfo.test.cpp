#include "debuginfo.test.h"

#include "common/polyfills/qt5/qtest.h"
#include "debuginfo.h"

#include <stdexcept>

using namespace debuginfo;

void TestDebugInfo::test_file_id_assignment() {
    DebugInfo info;

    FileId id1 = info.get_file_id("file1.c");
    FileId id2 = info.get_file_id("file2.c");
    FileId id3 = info.get_file_id("file3.c");

    QCOMPARE(id1, FileId(0));
    QCOMPARE(id2, FileId(1));
    QCOMPARE(id3, FileId(2));

    QCOMPARE(info.get_file_path(id1), std::string("file1.c"));
    QCOMPARE(info.get_file_path(id2), std::string("file2.c"));
    QCOMPARE(info.get_file_path(id3), std::string("file3.c"));
}

void TestDebugInfo::test_file_id_deduplication() {
    DebugInfo info;

    FileId id1 = info.get_file_id("file.c");
    FileId id2 = info.get_file_id("other.c");
    FileId id3 = info.get_file_id("file.c"); // duplicate

    QCOMPARE(id1, id3);
    QVERIFY(id1 != id2);
}

void TestDebugInfo::test_add_and_find_single() {
    DebugInfo info;

    FileId file = info.get_file_id("test.c");
    info.add_line(0x1000, file, 42);
    info.finalize();

    const SourceLocation *loc = info.find(0x1000);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->file_id, file);
    QCOMPARE(loc->line, 42u);
}

void TestDebugInfo::test_find_returns_containing_location() {
    DebugInfo info;

    FileId file = info.get_file_id("test.c");
    // Simulate: line 10 starts at 0x100, line 20 starts at 0x110
    info.add_line(0x100, file, 10);
    info.add_line(0x110, file, 20);
    info.add_line(0x120, file, 30);
    info.finalize();

    // Query address 0x108 (between 0x100 and 0x110)
    // Should return line 10, not line 20
    const SourceLocation *loc = info.find(0x108);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 10u);

    // Query address 0x115 (between 0x110 and 0x120)
    loc = info.find(0x115);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 20u);
}

void TestDebugInfo::test_find_exact_match() {
    DebugInfo info;

    FileId file = info.get_file_id("test.c");
    info.add_line(0x100, file, 10);
    info.add_line(0x110, file, 20);
    info.finalize();

    const SourceLocation *loc = info.find(0x100);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 10u);

    loc = info.find(0x110);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 20u);
}

void TestDebugInfo::test_find_before_all_entries() {
    DebugInfo info;

    FileId file = info.get_file_id("test.c");
    info.add_line(0x1000, file, 10);
    info.finalize();

    // Query address before all entries
    const SourceLocation *loc = info.find(0x500);
    QVERIFY(loc == nullptr);
}

void TestDebugInfo::test_find_after_last_entry() {
    DebugInfo info;

    FileId file = info.get_file_id("test.c");
    info.add_line(0x100, file, 10);
    info.add_line(0x200, file, 20);
    info.finalize();

    // Query address after all entries - should return last entry
    const SourceLocation *loc = info.find(0x500);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 20u);
}

void TestDebugInfo::test_find_empty() {
    DebugInfo info;
    info.finalize();

    const SourceLocation *loc = info.find(0x1000);
    QVERIFY(loc == nullptr);
}

void TestDebugInfo::test_finalize_sorts() {
    DebugInfo info;

    FileId file = info.get_file_id("test.c");
    // Add lines out of order
    info.add_line(0x300, file, 30);
    info.add_line(0x100, file, 10);
    info.add_line(0x200, file, 20);
    info.finalize();

    // Verify they are properly sorted by checking lookups
    const SourceLocation *loc = info.find(0x150);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 10u); // Should be line 10 (addr 0x100), not line 30

    loc = info.find(0x250);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 20u);
}

void TestDebugInfo::test_hint_sequential_access() {
    DebugInfo info;

    FileId file = info.get_file_id("test.c");
    for (uint32_t i = 0; i < 100; ++i) {
        info.add_line(0x1000 + i * 4, file, i + 1);
    }
    info.finalize();

    // Simulate sequential stepping through code
    size_t hint = 0;
    for (uint32_t i = 0; i < 100; ++i) {
        uint64_t addr = 0x1000 + i * 4;
        const SourceLocation *loc = info.find(addr, &hint);
        QVERIFY(loc != nullptr);
        QCOMPARE(loc->line, i + 1);
        // Hint should be updated
        QCOMPARE(hint, static_cast<size_t>(i));
    }
}

void TestDebugInfo::test_hint_does_not_affect_correctness() {
    DebugInfo info;

    FileId file = info.get_file_id("test.c");
    info.add_line(0x100, file, 10);
    info.add_line(0x200, file, 20);
    info.add_line(0x300, file, 30);
    info.add_line(0x400, file, 40);
    info.finalize();

    // Test with a bad hint that points to a completely wrong location
    size_t hint = 3;                                     // Points to 0x400
    const SourceLocation *loc = info.find(0x150, &hint); // Should find line 10
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 10u);

    // Test jumping backwards with hint
    hint = 2; // Points to 0x300
    loc = info.find(0x100, &hint);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 10u);

    // Test with hint beyond array bounds (should not crash)
    hint = 9999;
    loc = info.find(0x250, &hint);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 20u);
}

void TestDebugInfo::test_get_file_bounds_check() {
    DebugInfo info;

    info.get_file_id("file.c");

    // Valid access should work
    QCOMPARE(info.get_file_path(0), std::string("file.c"));

    // Invalid access should throw
    QVERIFY_THROWS_EXCEPTION(std::out_of_range, info.get_file_path(999));
}

void TestDebugInfo::test_clear() {
    DebugInfo info;

    FileId file = info.get_file_id("test.c");
    info.add_line(0x1000, file, 10);
    info.finalize();

    // Verify data exists
    QVERIFY(info.find(0x1000) != nullptr);

    info.clear();

    // Verify data is cleared
    QVERIFY(info.find(0x1000) == nullptr);
    QVERIFY_THROWS_EXCEPTION(std::out_of_range, info.get_file_path(0));
}

QTEST_APPLESS_MAIN(TestDebugInfo)