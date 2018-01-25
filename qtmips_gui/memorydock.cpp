#include "memorydock.h"

DataView::DataView(QWidget *parent) : MemoryView(parent) { }

QList<QWidget*> DataView::row_widget(std::uint32_t address, QWidget *parent) {
    QList<QWidget*> widgs;
    QLabel *l;

    l = new QLabel(QString("0x%1").arg(address, 8, 16, QChar('0')), parent);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    widgs.append(l);

    l = new QLabel(parent);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    if (memory != nullptr) {
        l->setText(QString("0x%1").arg(memory->read_word(address), 8, 16, QChar('0')));
    }
    else
        l->setText("        "); // Just fill it in with some plain text so we don't have just addresses there
    widgs.append(l);

    return widgs;
}

MemoryDock::MemoryDock(QWidget *parent) : QDockWidget(parent) {
    view = new DataView(this);
    setWidget(view);

    setObjectName("Memory");
    setWindowTitle("Memory");
}

void MemoryDock::setup(machine::QtMipsMachine *machine) {
    view->setup(machine);
}
