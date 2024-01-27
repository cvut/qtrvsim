#include "editordock.h"

#include "common/logging.h"
#include "dialogs/savechanged/savechangeddialog.h"
#include "editortab.h"
#include "helper/async_modal.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
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
    , settings(std::move(settings)) {
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

    connect(
        this, &EditorDock::currentChanged, parent_tabs,
        [this, parent_tabs](int index) {
            // Update parent title
            if (count() == 0 || index < 0) return;
            auto *editor = get_tab(index)->get_editor();
            QString title = QString("&Editor (%1)").arg(editor->title());
            parent_tabs->setTabText(parent_tabs->indexOf(this), title);
            // IMPORTANT: This repeated call solved a very annoying QT resize bug. Do not remove it!
            parent_tabs->setTabText(parent_tabs->indexOf(this), title);
            parent_tabs->setCurrentIndex(parent_tabs->indexOf(this));
        },
        Qt::QueuedConnection);
}

EditorTab *EditorDock::get_tab(int index) const {
    return dynamic_cast<EditorTab *>(widget(index));
}

EditorTab *EditorDock::open_file(const QString &filename, bool save_as_required) {
    auto tab = new EditorTab(line_numbers_visible, this);
    if (tab->get_editor()->loadFile(filename)) {
        addTab(tab, tab->title());
        setCurrentWidget(tab);
        if (save_as_required)
            tab->get_editor()->setSaveAsRequired(save_as_required);
        return tab;
    } else {
        delete tab;
        return nullptr;
    }
}

EditorTab *EditorDock::open_file_if_not_open(const QString &filename, bool save_as_required) {
    auto tab = find_tab_by_filename(filename);
    if (tab == nullptr) {
        return open_file(filename, save_as_required);
    } else {
        setCurrentWidget(tab);
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
    setCurrentWidget(tab);
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
    settings->setValue("editorShowLineNumbers", visible);
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
        setCurrentIndex(tab_id.value());
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

bool EditorDock::set_cursor_to(const QString &filename, int line, int column) {
    auto tab = (filename == "Unknown") ? get_tab(currentIndex()) : find_tab_by_filename(filename);
    if (tab == nullptr) {
        WARN(
            "Cannot find tab for file '%s'. Unable to set cursor.", filename.toStdString().c_str());
        return false;
    }
    setCurrentWidget(tab);
    tab->get_editor()->setCursorTo(line, column);
    return true;
}
