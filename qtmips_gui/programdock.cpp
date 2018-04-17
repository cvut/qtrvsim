#include "programdock.h"
#include "qtmipsexception.h"

ProgramView::ProgramView(QWidget *parent, QSettings *settings) : MemoryView(parent, settings->value("ProgramViewAddr0", 0x8001FF80).toULongLong()) {
    this->settings = settings;
    /*
    cb_single = new QComboBox(this);
    cb_single->addItems({
        "Don't follow",
        "Follow executing instruction"
    });
    cb_single->setCurrentIndex(1);
    cb_single->hide();
    layout->addWidget(cb_single);
    connect(cb_single, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_single_changed(int)));

    cb_pipelined = new QComboBox(this);
    cb_pipelined->addItems({
        "Don't follow",
        "Follow Instruction fetch stage",
        "Follow Instruction decode stage",
        "Follow Execution stage",
        "Follow Memory access stage",
        "Follow Registers write back stage",
    });
    cb_pipelined->hide();
    cb_pipelined->setCurrentIndex(1);
    layout->addWidget(cb_pipelined);
    connect(cb_pipelined, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_pipelined_changed(int)));
    */
}

void ProgramView::setup(machine::QtMipsMachine *machine) {
    MemoryView::setup(machine);
    if (machine == nullptr)
        return;

    /*
    bool pipelined = machine->config().pipelined();
    cb_single->setVisible(!pipelined);
    cb_pipelined->setVisible(pipelined);
    // Sync selection somewhat
    if (pipelined) {
        if (cb_single->currentIndex() == 0)
            cb_pipelined->setCurrentIndex(0);
        else if (cb_pipelined->currentIndex() == 0)
            cb_pipelined->setCurrentIndex(1);
    } else
        cb_single->setCurrentIndex(cb_pipelined->currentIndex() == 0 ? 0 : 1);
    // TODO connect to instructuion hooks
    */
}

void ProgramView::jump_to_pc(std::uint32_t addr) {
    set_focus(addr);
}

QList<QWidget*> ProgramView::row_widget(std::uint32_t address, QWidget *parent) {
    QList<QWidget*> widgs;
    QLabel *l;

    QFont f;
    f.setStyleHint(QFont::Monospace);

    l = new QLabel(" ", parent);
    l->setFont(f);
    widgs.append(l);

    l = new QLabel(QString("0x") + QString("%1").arg(address, 8, 16, QChar('0')).toUpper(), parent);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    l->setFont(f);
    widgs.append(l);

    l = new QLabel(parent);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    l->setFont(f);
    l->setMinimumWidth(60);
    if (memory != nullptr)
        l->setText(machine::Instruction(memory->read_word(address)).to_str());
    widgs.append(l);

    return widgs;
}

void ProgramView::addr0_save_change(std::uint32_t val) {
    settings->setValue("ProgramViewAddr0", val);
}

void ProgramView::cb_single_changed(int index) {
    // TODO set memory view
}

void ProgramView::cb_pipelined_changed(int index) {
    // TODO set memory view
}

ProgramDock::ProgramDock(QWidget *parent, QSettings *settings) : QDockWidget(parent) {
    view = new ProgramView(this, settings);
    setWidget(view);

    setObjectName("Program");
    setWindowTitle("Program");
}

void ProgramDock::setup(machine::QtMipsMachine *machine) {
    view->setup(machine);
}

void ProgramDock::jump_to_pc(std::uint32_t addr) {
    view->jump_to_pc(addr);
}
