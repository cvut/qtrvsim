#include "memorydock.h"

#include "memorymodel.h"
#include "memorytableview.h"
#include "ui/hexlineedit.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QWidget>

MemoryDock::MemoryDock(QWidget *parent, QSettings *settings) : Super(parent) {
    setObjectName("Memory");
    setWindowTitle("Memory");

    auto *content = new QWidget();

    auto *cell_size = new QComboBox();
    cell_size->addItem("Byte", MemoryModel::CELLSIZE_BYTE);
    cell_size->addItem("Half-word", MemoryModel::CELLSIZE_HWORD);
    cell_size->addItem("Word", MemoryModel::CELLSIZE_WORD);
    cell_size->setCurrentIndex(MemoryModel::CELLSIZE_WORD);

    auto *cached_access = new QComboBox();
    cached_access->addItem("Direct", 0);
    cached_access->addItem("Cached", 1);

    auto *memory_content = new MemoryTableView(nullptr, settings);
    // memory_content->setSizePolicy();
    auto *memory_model = new MemoryModel(this);
    memory_content->setModel(memory_model);
    memory_content->verticalHeader()->hide();
    // memory_content->setHorizontalHeader(memory_model->);

    auto *go_edit = new HexLineEdit(nullptr, 8, 16, "0x");

    vm_toggle_ = new QCheckBox(tr("Show virtual"));

    auto *layout_top = new QHBoxLayout;
    layout_top->addWidget(cell_size);
    layout_top->addWidget(cached_access);
    layout_top->addWidget(vm_toggle_);
    connect(vm_toggle_, &QCheckBox::toggled, this, [this](bool nowShowingVirtual) {
        auto *view = findChild<MemoryTableView *>();
        auto *model = findChild<MemoryModel *>();
        if (!view || !model) return;
        QModelIndex idx = view->currentIndex();
        int row = idx.isValid() ? idx.row() : 0;
        machine::Address addr;
        if (!model->get_row_address(addr, row)) return;
        machine::Address newAddr = addr;
        if (!nowShowingVirtual) {
            newAddr = machinePtr->virtualToPhysical(addr);
        } else {
            auto const *tlb = dynamic_cast<machine::TLB *>(machinePtr->data_frontend());
            machine::VirtualAddress va;
            if (tlb && tlb->reverse_lookup(addr, va)) {
                newAddr = machine::Address { va.get_raw() };
            }
        }
        model->setVirtualMode(nowShowingVirtual);
        view->focus_address(newAddr);
    });

    auto *layout = new QVBoxLayout;
    layout->addLayout(layout_top);
    layout->addWidget(memory_content);
    layout->addWidget(go_edit);

    content->setLayout(layout);

    setWidget(content);

    connect(this, &MemoryDock::machine_setup, memory_model, &MemoryModel::setup);
    connect(vm_toggle_, &QCheckBox::toggled, memory_model, &MemoryModel::setVirtualMode);
    connect(
        cell_size, QOverload<int>::of(&QComboBox::currentIndexChanged), memory_content,
        &MemoryTableView::set_cell_size);
    connect(
        memory_model, &QAbstractItemModel::layoutChanged, memory_content,
        &MemoryTableView::recompute_columns);
    connect(
        cached_access, QOverload<int>::of(&QComboBox::currentIndexChanged), memory_model,
        &MemoryModel::cached_access);
    connect(
        go_edit, &HexLineEdit::value_edit_finished, memory_content,
        [memory_content](uint32_t value) {
            memory_content->go_to_address(machine::Address(value));
        });
    connect(
        memory_content, &MemoryTableView::address_changed, go_edit,
        [go_edit](machine::Address addr) { go_edit->set_value(addr.get_raw()); });
    connect(this, &MemoryDock::focus_addr, memory_content, &MemoryTableView::focus_address);
    connect(
        memory_model, &MemoryModel::setup_done, memory_content,
        &MemoryTableView::recompute_columns);
}

void MemoryDock::setup(machine::Machine *machine) {
    machinePtr = machine;
    emit machine_setup(machine);

    bool vm_on = machine->config().get_vm_enabled();
    vm_toggle_->setVisible(vm_on);
    vm_toggle_->setChecked(vm_on);

    QTimer::singleShot(0, this, [this]() {
        if (!machinePtr) return;
        machine::Address pc = machinePtr->registers()->read_pc();
        if (machinePtr->config().get_vm_enabled() && vm_toggle_->isChecked()) {
            this->focus_addr(pc);
        } else if (machinePtr->config().get_vm_enabled()) {
            this->focus_addr(machinePtr->virtualToPhysical(pc));
        } else {
            this->focus_addr(pc);
        }
    });
}
