#include "predictor_btb_dock.h"

LOG_CATEGORY("gui.DockPredictorBTB");

DockPredictorBTB::DockPredictorBTB(QWidget *parent) : Super(parent) {
    setObjectName("PredictorBTB");
    setWindowTitle("Predictor Branch Target Buffer");

    btb = new QTableWidget();
    btb->setRowCount(0);
    btb->setColumnCount(3); // Index, Instruction Address, Target Address
    btb->setHorizontalHeaderLabels({ "Index", "Instruction Address", "Target Address" });
    btb->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    btb->resizeRowsToContents();
    btb->verticalHeader()->hide();
    init_table();

    layout = new QVBoxLayout();
    layout->addWidget(btb);

    content = new QWidget();
    content->setLayout(layout);
    setWidget(content);
};

DockPredictorBTB::~DockPredictorBTB() {
    delete btb;
    btb = nullptr;
    delete layout;
    layout = nullptr;
    delete content;
    content = nullptr;
};

uint8_t DockPredictorBTB::init_number_of_bits(const uint8_t b) const {
    if (b > BP_MAX_BTB_BITS) {
        WARN("Number of BTB bits (%u) was larger than %u during init", b, BP_MAX_BTB_BITS);
        return BP_MAX_BTB_BITS;
    }
    return b;
};

void DockPredictorBTB::init_table() {
    for (uint16_t row_index = 0; row_index < btb->rowCount(); row_index++) {
        for (uint16_t column_index = 0; column_index < btb->columnCount(); column_index++) {
            // Get cell item, or create new one if needed
            QTableWidgetItem *item { btb->item(row_index, column_index) };
            if (item == nullptr) {
                item = new QTableWidgetItem();
                btb->setItem(row_index, column_index, item);
            }

            // Init cell
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            if (column_index == DOCK_BTB_COL_INDEX) {
                item->setData(Qt::DisplayRole, QString::number(row_index));
            } else if (column_index == DOCK_BTB_COL_INSTR_ADDR) {
                item->setData(Qt::DisplayRole, QString(""));
            } else if (column_index == DOCK_BTB_COL_TARGET_ADDR) {
                item->setData(Qt::DisplayRole, QString(""));
            }
        }
    }
};

void DockPredictorBTB::set_table_color(QColor color) {
    for (uint16_t row_index = 0; row_index < btb->rowCount(); row_index++) {
        for (uint16_t column_index = 0; column_index < btb->columnCount(); column_index++) {
            // Get cell item, or create new one if needed
            QTableWidgetItem *item { btb->item(row_index, column_index) };
            if (item == nullptr) {
                item = new QTableWidgetItem();
                btb->setItem(row_index, column_index, item);
            }

            // Set color
            item->setBackground(QBrush(color));
        }
    }
};

void DockPredictorBTB::set_row_color(uint16_t row_index, QColor color) {
    for (uint16_t column_index = 0; column_index < btb->columnCount(); column_index++) {
        // Get cell item, or create new one if needed
        QTableWidgetItem *item { btb->item(row_index, column_index) };
        if (item == nullptr) {
            item = new QTableWidgetItem();
            btb->setItem(row_index, column_index, item);
        }

        // Set color
        item->setBackground(QBrush(color));
    }
};

void DockPredictorBTB::setup(
    const machine::BranchPredictor *branch_predictor,
    const machine::Core *core) {
    connect(
        branch_predictor, &machine::BranchPredictor::update_btb_row_done, this,
        &DockPredictorBTB::update_row);
    connect(
        branch_predictor, &machine::BranchPredictor::requested_bht_target_address, this,
        &DockPredictorBTB::highligh_row_after_prediction);
    connect(core, &machine::Core::step_started, this, &DockPredictorBTB::reset_colors);

    number_of_bits = init_number_of_bits(branch_predictor->get_number_of_btb_bits());
    const bool is_predictor_enabled { branch_predictor->get_enabled() };

    if (is_predictor_enabled) {
        btb->setRowCount(qPow(2, number_of_bits));
        btb->setDisabled(false);
    } else {
        btb->setRowCount(0);
        btb->setDisabled(true);
    }
    init_table();
    btb->resizeRowsToContents();
    set_table_color(Q_COLOR_DEFAULT);
};

void DockPredictorBTB::update_row(
    uint16_t index,
    machine::Address instruction_address,
    machine::Address target_address) {
    if (index >= btb->rowCount()) {
        WARN("BTB dock update received invalid row index: %u", index);
        return;
    }

    set_row_color(index, Q_COLOR_UPDATE);

    QTableWidgetItem *item_index { btb->item(index, DOCK_BTB_COL_INDEX) };
    QTableWidgetItem *item_instr_addr { btb->item(index, DOCK_BTB_COL_INSTR_ADDR) };
    QTableWidgetItem *item_target_addr { btb->item(index, DOCK_BTB_COL_TARGET_ADDR) };

    if (item_index == nullptr) {
        item_index = new QTableWidgetItem();
        btb->setItem(index, DOCK_BTB_COL_INDEX, item_index);
    }

    if (item_instr_addr == nullptr) {
        item_instr_addr = new QTableWidgetItem();
        btb->setItem(index, DOCK_BTB_COL_INSTR_ADDR, item_instr_addr);
    }

    if (item_target_addr == nullptr) {
        item_target_addr = new QTableWidgetItem();
        btb->setItem(index, DOCK_BTB_COL_TARGET_ADDR, item_target_addr);
    }

    item_instr_addr->setData(Qt::DisplayRole, machine::addr_to_hex_str(instruction_address));
    item_target_addr->setData(Qt::DisplayRole, machine::addr_to_hex_str(target_address));
};

void DockPredictorBTB::highligh_row_after_prediction(uint16_t index) {
    set_row_color(index, Q_COLOR_PREDICT);
}

void DockPredictorBTB::reset_colors() {
    set_table_color(Q_COLOR_DEFAULT);
}