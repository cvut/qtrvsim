#include "memorydock.h"

DataView::DataView(QWidget *parent, QSettings *settings) : MemoryView(parent, settings->value("DataViewAddr0", 0).toULongLong()) {
    this->settings = settings;
}

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

void DataView::addr0_save_change(std::uint32_t val) {
    settings->setValue("DataViewAddr0", val);
}

MemoryDock::MemoryDock(QWidget *parent, QSettings *settings) : QDockWidget(parent) {
    view = new DataView(this, settings);
    setWidget(view);

    setObjectName("Memory");
    setWindowTitle("Memory");
}

void MemoryDock::setup(machine::QtMipsMachine *machine) {
    view->setup(machine);
}
