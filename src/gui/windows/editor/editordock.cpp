#include "editordock.h"

#include "common/logging.h"
#include "debuginfo/debuginfo.h"
#include "dialogs/savechanged/savechangeddialog.h"
#include "editortab.h"
#include "helper/async_modal.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QScopedValueRollback>
#include <QTextEdit>
#include <QTimer>
#include <qtabbar.h>
#include <utility>

LOG_CATEGORY("gui.editordock");

#ifdef __EMSCRIPTEN__
    #include "qhtml5file.h"
#endif

int compare_filenames(const QString &filename1, const QString &filename2) {
    QFileInfo fi1(filename1);
    QFileInfo fi2(filename2);
    QString canon1 = fi1.canonicalFilePath();
    QString canon2 = fi2.canonicalFilePath();
    if (!canon1.isEmpty() && (canon1 == canon2)) { return 2; }
    if (filename1 == filename2) { return 1; }
    return 0;
}

EditorDock::EditorDock(QSharedPointer<QSettings> settings, QTabWidget *parent_tabs, QWidget *parent)
    : Super(parent)
    , settings(std::move(settings))
    , parent_tabs(parent_tabs) {
    {
        auto bar = tabBar();
        bar->setMovable(true);
        QFont font = bar->font();
        font.setPointSize(10);
        font.setBold(false);
        bar->setFont(font);
    }

    setObjectName("EditorDock");
    setTabsClosable(true);
    connect(this, &EditorDock::tabCloseRequested, this, [this](int index) { close_tab(index); });

    connect(this, &EditorDock::currentChanged, this, [this](int) {
        if (activate_tab_changes && this->parent_tabs && currentIndex() >= 0) {
            this->parent_tabs->setCurrentWidget(this);
        }
        QTimer::singleShot(0, this, [this]() {
            if (!this->parent_tabs || count() == 0 || currentIndex() < 0) return;
            auto *editor = get_current_editor();
            QString title = QString("&Editor (%1)").arg(editor->title());
            this->parent_tabs->setTabText(this->parent_tabs->indexOf(this), title);
            // IMPORTANT: This repeated call solves a Qt resize bug. Do not remove it!
            this->parent_tabs->setTabText(this->parent_tabs->indexOf(this), title);
        });
    });
}

void EditorDock::activate_tab(EditorTab *tab, bool activate_in_parent) {
    setCurrentWidget(tab);
    if (parent_tabs && activate_in_parent) { parent_tabs->setCurrentWidget(this); }
}

EditorTab *EditorDock::get_tab(int index) const {
    return dynamic_cast<EditorTab *>(widget(index));
}

EditorTab *EditorDock::open_file(const QString &filename, bool save_as_required, bool activate) {
    if (unopenable_files.contains(filename)) { return nullptr; }
    auto tab = new EditorTab(line_numbers_visible, this);
    if (tab->get_editor()->loadFile(filename)) {
        unopenable_files.remove(filename);
        QScopedValueRollback<bool> activation_guard(activate_tab_changes, activate);
        addTab(tab, tab->title());
        if (activate) { activate_tab(tab); }
        if (save_as_required) tab->get_editor()->setSaveAsRequired(save_as_required);
        return tab;
    } else {
        unopenable_files.insert(filename);
        delete tab;
        return nullptr;
    }
}

EditorTab *
EditorDock::open_file_if_not_open(const QString &filename, bool save_as_required, bool activate) {
    auto tab = find_tab_by_filename(filename);
    if (tab == nullptr) {
        return open_file(filename, save_as_required, activate);
    } else {
        if (activate) { activate_tab(tab); }
        return tab;
    }
}

EditorTab *EditorDock::create_empty_tab() {
    auto tab = new EditorTab(line_numbers_visible, this);
    while (true) {
        auto filename = QString("Unknown %1").arg(unknown_editor_counter++);
        if (!find_tab_id_by_filename(filename).has_value()) {
            tab->get_editor()->setFileName(filename);
            tab->get_editor()->setSaveAsRequired(true);
            break;
        }
    }
    addTab(tab, tab->title());
    activate_tab(tab);
    return tab;
}

std::optional<int> EditorDock::find_tab_id_by_filename(const QString &filename) const {
    int best_match = 0;
    int best_match_index = -1;
    for (int i = 0; i < this->count(); i++) {
        auto *editor = get_tab(i)->get_editor();
        int match = compare_filenames(filename, editor->filename());
        if (match == 2) { return i; }
        if (match > best_match) {
            best_match = match;
            best_match_index = i;
        }
    }
    if (best_match_index >= 0) { return best_match_index; }
    return std::nullopt;
}

