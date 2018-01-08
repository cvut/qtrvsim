#ifndef PROGRAMDOCK_H
#define PROGRAMDOCK_H

#include <QDockWidget>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include "qtmipsmachine.h"
#include "memoryview.h"

class ProgramDock : public QDockWidget {
    Q_OBJECT
public:
    ProgramDock(QWidget *parent);
    ~ProgramDock();

    void setup(machine::QtMipsMachine *machine);

private slots:
    void ctlbox_single_changed(int index);
    void ctlbox_pipelined_changed(int index);

private:
    QWidget *widg;
    QBoxLayout *widg_layout;

    MemoryView *memory_view;
    QComboBox *ctlbox_single;
    QComboBox *ctlbox_pipelined;
};

#endif // PROGRAMDOCK_H
