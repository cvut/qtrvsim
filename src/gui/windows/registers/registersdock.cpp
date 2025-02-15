#include "registersdock.h"

#include "machine/instruction.h"

RegistersDock::RegistersDock(QWidget *parent, machine::Xlen xlen)
    : QDockWidget(parent)
    , xlen(xlen)
    , scroll_area(new QScrollArea(this))
    , table_widget(new StaticTable(scroll_area.data()))
    , pal_normal(createPalette(QColor(0, 0, 0)))
    , pal_updated(createPalette(QColor(240, 0, 0)))
    , pal_read(createPalette(QColor(0, 0, 240))) {
    scroll_area->setWidgetResizable(true);
    gp_highlighted.reset();

    for (size_t i = 0; i < gp.size(); i++) {
        gp[i] = addRegisterLabel(QString("x%1/%2").arg(i).arg(machine::Rv_regnames[i]));
    }
    pc = addRegisterLabel("pc");

    scroll_area->setWidget(table_widget.data());
    setWidget(scroll_area.data());

    setObjectName("Registers");
    setWindowTitle("Registers");
}

const char *RegistersDock::sizeHintText() {
    if (xlen == machine::Xlen::_64)
        return "0x0000000000000000";
    else
        return "0x00000000";
}

QLabel *RegistersDock::addRegisterLabel(const QString &title) {
    auto *data_label = new QLabel(sizeHintText(), table_widget.data());
    data_label->setTextFormat(Qt::PlainText);
    data_label->setFixedSize(data_label->sizeHint());
    data_label->setText("");
    data_label->setPalette(pal_normal);
    data_label->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto *title_label = new QLabel(title, table_widget.data());
    title_label->setPalette(pal_normal);

    // Add row take ownership of the labels.
    table_widget->addRow({ OWNED title_label, OWNED data_label });

    return BORROWED data_label;
}

void RegistersDock::connectToMachine(machine::Machine *machine) {
    if (machine == nullptr) {
        // Reset data
        pc->setText("");
        for (auto &i : gp) {
            i->setText("");
        }
        return;
    }

    regs_handle = machine->registers();

    // if xlen changes adjust space to show full value
    if (xlen != machine->config().get_simulated_xlen()) {
        xlen = machine->config().get_simulated_xlen();
        auto *dumy_data_label = new QLabel(sizeHintText(), table_widget.data());
        for (auto &i : gp) {
            i->setFixedSize(dumy_data_label->sizeHint());
        }
        pc->setFixedSize(dumy_data_label->sizeHint());
        delete dumy_data_label;
    }

    if (regs_handle == nullptr) { return; }

    reload();

    connect(regs_handle, &machine::Registers::pc_update, this, &RegistersDock::pc_changed);
    connect(regs_handle, &machine::Registers::gp_update, this, &RegistersDock::gp_changed);
    connect(regs_handle, &machine::Registers::gp_read, this, &RegistersDock::gp_read);
    connect(machine, &machine::Machine::tick, this, &RegistersDock::clear_highlights);
}

void RegistersDock::showEvent(QShowEvent *event) {
    reload();
    QDockWidget::showEvent(event);
}

void RegistersDock::pc_changed(machine::Address val) {
    if (isHidden()) { return; }
    setRegisterValueToLabel(pc, val.get_raw());
}

void RegistersDock::gp_changed(machine::RegisterId i, machine::RegisterValue val) {
    if (isHidden()) { return; }

    setRegisterValueToLabel(gp[i], val);
    gp[i]->setPalette(pal_updated);
    gp_highlighted[i] = true;
}

void RegistersDock::gp_read(machine::RegisterId i, machine::RegisterValue val) {
    Q_UNUSED(val)

    if (isHidden()) { return; }

    if (!(gp_highlighted[i])) {
        gp[i]->setPalette(pal_read);
        gp_highlighted[i] = true;
    }
}

void RegistersDock::clear_highlights() {
    if (gp_highlighted.any()) {
        for (size_t i = 0; i < gp.size(); i++) {
            if (gp_highlighted[i]) { gp[i]->setPalette(pal_normal); }
        }
    }
    gp_highlighted.reset();
}
void RegistersDock::reload() {
    if (regs_handle == nullptr) { return; }
    setRegisterValueToLabel(pc, regs_handle->read_pc().get_raw());
    for (size_t i = 0; i < gp.size(); i++) {
        setRegisterValueToLabel(gp[i], regs_handle->read_gp_internal(i));
    }
    clear_highlights();
}

void RegistersDock::setRegisterValueToLabel(QLabel *label, machine::RegisterValue value) {
    label->setText(QString("0x%1").arg(value.as_xlen(xlen), 0, 16));
}

QPalette RegistersDock::createPalette(const QColor &color) const {
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, color);
    return palette;
}
