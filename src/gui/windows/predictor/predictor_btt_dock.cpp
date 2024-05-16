#include "predictor_btt_dock.h"

LOG_CATEGORY("gui.DockPredictorBTT");

DockPredictorBTT::DockPredictorBTT(QWidget *parent) : Super(parent) {
    setObjectName("PredictorBTT");
    setWindowTitle("Predictor Branch Target Table");

    btt = new QTableWidget();
    btt->setRowCount(0);
    btt->setColumnCount(3); // Index, Instruction Address, Target Address
    btt->setHorizontalHeaderLabels({ "Index", "Instruction Address", "Target Address" });
    btt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    btt->resizeRowsToContents();
    btt->verticalHeader()->hide();
    init_table();

    layout = new QVBoxLayout();
    layout->addWidget(btt);

    content = new QWidget();
    content->setLayout(layout);
    setWidget(content);
};

DockPredictorBTT::~DockPredictorBTT() {
    delete btt;
    btt = nullptr;
    delete layout;
    layout = nullptr;
    delete content;
    content = nullptr;
};

uint8_t DockPredictorBTT::init_number_of_bits(const uint8_t number_of_bits) const {
    if (number_of_bits > PREDICTOR_MAX_TABLE_BITS) {
        WARN(
            "Number of BTT bits (%u) was larger than %u during init", number_of_bits,
            PREDICTOR_MAX_TABLE_BITS);
        return PREDICTOR_MAX_TABLE_BITS;
    }
    return number_of_bits;
};

void DockPredictorBTT::init_table() {
    for (uint16_t row_index = 0; row_index < btt->rowCount(); row_index++) {
        for (uint16_t column_index = 0; column_index < btt->columnCount(); column_index++) {
            // Get cell item, or create new one if needed
            QTableWidgetItem *item { btt->item(row_index, column_index) };
            if (item == nullptr) {
                item = new QTableWidgetItem();
                btt->setItem(row_index, column_index, item);
            }

            // Init cell
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            if (column_index == DOCK_BTT_COL_INDEX) {
                item->setData(Qt::DisplayRole, QString::number(row_index));
            } else if (column_index == DOCK_BTT_COL_INSTR_ADDR) {
                item->setData(Qt::DisplayRole, QString(""));
            } else if (column_index == DOCK_BTT_COL_TARGET_ADDR) {
                item->setData(Qt::DisplayRole, QString(""));
            }
        }
    }
};

void DockPredictorBTT::set_table_color(QColor color) {
    for (uint16_t row_index = 0; row_index < btt->rowCount(); row_index++) {
        for (uint16_t column_index = 0; column_index < btt->columnCount(); column_index++) {
            // Get cell item, or create new one if needed
            QTableWidgetItem *item { btt->item(row_index, column_index) };
            if (item == nullptr) {
                item = new QTableWidgetItem();
                btt->setItem(row_index, column_index, item);
            }

            // Set color
            item->setBackground(QBrush(color));
        }
    }
};

void DockPredictorBTT::set_row_color(uint16_t row_index, QColor color) {
    for (uint16_t column_index = 0; column_index < btt->columnCount(); column_index++) {
        // Get cell item, or create new one if needed
        QTableWidgetItem *item { btt->item(row_index, column_index) };
        if (item == nullptr) {
            item = new QTableWidgetItem();
            btt->setItem(row_index, column_index, item);
        }

        // Set color
        item->setBackground(QBrush(color));
    }
};

void DockPredictorBTT::setup(
    const machine::BranchPredictor *branch_predictor,
    const machine::Core *core) {
    connect(
        branch_predictor, &machine::BranchPredictor::updated_btt_row, this,
        &DockPredictorBTT::update_row);
    connect(
        branch_predictor, &machine::BranchPredictor::new_prediction, this,
        &DockPredictorBTT::update_new_prediction);
    connect(core, &machine::Core::step_started, this, &DockPredictorBTT::reset_colors);

    number_of_bits = init_number_of_bits(branch_predictor->get_number_of_table_bits());
    const bool is_predictor_enabled { branch_predictor->get_enabled() };

    if (is_predictor_enabled) {
        btt->setRowCount(qPow(2, number_of_bits));
        btt->setDisabled(false);
    } else {
        btt->setRowCount(0);
        btt->setDisabled(true);
    }
    init_table();
    btt->resizeRowsToContents();
    set_table_color(Q_COLOR_DEFAULT);
};

void DockPredictorBTT::update_row(
    uint16_t index,
    machine::Address instruction_address,
    machine::Address target_address) {
    if (index >= btt->rowCount()) {
        WARN("BTT dock update received invalid row index: %u", index);
        return;
    }

    set_row_color(index, Q_COLOR_UPDATE);

    QTableWidgetItem *item_index { btt->item(index, DOCK_BTT_COL_INDEX) };
    QTableWidgetItem *item_instr_addr { btt->item(index, DOCK_BTT_COL_INSTR_ADDR) };
    QTableWidgetItem *item_target_addr { btt->item(index, DOCK_BTT_COL_TARGET_ADDR) };

    if (item_index == nullptr) {
        item_index = new QTableWidgetItem();
        btt->setItem(index, DOCK_BTT_COL_INDEX, item_index);
    }

    if (item_instr_addr == nullptr) {
        item_instr_addr = new QTableWidgetItem();
        btt->setItem(index, DOCK_BTT_COL_INSTR_ADDR, item_instr_addr);
    }

    if (item_target_addr == nullptr) {
        item_target_addr = new QTableWidgetItem();
        btt->setItem(index, DOCK_BTT_COL_TARGET_ADDR, item_target_addr);
    }

    item_instr_addr->setData(Qt::DisplayRole, machine::addr_to_hex_str(instruction_address));
    item_target_addr->setData(Qt::DisplayRole, machine::addr_to_hex_str(target_address));
};

void DockPredictorBTT::update_new_prediction(
    machine::PredictionInput input,
    machine::BranchResult result) {
    UNUSED(result);
    set_row_color(input.bht_index, Q_COLOR_PREDICT);
}

void DockPredictorBTT::reset_colors() {
    set_table_color(Q_COLOR_DEFAULT);
}