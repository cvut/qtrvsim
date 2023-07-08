#include "programdock.h"

#include "helper/async_modal.h"
#include "programmodel.h"
#include "programtableview.h"
#include "ui/hexlineedit.h"

#include <QComboBox>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

ProgramDock::ProgramDock(QWidget *parent, QSettings *settings) : Super(parent) {
    setObjectName("Program");
    setWindowTitle("Program");

    this->settings = settings;
    follow_source
        = (enum FollowSource)settings->value("ProgramViewFollowSource", FOLLOWSRC_FETCH).toInt();

    for (auto &i : follow_addr) {
        i = machine::Address::null();
    }

    auto *content = new QWidget();

    auto *follow_inst = new QComboBox();
    follow_inst->addItem("Follow none");
    follow_inst->addItem("Follow fetch");
    follow_inst->addItem("Follow decode");
    follow_inst->addItem("Follow execute");
    follow_inst->addItem("Follow memory");
    follow_inst->addItem("Follow writeback");
    follow_inst->setCurrentIndex((int)follow_source);

    auto *program_content = new ProgramTableView(nullptr, settings);
    // program_content->setSizePolicy();
    auto *program_model = new ProgramModel(this);
    program_content->setModel(program_model);
    program_content->verticalHeader()->hide();
    // program_content->setHorizontalHeader(program_model->);

    auto *go_edit = new HexLineEdit(nullptr, 8, 16, "0x");

    auto *layout = new QVBoxLayout;
    layout->addWidget(follow_inst);
    layout->addWidget(program_content);
    layout->addWidget(go_edit);

    content->setLayout(layout);

    setWidget(content);

    connect(this, &ProgramDock::machine_setup, program_model, &ProgramModel::setup);
    connect(
        go_edit, &HexLineEdit::value_edit_finished, program_content,
        [program_content](uint32_t value) {
            program_content->go_to_address(machine::Address(value));
        });
    connect(program_content, &ProgramTableView::address_changed, go_edit, &HexLineEdit::set_value);
    connect(this, &ProgramDock::jump_to_pc, program_content, &ProgramTableView::go_to_address);
    connect(
        follow_inst, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &ProgramDock::set_follow_inst);
    connect(this, &ProgramDock::focus_addr, program_content, &ProgramTableView::focus_address);
    connect(
        this, &ProgramDock::focus_addr_with_save, program_content,
        &ProgramTableView::focus_address_with_save);
    connect(
        program_content, &QAbstractItemView::doubleClicked, program_model,
        &ProgramModel::toggle_hw_break);
    connect(
        this, &ProgramDock::stage_addr_changed, program_model, &ProgramModel::update_stage_addr);
    connect(program_model, &ProgramModel::report_error, this, &ProgramDock::report_error);
    connect(this, &ProgramDock::request_update_all, program_model, &ProgramModel::update_all);
}

void ProgramDock::setup(machine::Machine *machine) {
    machine::Address pc;
    emit machine_setup(machine);
    if (machine == nullptr) { return; }
    pc = machine->registers()->read_pc();
    for (machine::Address &address : follow_addr) {
        address = pc;
    }
    update_follow_position();
}

void ProgramDock::set_follow_inst(int follow) {
    follow_source = (enum FollowSource)follow;
    settings->setValue("ProgramViewFollowSource", (int)follow_source);
    update_follow_position();
}

void ProgramDock::update_pipeline_addrs(const machine::CoreState &s) {
    const machine::Pipeline &p = s.pipeline;
    fetch_inst_addr(p.fetch.result.inst_addr);
    decode_inst_addr(p.decode.result.inst_addr);
    execute_inst_addr(p.execute.result.inst_addr);
    memory_inst_addr(p.memory.result.inst_addr);
    writeback_inst_addr(p.writeback.internal.inst_addr);
}

void ProgramDock::fetch_inst_addr(machine::Address addr) {
    if (addr != machine::STAGEADDR_NONE) { follow_addr[FOLLOWSRC_FETCH] = addr; }
    emit stage_addr_changed(ProgramModel::STAGEADDR_FETCH, addr);
    if (follow_source == FOLLOWSRC_FETCH) { update_follow_position(); }
}

void ProgramDock::decode_inst_addr(machine::Address addr) {
    if (addr != machine::STAGEADDR_NONE) { follow_addr[FOLLOWSRC_DECODE] = addr; }
    emit stage_addr_changed(ProgramModel::STAGEADDR_DECODE, addr);
    if (follow_source == FOLLOWSRC_DECODE) { update_follow_position(); }
}

void ProgramDock::execute_inst_addr(machine::Address addr) {
    if (addr != machine::STAGEADDR_NONE) { follow_addr[FOLLOWSRC_EXECUTE] = addr; }
    emit stage_addr_changed(ProgramModel::STAGEADDR_EXECUTE, addr);
    if (follow_source == FOLLOWSRC_EXECUTE) { update_follow_position(); }
}

void ProgramDock::memory_inst_addr(machine::Address addr) {
    if (addr != machine::STAGEADDR_NONE) { follow_addr[FOLLOWSRC_MEMORY] = addr; }
    emit stage_addr_changed(ProgramModel::STAGEADDR_MEMORY, addr);
    if (follow_source == FOLLOWSRC_MEMORY) { update_follow_position(); }
}

void ProgramDock::writeback_inst_addr(machine::Address addr) {
    if (addr != machine::STAGEADDR_NONE) { follow_addr[FOLLOWSRC_WRITEBACK] = addr; }
    emit stage_addr_changed(ProgramModel::STAGEADDR_WRITEBACK, addr);
    if (follow_source == FOLLOWSRC_WRITEBACK) { update_follow_position(); }
}

void ProgramDock::update_follow_position() {
    if (follow_source != FOLLOWSRC_NONE) { emit focus_addr(follow_addr[follow_source]); }
}

void ProgramDock::report_error(const QString &error) {
    showAsyncMessageBox(this, QMessageBox::Critical, "Simulator Error", error);
}
