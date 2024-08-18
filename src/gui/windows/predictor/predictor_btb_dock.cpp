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
}

DockPredictorBTB::~DockPredictorBTB() {
    delete btb;
    btb = nullptr;
    delete layout;
    layout = nullptr;
    delete content;
    content = nullptr;
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

void DockPredictorBTB::init_table() {
    for (uint16_t row_index = 0; row_index < btb->rowCount(); row_index++) {
        QTableWidgetItem *item;

        item = get_btb_cell_item(row_index, DOCK_BTB_COL_INDEX);
        item->setData(Qt::DisplayRole, QString::number(row_index));

        item = get_btb_cell_item(row_index, DOCK_BTB_COL_INSTR_ADDR);
        item->setData(Qt::DisplayRole, QString(""));

        item = get_btb_cell_item(row_index, DOCK_BTB_COL_TARGET_ADDR);
        item->setData(Qt::DisplayRole, QString(""));
    }
}

void DockPredictorBTB::set_table_color(QColor color) {
    for (uint16_t row_index = 0; row_index < btb->rowCount(); row_index++) {
        for (uint16_t column_index = 0; column_index < btb->columnCount(); column_index++) {
            QTableWidgetItem *item { get_btb_cell_item(row_index, column_index) };
            item->setBackground(QBrush(color));
        }
    }
}

void DockPredictorBTB::set_row_color(uint16_t row_index, QColor color) {
    for (uint16_t column_index = 0; column_index < btb->columnCount(); column_index++) {
        QTableWidgetItem *item { get_btb_cell_item(row_index, column_index) };
        item->setBackground(QBrush(color));
    }
}

void DockPredictorBTB::setup(
    const machine::BranchPredictor *branch_predictor,
    const machine::Core *core) {
    connect(
        branch_predictor, &machine::BranchPredictor::btb_row_updated,
        this, &DockPredictorBTB::update_btb_row);
    connect(
        branch_predictor, &machine::BranchPredictor::btb_target_address_requested,
        this, &DockPredictorBTB::highligh_row_after_prediction);
    connect(
        branch_predictor, &machine::BranchPredictor::cleared,
        this, &DockPredictorBTB::clear);
    connect(
        core, &machine::Core::step_started,
        this, &DockPredictorBTB::reset_colors);

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
}

void DockPredictorBTB::update_btb_row(
    uint16_t row_index,
    machine::BranchTargetBufferEntry btb_entry
) {
    if (row_index >= btb->rowCount()) {
        WARN("BTB dock update received invalid row index: %u", row_index);
        return;
    }

    QTableWidgetItem *item;

    item = get_btb_cell_item(row_index, DOCK_BTB_COL_INSTR_ADDR);
    item->setData(Qt::DisplayRole, machine::addr_to_hex_str(btb_entry.instruction_address));

    item = get_btb_cell_item(row_index, DOCK_BTB_COL_TARGET_ADDR);
    item->setData(Qt::DisplayRole, machine::addr_to_hex_str(btb_entry.target_address));

    set_row_color(row_index, Q_COLOR_UPDATE);
}

void DockPredictorBTB::highligh_row_after_prediction(uint16_t index) {
    set_row_color(index, Q_COLOR_PREDICT);
}

void DockPredictorBTB::reset_colors() {
    set_table_color(Q_COLOR_DEFAULT);
}

void DockPredictorBTB::clear() {
    reset_colors();
    init_table();
}