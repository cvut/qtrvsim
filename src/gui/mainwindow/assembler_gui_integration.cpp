#include "assembler_gui_integration.h"

#include "windows/editor/editordock.h"
#include "windows/editor/srceditor.h"
#include <QTextBlock>

AssemblerGuiIntegration::AssemblerGuiIntegration(EditorDock *editor_dock, QObject *parent)
    : Super(parent)
    , editor_dock(editor_dock) {}

bool AssemblerGuiIntegration::process_file(const QString &filename, QString *error_ptr) {
    EditorTab *tab = editor_dock->find_tab_by_filename(filename);
    if (tab == nullptr) { return Super::process_file(filename, error_ptr); }
    SrcEditor *editor = tab->get_editor();
    QTextDocument *doc = editor->document();
    int ln = 1;
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next(), ln++) {
        QString line = block.text();
        process_line(line, filename, ln);
    }
    return !error_occured;
}

bool AssemblerGuiIntegration::process_pragma(
    QStringList &operands,
    const QString &filename,
    int line_number,
    QString *error_ptr) {
    (void)error_ptr;
    if (operands.count() < 2
        || (QString::compare(operands.at(0), "qtrvsim", Qt::CaseInsensitive)
            && QString::compare(operands.at(0), "qtmips", Qt::CaseInsensitive))) {
        return true;
    }
    QString op = operands.at(1).toLower();
    if (op == "show") {
        if (operands.count() < 3) { return true; }
        emit dock_show_requested(operands.at(2), filename, line_number);
        return true;
    }
    if (op == "tab") {
        if ((operands.count() < 3) || error_occured) { return true; }
        emit tab_switch_requested(operands.at(2), filename, line_number);
        return true;
    }
    if (op == "focus") {
        bool ok;
        if (operands.count() < 4) { return true; }
        fixmatheval::FmeExpression expression;
        fixmatheval::FmeValue value;
        QString error;
        ok = expression.parse(operands.at(3), error);
        if (!ok) {
            emit report_message(
                messagetype::MSG_WARNING, filename, line_number, 0,
                "expression parse error " + error, "");
            return true;
        }
        ok = expression.eval(value, symtab, error, next_instr_dest_addr);
        if (!ok) {
            emit report_message(
                messagetype::MSG_WARNING, filename, line_number, 0,
                "expression evaluation error " + error, "");
            return true;
        }
        if (!QString::compare(operands.at(2), "memory", Qt::CaseInsensitive)) {
            emit memory_focus_requested(machine::Address(value));
            return true;
        }
        if (!QString::compare(operands.at(2), "program", Qt::CaseInsensitive)) {
            emit program_focus_requested(machine::Address(value));
            return true;
        }
        emit report_message(
            messagetype::MSG_WARNING, filename, line_number, 0,
            "unknown #pragma qtrvsim focus unknown object " + operands.at(2), "");
        return true;
    }
    emit report_message(
        messagetype::MSG_WARNING, filename, line_number, 0, "unknown #pragma qtrvsim " + op, "");
    return true;
}