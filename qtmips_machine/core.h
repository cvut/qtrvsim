#ifndef CORE_H
#define CORE_H

#include <QObject>
#include "instruction.h"
#include "registers.h"
#include "memory.h"
#include "programloader.h"
#include "programmemory.h"

class Core : public QObject {
    Q_OBJECT
public:
    Core();

signals:

public slots:
};

#endif // CORE_H
