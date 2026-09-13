#include "editordock.test.h"

#include "editordock.h"

#include <QTemporaryDir>
#include <limits>

namespace {

struct EditorFixture {
    QTemporaryDir dir;
    QSharedPointer<QSettings> settings {
        new QSettings(dir.filePath("settings.ini"), QSettings::IniFormat)
    };
    QTabWidget parent;
    QWidget core;
    EditorDock dock { settings, &parent };

    EditorFixture() {
        parent.addTab(&core, "Core");
        parent.addTab(&dock, "Editor");
    }

    QString write_source(const QString &name) {
        const auto filename = dir.filePath(name);
        QFile file(filename);
        const QByteArray contents("addi x1, x0, 1\naddi x2, x0, 2\n");
        if (!file.open(QIODevice::WriteOnly) || file.write(contents) != contents.size()) {
            return {};
        }
        return filename;
    }
};

} // namespace

void TestEditorDock::follow_location_data() {
    QTest::addColumn<bool>("existing_file");
    QTest::addColumn<bool>("other_tab");
    QTest::addColumn<bool>("follow");
    QTest::addColumn<bool>("auto_open");
    QTest::newRow("existing-background") << true << true << false << true;
    QTest::newRow("new-background") << false << true << false << true;
    QTest::newRow("first-background") << false << false << false << true;
    QTest::newRow("follow-existing") << true << true << true << false;
    QTest::newRow("follow-current") << true << false << true << false;
    QTest::newRow("follow-current-auto-open") << true << false << true << true;
    QTest::newRow("current-background") << true << false << false << true;
    QTest::newRow("follow-new") << false << true << true << true;
    QTest::newRow("no-auto-open") << false << true << true << false;
    QTest::newRow("disabled") << false << true << false << false;
}

void TestEditorDock::follow_location() {
    QFETCH(bool, existing_file);
    QFETCH(bool, other_tab);
    QFETCH(bool, follow);
    QFETCH(bool, auto_open);
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const auto filename = dir.filePath("source.S");
    QFile source(filename);
    QVERIFY(source.open(QIODevice::WriteOnly));
    const QByteArray contents("addi x1, x0, 1\naddi x2, x0, 2\n");
    QCOMPARE(source.write(contents), qint64(contents.size()));
    source.close();
    QSharedPointer<QSettings> settings(
        new QSettings(dir.filePath("settings.ini"), QSettings::IniFormat));
    QTabWidget parent_tabs;
    QWidget core;
    parent_tabs.addTab(&core, "Core");
    EditorDock dock(settings, &parent_tabs);
    parent_tabs.addTab(&dock, "Editor");
    if (existing_file) { QVERIFY(dock.open_file(filename) != nullptr); }
    if (other_tab) { dock.create_empty_tab(); }
    QCoreApplication::processEvents();
    parent_tabs.setCurrentWidget(&core);
    auto *previous = dock.currentWidget();
    QSignalSpy available(&dock, &EditorDock::editor_available_changed);

    debuginfo::DebugInfo info;
    info.add_line(0x200, info.get_file_id(filename.toStdString()), 2);
    info.add_end_sequence(0x204);
    info.finalize();
    size_t hint = 0;
    dock.follow_debug_location(&info, 0x200, &hint, follow, auto_open);
    QCoreApplication::processEvents();

    auto *tab = dock.find_tab_by_filename(filename);
    QCOMPARE(tab != nullptr, existing_file || auto_open);
    if (follow && tab) {
        QCOMPARE(parent_tabs.currentWidget(), static_cast<QWidget *>(&dock));
        QCOMPARE(dock.currentWidget(), static_cast<QWidget *>(tab));
        QCOMPARE(tab->get_editor()->textCursor().blockNumber(), 1);
        QCOMPARE(tab->get_editor()->extraSelections().size(), 1);
        tab->get_editor()->insertPlainText(" ");
        QVERIFY(tab->get_editor()->extraSelections().isEmpty());
    } else {
        QCOMPARE(parent_tabs.currentWidget(), &core);
        if (previous) { QCOMPARE(dock.currentWidget(), previous); }
        if (tab) { QVERIFY(tab->get_editor()->extraSelections().isEmpty()); }
    }
    if (!existing_file && auto_open) { QVERIFY(!available.isEmpty()); }
}

