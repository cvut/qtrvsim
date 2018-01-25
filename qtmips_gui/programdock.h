#ifndef PROGRAMDOCK_H
#define PROGRAMDOCK_H

#include <QDockWidget>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include "qtmipsmachine.h"
#include "memoryview.h"

class ProgramView : public MemoryView {
    Q_OBJECT
public:
    ProgramView(QWidget *parent);

    void setup(machine::QtMipsMachine*);

    void jump_to_pc(std::uint32_t);

protected:
    QList<QWidget*> row_widget(std::uint32_t address, QWidget *parent);

private slots:
    void cb_single_changed(int index);
    void cb_pipelined_changed(int index);

private:
    QComboBox *cb_single;
    QComboBox *cb_pipelined;
};

class ProgramDock : public QDockWidget {
    Q_OBJECT
public:
    ProgramDock(QWidget *parent);

    void setup(machine::QtMipsMachine *machine);

public slots:
    void jump_to_pc(std::uint32_t);

private:
    ProgramView *view;
};

#endif // PROGRAMDOCK_H
