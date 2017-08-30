#ifndef MACHINEAPP_H
#define MACHINEAPP_H

#include <QCoreApplication>

#include "qtmipsmachine.h"
#include "programloader.h"

class MachineApp : public QCoreApplication {
public:
    MachineApp(int argc, char **argv);
private:
    QtMipsMachine *machine;
};

#endif // MACHINEAPP_H