void TestEditorDock::navigate_to_source_data() {
    QTest::addColumn<bool>("exists");
    QTest::addColumn<bool>("already_open");
    QTest::addColumn<quint32>("line");
    QTest::addColumn<bool>("expected");
    QTest::newRow("open-source") << true << true << quint32(2) << true;
    QTest::newRow("new-source") << true << false << quint32(2) << true;
    QTest::newRow("missing-source") << false << false << quint32(2) << false;
    QTest::newRow("zero-line") << true << true << quint32(0) << false;
    QTest::newRow("unavailable-line") << true << true << quint32(10) << false;
    QTest::newRow("unavailable-line-new-file") << true << false << quint32(10) << false;
    QTest::newRow("overflow-line") << true << true << std::numeric_limits<quint32>::max() << false;
}

void TestEditorDock::navigate_to_source() {
    QFETCH(bool, exists);
    QFETCH(bool, already_open);
    QFETCH(quint32, line);
    QFETCH(bool, expected);
    EditorFixture fixture;
    QVERIFY(fixture.dir.isValid());
    const auto filename
        = exists ? fixture.write_source("source.S") : fixture.dir.filePath("missing.S");
    QVERIFY(!filename.isEmpty());
    if (already_open) { QVERIFY(fixture.dock.open_file(filename) != nullptr); }
    auto *previous = fixture.dock.create_empty_tab();
    auto *previous_editor = previous->get_editor();
    previous_editor->setPlainText("unrelated first line\nunrelated second line");
    previous_editor->setCursorTo(2, 1);
    previous_editor->setLineHighlight(SrcEditor::LineHighlight::Diagnostic, Qt::red);
    QCoreApplication::processEvents();
    fixture.parent.setCurrentWidget(&fixture.core);
    const auto previous_position = previous_editor->textCursor().position();

    auto *editor = fixture.dock.navigate_to_source(filename, line);
    QCOMPARE(editor != nullptr, expected);
    if (editor) {
        editor->setLineHighlight(SrcEditor::LineHighlight::Navigation, Qt::yellow);
        QCOMPARE(editor->filename(), filename);
        QCOMPARE(editor->textCursor().blockNumber(), 1);
        QCOMPARE(editor->extraSelections().size(), 1);
        QCOMPARE(fixture.parent.currentWidget(), static_cast<QWidget *>(&fixture.dock));
    }
    QCoreApplication::processEvents();
    if (!expected) {
        QCOMPARE(fixture.dock.currentWidget(), static_cast<QWidget *>(previous));
        QCOMPARE(fixture.parent.currentWidget(), &fixture.core);
    }
    QCOMPARE(previous_editor->textCursor().position(), previous_position);
    QCOMPARE(previous_editor->extraSelections().size(), 1);
    QCOMPARE(
        previous_editor->extraSelections().first().format.background().color(), QColor(Qt::red));
}

