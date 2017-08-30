#include <QCoreApplication>
#include <iostream>
#include "machineapp.h"

#include "instructions/arithmetic.h"

int main(int argc, char *argv[])
{
    MachineApp app(argc, argv);

    return app.exec();
}
