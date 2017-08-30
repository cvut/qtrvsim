#ifndef QTMIPSMACHINE_H
#define QTMIPSMACHINE_H

#include <QObject>

#include "qtmipsexception.h"
#include "programloader.h"
#include "core.h"
// TODO piplined core

class QtMipsMachine : QObject {
    Q_OBJECT
public:
    QtMipsMachine(char *file);

    // TODO handle speed
    void play();
    void pause();
    void step();
    void restart();
private:
    ProgramLoader *loader;
};

#endif // QTMIPSMACHINE_H
