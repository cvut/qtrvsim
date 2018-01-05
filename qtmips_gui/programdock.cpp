#include "programdock.h"
#include "qtmipsexception.h"

ProgramDock::ProgramDock(QWidget *parent) : QDockWidget(parent) {
    widg = new QWidget(this);
    widg_layout = new QBoxLayout(QBoxLayout::TopToBottom, widg);
    widg_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    // TODO memory view

    /*
    ctlline = new QLineEdit(widg);
    ctlline->setText("0x00000000");
    ctlline->setInputMask("\\0\\xHHHHHHHH");
    ctlline->hide();
    widg_layout->addWidget(ctlline);
    connect(ctlline, SIGNAL(returnPressed()), this, SLOT(ctlline_returnPress()));
    */

    ctlbox_single = new QComboBox(widg);
    ctlbox_single->addItems({
        "Don't follow",
        "Follow executing instruction"
    });
    ctlbox_single->setCurrentIndex(1);
    ctlbox_single->hide();
    widg_layout->addWidget(ctlbox_single);
    connect(ctlbox_single, SIGNAL(currentIndexChanged(int)), this, SLOT(ctlbox_single_changed(int)));

    ctlbox_pipelined = new QComboBox(widg);
    ctlbox_pipelined->addItems({
        "Don't follow",
        "Follow Instruction fetch stage",
        "Follow Instruction decode stage",
        "Follow Execution stage",
        "Follow Memory access stage",
        "Follow Registers write back stage",
    });
    ctlbox_pipelined->hide();
    ctlbox_pipelined->setCurrentIndex(1);
    widg_layout->addWidget(ctlbox_pipelined);
    connect(ctlbox_pipelined, SIGNAL(currentIndexChanged(int)), this, SLOT(ctlbox_pipelined_changed(int)));

    setWidget(widg);
    setObjectName("Program");
    setWindowTitle("Program");
}

ProgramDock::~ProgramDock() {
    delete ctlbox_single;
    delete ctlbox_pipelined;
    delete widg;
    delete widg_layout;
}

void ProgramDock::setup(machine::QtMipsMachine *machine) {
    if (machine == nullptr) {
        // TODO zero memory viewer
        return;
    }

    // TODO pass to viewer

    bool pipelined = machine->config().pipelined();
    ctlbox_single->setVisible(!pipelined);
    ctlbox_pipelined->setVisible(pipelined);
    // Sync selection somewhat
    if (pipelined) {
        if (ctlbox_single->currentIndex() == 0)
            ctlbox_pipelined->setCurrentIndex(0);
        else if (ctlbox_pipelined->currentIndex() == 0)
            ctlbox_pipelined->setCurrentIndex(1);
    } else
        ctlbox_single->setCurrentIndex(ctlbox_pipelined->currentIndex() == 0 ? 0 : 1);

    // TODO also update current setting of memory viewer
}

void ProgramDock::ctlbox_single_changed(int index) {
    // TODO set memory view
}

void ProgramDock::ctlbox_pipelined_changed(int index) {
    // TODO set memory view
}
