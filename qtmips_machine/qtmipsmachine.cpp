#include "qtmipsmachine.h"

QtMipsMachine::QtMipsMachine(char *file) {
    this->loader = new ProgramLoader(file);
}