EditorTab *EditorDock::find_tab_by_filename(const QString &filename) const {
    auto index = find_tab_id_by_filename(filename);
    if (index.has_value()) {
        return get_tab(index.value());
    } else {
        return nullptr;
    }
}

SrcEditor *EditorDock::get_current_editor() const {
    if (count() == 0) return nullptr;
    return get_tab(currentIndex())->get_editor();
}

QStringList EditorDock::get_open_file_list() const {
    QStringList open_src_files;
    for (int i = 0; i < this->count(); i++) {
        auto *editor = get_tab(i)->get_editor();
        if (editor->filename().isEmpty()) { continue; }
        open_src_files.append(editor->filename());
    }
    return open_src_files;
}

bool EditorDock::get_modified_tab_filenames(QStringList &output, bool report_unnamed) const {
    output.clear();
    for (int i = 0; i < this->count(); i++) {
        auto editor = get_tab(i)->get_editor();
        if (editor->filename().isEmpty() && !report_unnamed) { continue; }
        if (!editor->isModified()) { continue; }
        output.append(editor->filename());
    }
    return !output.empty();
}

void EditorDock::set_show_line_numbers(bool visible) {
    line_numbers_visible = visible;
    settings->setValue("EditorShowLineNumbers", visible);
    for (int i = 0; i < this->count(); i++) {
        get_tab(i)->set_show_line_number(visible);
    }
}

void EditorDock::tabCountChanged() {
    Super::tabCountChanged();
    emit editor_available_changed(count() > 0);
}

void EditorDock::open_file_dialog() {
#ifndef __EMSCRIPTEN__
    QString file_name = QFileDialog::getOpenFileName(
        this, tr("Open File"), "", "Source Files (*.asm *.S *.s *.c Makefile)");

    if (file_name.isEmpty()) { return; }

    auto tab_id = find_tab_id_by_filename(file_name);
    if (tab_id.has_value()) {
        activate_tab(get_tab(tab_id.value()));
        return;
    }

    if (!open_file(file_name)) {
        showAsyncCriticalBox(
            this, "Simulator Error", tr("Cannot open file '%1' for reading.").arg(file_name));
    }
#else
    QHtml5File::load("*", [&](const QByteArray &content, const QString &filename) {
        auto tab = create_empty_tab();
        tab->get_editor()->loadByteArray(content, filename);
        setTabText(indexOf(tab), tab->get_editor()->title());
    });
#endif
}

void EditorDock::save_tab(int index) {
    auto editor = get_tab(index)->get_editor();
    if (editor->saveAsRequired()) { return save_tab_as(index); }
#ifndef __EMSCRIPTEN__
    if (!editor->saveFile()) {
        showAsyncCriticalBox(
            this, "Simulator Error", tr("Cannot save file '%1'.").arg(editor->filename()));
    }
#else
    QHtml5File::save(editor->document()->toPlainText().toUtf8(), editor->filename());
    editor->setModified(false);
#endif
}

void EditorDock::save_current_tab() {
    if (count() == 0) return;
    save_tab(currentIndex());
}

void EditorDock::save_tab_as(int index) {
#ifndef __EMSCRIPTEN__
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setDefaultSuffix("s");
    if (fileDialog.exec() != QDialog::Accepted) { return; }
    const QString fn = fileDialog.selectedFiles().first();
    auto tab = get_tab(index);
    if (!tab->get_editor()->saveFile(fn)) {
        showAsyncCriticalBox(this, "Simulator Error", tr("Cannot save file '%1'.").arg(fn));
        return;
    }
    setTabText(index, tab->get_editor()->title());
    emit currentChanged(index);
#else
    QString filename = get_tab(index)->get_editor()->filename();
    if (filename.isEmpty()) filename = "unknown.s";
    auto *dialog = new QInputDialog(this);
    dialog->setWindowTitle("Select file name");
    dialog->setLabelText("File name:");
    dialog->setTextValue(filename);
    dialog->setMinimumSize(QSize(200, 100));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(
        dialog, &QInputDialog::textValueSelected, this,
        [this, index](const QString &filename) { save_tab_to(index, filename); },
        Qt::QueuedConnection);
    dialog->open();
#endif
}

void EditorDock::save_current_tab_as() {
    if (count() == 0) return;
    save_tab_as(currentIndex());
}