void TestEditorDock::execution_highlight_moves() {
    EditorFixture fixture;
    QVERIFY(fixture.dir.isValid());
    const auto first_file = fixture.write_source("first.S");
    const auto second_file = fixture.write_source("second.S");
    QVERIFY(!first_file.isEmpty());
    QVERIFY(!second_file.isEmpty());
    auto *first_tab = fixture.dock.open_file(first_file);
    auto *second_tab = fixture.dock.open_file(second_file);
    QVERIFY(first_tab != nullptr);
    QVERIFY(second_tab != nullptr);
    auto *first = first_tab->get_editor();
    auto *second = second_tab->get_editor();
    first->setLineHighlight(SrcEditor::LineHighlight::Diagnostic, Qt::red);
    first->setLineHighlight(SrcEditor::LineHighlight::Navigation, Qt::yellow);
    second->setLineHighlight(SrcEditor::LineHighlight::Navigation, Qt::yellow);

    debuginfo::DebugInfo info;
    info.add_line(0x200, info.get_file_id(first_file.toStdString()), 2);
    info.add_line(0x204, info.get_file_id(second_file.toStdString()), 1);
    info.add_end_sequence(0x208);
    info.finalize();
    fixture.dock.follow_debug_location(&info, 0x200, nullptr, true, false);
    fixture.dock.follow_debug_location(&info, 0x200, nullptr, true, false);
    QCoreApplication::processEvents();
    QCOMPARE(first->extraSelections().size(), 3);
    QCOMPARE(
        first->extraSelections().first().format.background().color(),
        QColor(Qt::green).lighter(180));
    QCOMPARE(first->extraSelections().first().cursor.blockNumber(), 1);
    QCOMPARE(first->extraSelections().at(1).format.background().color(), QColor(Qt::red));
    QCOMPARE(second->extraSelections().size(), 1);

    fixture.dock.follow_debug_location(&info, 0x204, nullptr, true, false);
    QCOMPARE(first->extraSelections().size(), 2);
    QCOMPARE(first->extraSelections().first().format.background().color(), QColor(Qt::red));
    QCOMPARE(first->extraSelections().last().format.background().color(), QColor(Qt::yellow));
    QCOMPARE(second->extraSelections().size(), 2);
    QCOMPARE(second->extraSelections().first().cursor.blockNumber(), 0);
    fixture.dock.follow_debug_location(&info, 0x208, nullptr, true, false);
    QCOMPARE(second->extraSelections().size(), 1);
    QCOMPARE(second->extraSelections().first().format.background().color(), QColor(Qt::yellow));
}

void TestEditorDock::execution_highlight_clears_data() {
    QTest::addColumn<QString>("reason");
    for (const char *reason :
         { "gap", "no-debug-info", "empty-debug-info", "line-zero", "missing-source",
           "invalid-line", "empty-path", "disabled", "auto-open-only", "reset" }) {
        QTest::newRow(reason) << QString(reason);
    }
}

void TestEditorDock::execution_highlight_clears() {
    QFETCH(QString, reason);
    EditorFixture fixture;
    QVERIFY(fixture.dir.isValid());
    const auto filename = fixture.write_source("source.S");
    const auto next_filename = fixture.write_source("next.S");
    QVERIFY(!filename.isEmpty());
    QVERIFY(!next_filename.isEmpty());
    auto *tab = fixture.dock.open_file(filename);
    QVERIFY(tab != nullptr);
    auto *editor = tab->get_editor();
    editor->setLineHighlight(SrcEditor::LineHighlight::Diagnostic, Qt::red);

    debuginfo::DebugInfo info;
    const auto file = info.get_file_id(filename.toStdString());
    info.add_line(0x200, file, 1);
    info.add_end_sequence(0x204);
    info.add_line(0x208, info.get_file_id(fixture.dir.filePath("missing.S").toStdString()), 1);
    info.add_line(0x20c, file, 0);
    info.add_line(0x210, file, 100);
    info.add_line(0x214, info.get_file_id(""), 1);
    info.add_line(0x218, info.get_file_id(next_filename.toStdString()), 1);
    info.add_end_sequence(0x21c);
    info.finalize();
    fixture.dock.follow_debug_location(&info, 0x200, nullptr, true, false);
    QCoreApplication::processEvents();
    QCOMPARE(editor->extraSelections().size(), 2);
    fixture.parent.setCurrentWidget(&fixture.core);

    if (reason == "reset") {
        fixture.dock.clear_execution_highlight();
    } else {
        uint64_t pc = 0x200;
        if (reason == "gap") { pc = 0x204; }
        if (reason == "missing-source") { pc = 0x208; }
        if (reason == "line-zero") { pc = 0x20c; }
        if (reason == "invalid-line") { pc = 0x210; }
        if (reason == "empty-path") { pc = 0x214; }
        if (reason == "auto-open-only") { pc = 0x218; }
        if (reason == "empty-debug-info") { info.clear(); }
        fixture.dock.follow_debug_location(
            reason == "no-debug-info" ? nullptr : &info, pc, nullptr,
            reason != "disabled" && reason != "auto-open-only",
            reason == "auto-open-only" || reason == "missing-source");
    }
    QCoreApplication::processEvents();
    QCOMPARE(editor->extraSelections().size(), 1);
    QCOMPARE(editor->extraSelections().first().format.background().color(), QColor(Qt::red));
    QCOMPARE(fixture.dock.currentWidget(), static_cast<QWidget *>(tab));
    QCOMPARE(fixture.parent.currentWidget(), &fixture.core);
    if (reason == "auto-open-only") {
        QVERIFY(fixture.dock.find_tab_by_filename(next_filename) != nullptr);
    }
}

