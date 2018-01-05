#ifndef MEMORYDOCK_H
#define MEMORYDOCK_H

#include <QDockWidget>
#include "qtmipsmachine.h"

class MemoryDock : public QDockWidget  {
    Q_OBJECT
public:
    MemoryDock(QWidget *parent);
    ~MemoryDock();

    void setup(machine::QtMipsMachine *machine);

private:
    // TODO memory view
};

#endif // MEMORYDOCK_H