void EditorDock::save_tab_to(int index, const QString &filename) {
    if (filename.isEmpty()) {
        WARN("Cannot save file '%s'.", filename.toStdString().c_str());
        return;
    }

    auto editor = get_tab(index)->get_editor();
    if (filename.isEmpty() || (editor == nullptr)) { return; }
    editor->setFileName(filename);
    if (!editor->filename().isEmpty()) { save_current_tab(); }
}

void EditorDock::save_current_tab_to(const QString &filename) {
    if (count() == 0) return;
    save_tab_to(currentIndex(), filename);
}

void EditorDock::close_tab(int index) {
    auto editor = get_tab(index)->get_editor();
    if (!editor->isModified()) {
        close_tab_unchecked(index);
    } else {
        confirm_close_tab_dialog(index);
    }
}

void EditorDock::close_current_tab() {
    if (count() == 0) return;
    close_tab(currentIndex());
}

void EditorDock::close_tab_by_name(QString &filename, bool ask) {
    auto *tab = find_tab_by_filename(filename);
    if (tab == nullptr) {
        WARN("Cannot find tab for file '%s'. Unable to close it.", filename.toStdString().c_str());
        return;
    }
    if (!ask) {
        close_tab(indexOf(tab));
    } else {
        confirm_close_tab_dialog(indexOf(tab));
    }
}
void EditorDock::close_tab_unchecked(int index) {
    auto *tab = get_tab(index);
    removeTab(index);
    delete tab;
}

void EditorDock::confirm_close_tab_dialog(int index) {
    auto *msgbox = new QMessageBox(this);
    msgbox->setWindowTitle("Close unsaved source");
    msgbox->setText("Close unsaved source.");
    msgbox->setInformativeText("Do you want to save your changes?");
    msgbox->setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgbox->setDefaultButton(QMessageBox::Save);
    msgbox->setMinimumSize(QSize(200, 150));
    msgbox->setAttribute(Qt::WA_DeleteOnClose);
    connect(
        msgbox, &QDialog::finished, this,
        [this, index](int result) {
            if (result == QMessageBox::Save) {
                save_tab(index);
                close_tab_unchecked(index);
            } else if (result == QMessageBox::Discard) {
                close_tab_unchecked(index);
            }
        },
        Qt::QueuedConnection);
    msgbox->open();
}

bool EditorDock::set_cursor_to(const QString &filename, int line, int column, bool center) {
    auto tab = (filename == "Unknown") ? get_tab(currentIndex()) : find_tab_by_filename(filename);
    if (tab == nullptr) {
        WARN(
            "Cannot find tab for file '%s'. Unable to set cursor.", filename.toStdString().c_str());
        return false;
    }
    activate_tab(tab);
    tab->get_editor()->setCursorTo(line, column, center);
    return true;
}

SrcEditor *EditorDock::navigate_to_source(
    const QString &filename,
    uint32_t line,
    bool auto_open,
    bool set_focus) {
    if (filename.isEmpty()) { return nullptr; }
    auto *tab = auto_open ? open_file_if_not_open(filename, false, false)
                          : find_tab_by_filename(filename);
    if (!tab) { return nullptr; }
    auto *editor = tab->get_editor();
    if (line == 0 || line > static_cast<uint32_t>(editor->blockCount())) { return nullptr; }
    activate_tab(tab, set_focus);
    editor->setCursorTo(static_cast<int>(line), 1, false, set_focus);
    return editor;
}

void EditorDock::clear_execution_highlight() {
    if (execution_editor) {
        execution_editor->clearLineHighlight(SrcEditor::LineHighlight::Execution);
        execution_editor.clear();
    }
}

void EditorDock::clear_failed_open_cache() {
    unopenable_files.clear();
}

void EditorDock::follow_debug_location(
    debuginfo::DebugInfo *debug_info,
    uint64_t pc,
    size_t *hint,
    bool follow,
    bool auto_open,
    bool set_focus) {
    if (!follow) { clear_execution_highlight(); }
    if (!follow && !auto_open) { return; }
    if (!debug_info) {
        clear_execution_highlight();
        return;
    }

    auto *loc = debug_info->find(pc, hint);
    if (!loc) {
        clear_execution_highlight();
        return;
    }

    QString file = QString::fromStdString(debug_info->get_file_path(loc->file_id));
    if (!follow) {
        if (auto_open && !file.isEmpty()) { open_file_if_not_open(file, false, set_focus); }
        return;
    }

    auto *editor = navigate_to_source(file, loc->line, auto_open, set_focus);
    if (execution_editor != editor) { clear_execution_highlight(); }
    if (editor) {
        execution_editor = editor;
        editor->setLineHighlight(
            SrcEditor::LineHighlight::Execution, QColor(Qt::green).lighter(180));
    }
}
