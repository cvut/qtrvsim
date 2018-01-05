#include "memorydock.h"

MemoryDock::MemoryDock(QWidget *parent) : QDockWidget(parent) {
    // TODO setup memory view

    setObjectName("Memory");
    setWindowTitle("Memory");
}

MemoryDock::~MemoryDock() {

}

void MemoryDock::setup(machine::QtMipsMachine *machine) {
    if (machine == nullptr)
        // TODO reset memory view
        return;

    // TODO setup memory view

}
