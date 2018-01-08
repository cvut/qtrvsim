#ifndef MEMORYDOCK_H
#define MEMORYDOCK_H

#include <QDockWidget>
#include "qtmipsmachine.h"
#include "memoryview.h"

class MemoryDock : public QDockWidget  {
    Q_OBJECT
public:
    MemoryDock(QWidget *parent);
    ~MemoryDock();

    void setup(machine::QtMipsMachine *machine);

private:
    MemoryView *memory_view;
};

#endif // MEMORYDOCK_H
