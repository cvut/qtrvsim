#include "predictor_bht_dock.h"

LOG_CATEGORY("gui.DockPredictorBHT");

DockPredictorBHT::DockPredictorBHT(QWidget *parent) : Super(parent) {
    setObjectName("PredictorBHT");
    setWindowTitle("Predictor Branch History");

    /////////////////////////
    // Assign layout

    // Name
    layout_type->addWidget(label_type);
    layout_type->addWidget(label_type_value);

    // Stats
    layout_stats->addWidget(label_stats_correct_text, 0, 0);
    layout_stats->addWidget(label_stats_correct_value, 0, 1);
    layout_stats->addWidget(label_stats_wrong_text, 1, 0);
    layout_stats->addWidget(label_stats_wrong_value, 1, 1);
    layout_stats->addWidget(label_stats_accuracy_text, 2, 0);
    layout_stats->addWidget(label_stats_accuracy_value, 2, 1);

    // Main layout
    layout_main->addLayout(layout_type);
    layout_main->addLayout(layout_stats);
    layout_main->addLayout(layout_event);
    layout_main->addWidget(bht);

    content->setLayout(layout_main);
    setWidget(content);

    /////////////////////////
    // Init widget properties

    // Name
    label_type->setText("Predictor type:");
    label_type->setStyleSheet("font-weight: bold;");
    clear_name();

    // Stats
    label_stats_correct_text->setText("Correct predictions:");
    label_stats_wrong_text->setText("Wrong predictions:");
    label_stats_accuracy_text->setText("Accuracy:");
    clear_stats();

    // BHT
    bht->setRowCount(0);
    bht->setColumnCount(5);
    bht->setHorizontalHeaderLabels({ "Index", "State", "Correct", "Incorrect", "Accuracy" });
    bht->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bht->verticalHeader()->hide();
    bht->resizeRowsToContents();
}

