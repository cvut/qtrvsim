#include "csrdock.h"

#include "csr/controlstate.h"

CsrDock::CsrDock(QWidget *parent) : QDockWidget(parent) {
    scrollarea = new QScrollArea(this);
    scrollarea->setWidgetResizable(true);
    widg = new StaticTable(scrollarea);
    xlen = machine::Xlen::_32;

    for (size_t i = 0; i < machine::CSR::REGISTERS.size(); i++) {
        auto &desc = machine::CSR::REGISTERS.at(i);
        csr_view[i] = new QLabel(sizeHintText(), widg);
        csr_view[i]->setFixedSize(csr_view[i]->sizeHint());
        csr_view[i]->setText("");
        csr_view[i]->setTextInteractionFlags(Qt::TextSelectableByMouse);
        auto desc_label = new QLabel(QString(desc.name), widg);
        desc_label->setToolTip(
            (QString("%0 (0x%1)").arg(desc.description).arg(desc.address.data, 0, 16)));
        widg->addRow({ desc_label, csr_view[i] });
        csr_highlighted[i] = false;
    }
    scrollarea->setWidget(widg);

    setWidget(scrollarea);
    setObjectName("Control and Status Registers");
    setWindowTitle("Control and Status Registers");

    pal_normal = QPalette(csr_view[1]->palette());
    pal_updated = QPalette(csr_view[1]->palette());
    pal_read = QPalette(csr_view[1]->palette());
    pal_normal.setColor(QPalette::WindowText, QColor(0, 0, 0));
    pal_updated.setColor(QPalette::WindowText, QColor(240, 0, 0));
    pal_read.setColor(QPalette::WindowText, QColor(0, 0, 240));
    csr_highlighted_any = false;
}

const char *CsrDock::sizeHintText() {
    if (xlen == machine::Xlen::_64)
        return "0x0000000000000000";
    else
        return "0x00000000";
}

void CsrDock::setup(machine::Machine *machine) {
    if (machine == nullptr) {
        // Reset data
        for (auto &i : csr_view) {
            i->setText("");
        }
        return;
    }

    const machine::CSR::ControlState *controlst = machine->control_state();
    if (controlst == nullptr)
        return;

    // if xlen changes adjust space to show full value
    if (xlen != machine->config().get_simulated_xlen()) {
        xlen = machine->config().get_simulated_xlen();
        auto *dumy_data_label = new QLabel(sizeHintText(), widg);
        for (size_t i = 0; i < machine::CSR::REGISTERS.size(); i++) {
            csr_view[i]->setFixedSize(dumy_data_label->sizeHint());
        }
        delete dumy_data_label;
    }

    for (size_t i = 0; i < machine::CSR::REGISTERS.size(); i++) {
        labelVal(csr_view[i], controlst->read_internal(i).as_xlen(xlen));
    }

    connect(controlst, &machine::CSR::ControlState::write_signal, this, &CsrDock::csr_changed);
    connect(controlst, &machine::CSR::ControlState::read_signal, this, &CsrDock::csr_read);
    connect(machine, &machine::Machine::tick, this, &CsrDock::clear_highlights);
}

void CsrDock::csr_changed(size_t internal_reg_id, machine::RegisterValue val) {
    // FIXME assert takes literal
    SANITY_ASSERT(
        (uint)internal_reg_id < machine::CSR::REGISTERS.size(),
        QString("CsrDock received signal with invalid CSR register: ")
            + QString::number((uint)internal_reg_id));
    labelVal(csr_view[(uint)internal_reg_id], val.as_xlen(xlen));
    csr_view[internal_reg_id]->setPalette(pal_updated);
    csr_highlighted[internal_reg_id] = true;
    csr_highlighted_any = true;
}

void CsrDock::csr_read(size_t internal_reg_id, machine::RegisterValue val) {
    (void)val;
    // FIXME assert takes literal
    SANITY_ASSERT(
        (uint)internal_reg_id < machine::CSR::REGISTERS.size(),
        QString("CsrDock received signal with invalid CSR register: ")
            + QString::number((uint)internal_reg_id));
    if (!csr_highlighted[internal_reg_id]) { csr_view[internal_reg_id]->setPalette(pal_read); }
    csr_highlighted[internal_reg_id] = true;
    csr_highlighted_any = true;
}

void CsrDock::clear_highlights() {
    if (!csr_highlighted_any) { return; }
    for (size_t i = 0; i < machine::CSR::REGISTERS.size(); i++) {
        if (csr_highlighted[i]) {
            csr_view[i]->setPalette(pal_normal);
            csr_highlighted[i] = false;
        }
    }
    csr_highlighted_any = false;
}

void CsrDock::labelVal(QLabel *label, uint64_t value) {
    QString t = QString("0x") + QString::number(value, 16);
    label->setText(t);
}
