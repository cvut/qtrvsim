#ifndef MEMORYDOCK_H
#define MEMORYDOCK_H

#include <QDockWidget>
#include <QLabel>
#include <QComboBox>
#include "qtmipsmachine.h"
#include "memoryview.h"

class DataView : public MemoryView {
    Q_OBJECT
public:
    DataView(QWidget *parent);

protected:
    QList<QWidget*> row_widget(std::uint32_t address, QWidget *parent);

private:
    QComboBox *cb_size;
};

class MemoryDock : public QDockWidget  {
    Q_OBJECT
public:
    MemoryDock(QWidget *parent);

    void setup(machine::QtMipsMachine *machine);

private:
    DataView *view;
};

#endif // MEMORYDOCK_H
