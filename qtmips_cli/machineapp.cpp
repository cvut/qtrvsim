#include "machineapp.h"
#include <iostream>

MachineApp::MachineApp(int argc, char **argv) : QCoreApplication(argc, argv) {
    connect(this, SIGNAL(aboutToQuit()), this, SLOT(quit()));
    // TODO drop hello
    std::cout << "Hello\n";
    std::cout.flush();
    // TODO check argc
    this->machine = new QtMipsMachine(argv[1]);
}
