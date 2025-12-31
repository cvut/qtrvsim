#ifndef ASSEMBLERINTEGRATION_H
#define ASSEMBLERINTEGRATION_H

#include "assembler/simpleasm.h"
#include "machine/machine.h"

#include <QObject>

class EditorDock;

/** Implements support for assembler pragmas that interact with the GUI */
class AssemblerGuiIntegration : public SimpleAsm {
    Q_OBJECT
    using Super = SimpleAsm;

public:
    explicit AssemblerGuiIntegration(EditorDock *editor_dock, QObject *parent = nullptr);
    bool process_file(const QString &filename, QString *error_ptr = nullptr) override;

signals:
    void dock_show_requested(const QString &name, const QString &file, int line);
    void tab_switch_requested(const QString &name, const QString &file, int line);
    void memory_focus_requested(machine::Address addr);
    void program_focus_requested(machine::Address addr);

protected:
    bool process_pragma(
        QStringList &operands,
        const QString &filename,
        int line_number,
        QString *error_ptr) override;

private:
    EditorDock *editor_dock;
};

#endif // ASSEMBLERINTEGRATION_H
