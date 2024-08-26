#include "predictor_btb_dock.h"

LOG_CATEGORY("gui.DockPredictorBTB");

DockPredictorBTB::DockPredictorBTB(QWidget *parent) : Super(parent) {
    setObjectName("PredictorBTB");
    setWindowTitle("Predictor Branch Target Buffer");

    /////////////////////////
    // Assign layout

    layout->addWidget(btb);
    content->setLayout(layout);
    setWidget(content);

    /////////////////////////
    // Init widget properties

    // BTB
    btb->setRowCount(0);
    btb->setColumnCount(4);
    btb->setHorizontalHeaderLabels({ "Index", "Instr. Address", "Target Address", "Type" });
    btb->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    btb->verticalHeader()->hide();
    btb->resizeRowsToContents();
}

uint8_t DockPredictorBTB::init_number_of_bits(const uint8_t b) const {
    if (b > BP_MAX_BTB_BITS) {
        WARN("Number of BTB bits (%u) was larger than %u during init", b, BP_MAX_BTB_BITS);
        return BP_MAX_BTB_BITS;
    }
    return b;
}

// Get BTB cell item, or create new one if needed
QTableWidgetItem* DockPredictorBTB::get_btb_cell_item(uint8_t row_index, uint8_t col_index) {
    QTableWidgetItem *item { btb->item(row_index, col_index) };
    if (item == nullptr) {
        item = new QTableWidgetItem();
        btb->setItem(row_index, col_index, item);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    return item;
}

void DockPredictorBTB::set_table_color(QColor color) {
    for (uint16_t row_index = 0; row_index < btb->rowCount(); row_index++) {
        for (uint16_t column_index = 0; column_index < btb->columnCount(); column_index++) {
            get_btb_cell_item(row_index, column_index)->setBackground(
                QBrush(color));
        }
    }
}

void DockPredictorBTB::set_row_color(uint16_t row_index, QColor color) {
    for (uint16_t column_index = 0; column_index < btb->columnCount(); column_index++) {
        get_btb_cell_item(row_index, column_index)->setBackground(
            QBrush(color));
    }
}

void DockPredictorBTB::setup(
    const machine::BranchPredictor *branch_predictor,
    const machine::Core *core) {

    clear();

    number_of_bits = init_number_of_bits(branch_predictor->get_number_of_btb_bits());
    const bool is_predictor_enabled { branch_predictor->get_enabled() };

    if (is_predictor_enabled) {
        btb->setRowCount(qPow(2, number_of_bits));
        btb->setDisabled(false);
        clear_btb();

        connect(
            branch_predictor, &machine::BranchPredictor::btb_row_updated,
            this, &DockPredictorBTB::update_btb_row);
        connect(
            branch_predictor, &machine::BranchPredictor::prediction_done,
            this, &DockPredictorBTB::highligh_row_after_prediction);
        connect(
            branch_predictor, &machine::BranchPredictor::update_done,
            this, &DockPredictorBTB::highligh_row_after_update);
        connect(
            core, &machine::Core::step_started,
            this, &DockPredictorBTB::reset_colors);
        
    } else {
        btb->setRowCount(0);
        btb->setDisabled(true);
    }
}

void DockPredictorBTB::update_btb_row(
    uint16_t row_index,
    machine::BranchTargetBufferEntry btb_entry
) {
    if (row_index >= btb->rowCount()) {
        WARN("BTB dock update received invalid row index: %u", row_index);
        return;
    }

    if (btb_entry.entry_valid) {
        get_btb_cell_item(row_index, DOCK_BTB_COL_INSTR_ADDR)->setData(
            Qt::DisplayRole, machine::addr_to_hex_str(btb_entry.instruction_address));

        get_btb_cell_item(row_index, DOCK_BTB_COL_TARGET_ADDR)->setData(
            Qt::DisplayRole, machine::addr_to_hex_str(btb_entry.target_address));

        get_btb_cell_item(row_index, DOCK_BTB_COL_TYPE)->setData(
            Qt::DisplayRole, machine::branch_type_to_string(btb_entry.branch_type).toString());
    } else {
        get_btb_cell_item(row_index, DOCK_BTB_COL_INSTR_ADDR)->setData(
            Qt::DisplayRole, "");

        get_btb_cell_item(row_index, DOCK_BTB_COL_TARGET_ADDR)->setData(
            Qt::DisplayRole, "");

        get_btb_cell_item(row_index, DOCK_BTB_COL_TYPE)->setData(
            Qt::DisplayRole, "");
    }
}

void DockPredictorBTB::highligh_row_after_prediction(uint16_t row_index) {
    set_row_color(row_index, Q_COLOR_PREDICT);
}

void DockPredictorBTB::highligh_row_after_update(uint16_t row_index) {
    set_row_color(row_index, Q_COLOR_UPDATE);
}

void DockPredictorBTB::reset_colors() {
    set_table_color(Q_COLOR_DEFAULT);
}

void DockPredictorBTB::clear_btb() {
    for (uint16_t row_index = 0; row_index < btb->rowCount(); row_index++) {
        get_btb_cell_item(row_index, DOCK_BTB_COL_INDEX)->setData(
            Qt::DisplayRole, QString::number(row_index));

        get_btb_cell_item(row_index, DOCK_BTB_COL_INSTR_ADDR)->setData(
            Qt::DisplayRole, QString(""));

        get_btb_cell_item(row_index, DOCK_BTB_COL_TARGET_ADDR)->setData(
            Qt::DisplayRole, QString(""));

        get_btb_cell_item(row_index, DOCK_BTB_COL_TYPE)->setData(
            Qt::DisplayRole, QString(""));
    }
    btb->resizeRowsToContents();
    set_table_color(Q_COLOR_DEFAULT);
}

void DockPredictorBTB::clear() {
    clear_btb();
}