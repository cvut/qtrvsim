#ifndef MEMORYDOCK_H
#define MEMORYDOCK_H

#include "machine/machine.h"
#include "machine/memory/address.h"

#include <QComboBox>
#include <QDockWidget>

class MemoryDock : public QDockWidget {
    Q_OBJECT

    using Super = QDockWidget;

public:
    MemoryDock(QWidget *parent, QSettings *settings);

    void setup(machine::Machine *machine);

signals:
    void machine_setup(machine::Machine *machine);
    void focus_addr(machine::Address);
    void set_address_digits(int digits);

public slots:
    void report_error(const QString &error);

private:
    machine::Machine *machinePtr;
};

#endif // MEMORYDOCK_H
