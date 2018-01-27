#include "memorydock.h"

DataView::DataView(QWidget *parent) : MemoryView(parent) { }

QList<QWidget*> DataView::row_widget(std::uint32_t address, QWidget *parent) {
    QList<QWidget*> widgs;
    QLabel *l;

    QFont f;
    f.setStyleHint(QFont::Monospace);

    l = new QLabel(QString("0x") + QString("%1").arg(address, 8, 16, QChar('0')).toUpper(), parent);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    l->setFont(f);
    widgs.append(l);

    l = new QLabel(parent);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    l->setFont(f);
    if (memory != nullptr)
        l->setText(QString("0x") + QString("%1").arg(memory->read_word(address), 8, 16, QChar('0')).toUpper());
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