void TestEditorDock::execution_highlight_survives_tab_close() {
    EditorFixture fixture;
    QVERIFY(fixture.dir.isValid());
    const auto filename = fixture.write_source("source.S");
    QVERIFY(!filename.isEmpty());
    auto *tab = fixture.dock.open_file(filename);
    QVERIFY(tab != nullptr);
    QPointer<SrcEditor> editor = tab->get_editor();
    debuginfo::DebugInfo info;
    info.add_line(0x200, info.get_file_id(filename.toStdString()), 1);
    info.add_end_sequence(0x204);
    info.finalize();
    fixture.dock.follow_debug_location(&info, 0x200, nullptr, true, false);
    fixture.dock.close_current_tab();
    QVERIFY(editor.isNull());
    fixture.dock.clear_execution_highlight();
    fixture.dock.follow_debug_location(&info, 0x200, nullptr, true, true);
    QCoreApplication::processEvents();
    QCOMPARE(fixture.dock.count(), 1);
    QCOMPARE(fixture.dock.get_current_editor()->extraSelections().size(), 1);
}

void TestEditorDock::failed_open_cache() {
    EditorFixture fixture;
    const QString non_existent = fixture.dir.filePath("non_existent_file.S");

    // First attempt fails and caches negative result
    QVERIFY(fixture.dock.open_file(non_existent) == nullptr);
    QCOMPARE(fixture.dock.count(), 0);

    // Follow location with auto_open does not construct or add tabs
    debuginfo::DebugInfo info;
    info.add_line(0x200, info.get_file_id(non_existent.toStdString()), 1);
    info.add_end_sequence(0x204);
    info.finalize();
    fixture.dock.follow_debug_location(&info, 0x200, nullptr, true, true);
    QCOMPARE(fixture.dock.count(), 0);

    // Creating the file and clearing cache allows subsequent open
    QFile file(non_existent);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("nop\n");
    file.close();

    fixture.dock.clear_failed_open_cache();
    QVERIFY(fixture.dock.open_file(non_existent) != nullptr);
    QCOMPARE(fixture.dock.count(), 1);
}

#ifndef __has_feature
    #define __has_feature(x) 0
#endif

#if defined(__SANITIZE_ADDRESS__) || __has_feature(address_sanitizer)
extern "C" __attribute__((used, visibility("default"))) const char *__lsan_default_suppressions() {
    return "leak:libqoffscreen\n"
           "leak:QPlatformScreen\n"
           "leak:handleScreenAdded\n"
           "leak:libfontconfig\n"
           "leak:QFont\n"
           "leak:QFontDatabase\n"
           "leak:QFontEngine\n"
           "leak:ExternalRefCountData\n"
           "leak:QObject::QObject\n"
           "leak:QObjectPrivate\n";
}
#endif

QTEST_MAIN(TestEditorDock)