// Get BHT cell item, or create new one if needed
QTableWidgetItem* DockPredictorBHT::get_bht_cell_item(uint8_t row_index, uint8_t col_index) {
    QTableWidgetItem *item { bht->item(row_index, col_index) };
    if (item == nullptr) {
        item = new QTableWidgetItem();
        bht->setItem(row_index, col_index, item);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    return item;
}

void DockPredictorBHT::set_table_color(QColor color) {
    for (uint16_t row_index = 0; row_index < bht->rowCount(); row_index++) {
        for (uint16_t column_index = 0; column_index < bht->columnCount(); column_index++) {
            get_bht_cell_item(row_index, column_index)->setBackground(
                QBrush(color));
        }
    }
}

void DockPredictorBHT::set_row_color(uint16_t row_index, QColor color) {
    for (uint16_t column_index = 0; column_index < bht->columnCount(); column_index++) {
        get_bht_cell_item(row_index, column_index)->setBackground(
            QBrush(color));
    }
}

void DockPredictorBHT::setup(
    const machine::BranchPredictor *branch_predictor,
    const machine::Core *core) {

    clear();

    number_of_bhr_bits = branch_predictor->get_number_of_bhr_bits();
    number_of_bht_bits = branch_predictor->get_number_of_bht_bits();
    initial_state = branch_predictor->get_initial_state();
    const machine::PredictorType predictor_type { branch_predictor->get_predictor_type() };
    const bool is_predictor_dynamic { machine::is_predictor_type_dynamic(predictor_type) };
    const bool is_predictor_enabled { branch_predictor->get_enabled() };

    if (is_predictor_enabled) {
        content->setDisabled(false);
        label_type_value->setText(branch_predictor->get_predictor_name().toString());

        connect(
            branch_predictor, &machine::BranchPredictor::predictor_stats_updated,
            this, &DockPredictorBHT::update_predictor_stats);
        connect(
            branch_predictor, &machine::BranchPredictor::prediction_done,
            this, &DockPredictorBHT::show_new_prediction);
        connect(
            core, &machine::Core::step_started,
            this, &DockPredictorBHT::reset_colors);

        if (is_predictor_dynamic) {
            bht->setDisabled(false);
            bht->setRowCount(qPow(2, number_of_bht_bits));
            clear_bht(initial_state);

            connect(
                branch_predictor, &machine::BranchPredictor::update_done,
                this, &DockPredictorBHT::show_new_update);
            connect(
                branch_predictor, &machine::BranchPredictor::predictor_bht_row_updated,
                this, &DockPredictorBHT::update_bht_row);
        } else {
            bht->setDisabled(true);
            bht->setRowCount(0);
        }
    } else {
        content->setDisabled(true);
        label_type_value->setText("None");
    }
}

void DockPredictorBHT::show_new_prediction(
    uint16_t btb_index,
    uint16_t bht_index,
    machine::PredictionInput input,
    machine::BranchResult result,
    machine::BranchType branch_type) {
    UNUSED(btb_index);
    UNUSED(input);
    UNUSED(result);
    if (branch_type == machine::BranchType::BRANCH) {
        set_row_color(bht_index, Q_COLOR_PREDICT);
    }
}

void DockPredictorBHT::show_new_update(
    uint16_t btb_index,
    uint16_t bht_index,
    machine::PredictionFeedback feedback) {
    UNUSED(btb_index);
    if (feedback.branch_type == machine::BranchType::BRANCH) {
        set_row_color(bht_index, Q_COLOR_UPDATE);
    }
}

void DockPredictorBHT::update_predictor_stats(machine::PredictionStatistics stats) {
    label_stats_correct_value->setText(QString::number(stats.correct));
    label_stats_wrong_value->setText(QString::number(stats.wrong));
    label_stats_accuracy_value->setText(QString::number(stats.accuracy) + " %");
    if (stats.total > 0) {
        label_stats_accuracy_value->setText(QString::number(stats.accuracy) + " %");
    } else {
        label_stats_accuracy_value->setText("N/A");
    }
}

void DockPredictorBHT::update_bht_row(uint16_t row_index, machine::BranchHistoryTableEntry bht_entry) {
    if (row_index >= bht->rowCount()) {
        WARN("BHT dock update received invalid row index: %u", row_index);
        return;
    }

    for (uint16_t column_index = 0; column_index < bht->columnCount(); column_index++) {
        get_bht_cell_item(row_index, DOCK_BHT_COL_STATE)->setData(
            Qt::DisplayRole, machine::predictor_state_to_string(bht_entry.state, true).toString());

        get_bht_cell_item(row_index, DOCK_BHT_COL_CORRECT)->setData(
            Qt::DisplayRole, QString::number(bht_entry.stats.correct));

        get_bht_cell_item(row_index, DOCK_BHT_COL_INCORRECT)->setData(
            Qt::DisplayRole, QString::number(bht_entry.stats.wrong));

        if (bht_entry.stats.total > 0) {
            get_bht_cell_item(row_index, DOCK_BHT_COL_ACCURACY)->setData(
                Qt::DisplayRole, QString::number(bht_entry.stats.accuracy) + " %");
        } else {
            get_bht_cell_item(row_index, DOCK_BHT_COL_ACCURACY)->setData(
                Qt::DisplayRole, "N/A");
        }
        
    }
}

void DockPredictorBHT::reset_colors() {
    set_table_color(Q_COLOR_DEFAULT);
}

void DockPredictorBHT::clear_stats() {
    label_stats_correct_value->setText("0");
    label_stats_wrong_value->setText("0");
    label_stats_accuracy_value->setText("N/A");
}

void DockPredictorBHT::clear_name() {
    label_type_value->setText("");
}

void DockPredictorBHT::clear_bht(machine::PredictorState initial_state) {
    for (uint16_t row_index = 0; row_index < bht->rowCount(); row_index++) {
        get_bht_cell_item(row_index, DOCK_BHT_COL_INDEX)->setData(
            Qt::DisplayRole, QString::number(row_index));

        get_bht_cell_item(row_index, DOCK_BHT_COL_STATE)->setData(
            Qt::DisplayRole, machine::predictor_state_to_string(initial_state, true).toString());

        get_bht_cell_item(row_index, DOCK_BHT_COL_CORRECT)->setData(
            Qt::DisplayRole, QString::number(0));

        get_bht_cell_item(row_index, DOCK_BHT_COL_INCORRECT)->setData(
            Qt::DisplayRole, QString::number(0));

        get_bht_cell_item(row_index, DOCK_BHT_COL_ACCURACY)->setData(
            Qt::DisplayRole, QString("N/A"));
    }
    bht->resizeRowsToContents();
    set_table_color(Q_COLOR_DEFAULT);
}

void DockPredictorBHT::clear() {
    clear_name();
    clear_stats();
    clear_bht();
}
