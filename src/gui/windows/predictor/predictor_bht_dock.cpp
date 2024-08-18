#include "predictor_bht_dock.h"

LOG_CATEGORY("gui.DockPredictorBHT");

DockPredictorBHT::DockPredictorBHT(QWidget *parent) : Super(parent) {
    setObjectName("PredictorBHT");
    setWindowTitle("Predictor Branch History");

    /////////////////////////
    // Create widgets

    content = new QGroupBox();
    layout_main = new QVBoxLayout();

    // Name
    layout_type = new QHBoxLayout();
    label_type = new QLabel();
    label_type_value = new QLabel();

    // Stats
    layout_stats = new QGridLayout();
    label_stats_correct_text = new QLabel();
    label_stats_correct_value = new QLabel();
    label_stats_wrong_text = new QLabel();
    label_stats_wrong_value = new QLabel();
    label_stats_accuracy_text = new QLabel();
    label_stats_accuracy_value = new QLabel();

    // Prediction & Update
    layout_event = new QHBoxLayout();

    // Prediction
    group_event_predict = new QGroupBox();
    layout_event_predict = new QVBoxLayout();
    label_event_predict_header = new QLabel();
    label_event_predict_instruction = new QLabel();
    value_event_predict_instruction = new QLineEdit();
    label_event_predict_address = new QLabel();
    value_event_predict_address = new QLineEdit();
    label_event_predict_index = new QLabel();
    value_event_predict_index = new QLineEdit();
    label_event_predict_result = new QLabel();
    value_event_predict_result = new QLineEdit();

    // Update
    group_event_update = new QGroupBox();
    layout_event_update = new QVBoxLayout();
    label_event_update_header = new QLabel();
    label_event_update_instruction = new QLabel();
    value_event_update_instruction = new QLineEdit();
    label_event_update_address = new QLabel();
    value_event_update_address = new QLineEdit();
    label_event_update_index = new QLabel();
    value_event_update_index = new QLineEdit();
    label_event_update_result = new QLabel();
    value_event_update_result = new QLineEdit();

    // BHR
    layout_bhr = new QHBoxLayout();
    label_bhr = new QLabel();
    value_bhr = new QLineEdit();

    // BHT
    bht = new QTableWidget();

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

    // Prediction
    layout_event->addWidget(group_event_predict);
    group_event_predict->setLayout(layout_event_predict);
    layout_event_predict->addWidget(label_event_predict_header);
    layout_event_predict->addWidget(label_event_predict_instruction);
    layout_event_predict->addWidget(value_event_predict_instruction);
    layout_event_predict->addWidget(label_event_predict_address);
    layout_event_predict->addWidget(value_event_predict_address);
    layout_event_predict->addWidget(label_event_predict_index);
    layout_event_predict->addWidget(value_event_predict_index);
    layout_event_predict->addWidget(label_event_predict_result);
    layout_event_predict->addWidget(value_event_predict_result);

    // Update
    layout_event->addWidget(group_event_update);
    group_event_update->setLayout(layout_event_update);
    layout_event_update->addWidget(label_event_update_header);
    layout_event_update->addWidget(label_event_update_instruction);
    layout_event_update->addWidget(value_event_update_instruction);
    layout_event_update->addWidget(label_event_update_address);
    layout_event_update->addWidget(value_event_update_address);
    layout_event_update->addWidget(label_event_update_index);
    layout_event_update->addWidget(value_event_update_index);
    layout_event_update->addWidget(label_event_update_result);
    layout_event_update->addWidget(value_event_update_result);

    // BHR
    layout_bhr->addWidget(label_bhr);
    layout_bhr->addWidget(value_bhr);

    // Main layout
    layout_main->addLayout(layout_type);
    layout_main->addLayout(layout_stats);
    layout_main->addLayout(layout_event);
    layout_main->addLayout(layout_bhr);
    layout_main->addWidget(bht);

    content->setLayout(layout_main);
    setWidget(content);

    /////////////////////////
    // Init widget properties

    // Name
    label_type->setText("Predictor type:");
    label_type->setStyleSheet("font-weight: bold;");
    label_type_value->setText("");

    // Stats
    label_stats_correct_text->setText("Correct predictions:");
    label_stats_correct_value->setText("0");
    label_stats_wrong_text->setText("Wrong predictions:");
    label_stats_wrong_value->setText("0");
    label_stats_accuracy_text->setText("Accuracy:");
    label_stats_accuracy_value->setText("100 %");

    // Prediction
    label_event_predict_header->setText("Last prediction");
    label_event_predict_header->setStyleSheet("font-weight: bold;");
    label_event_predict_instruction->setText("Instruction:");
    label_event_predict_address->setText("Instruction Address:");
    label_event_predict_index->setText("Computed index:");
    label_event_predict_result->setText("Prediction result:");
    value_event_predict_instruction->setReadOnly(true);
    value_event_predict_address->setReadOnly(true);
    value_event_predict_index->setReadOnly(true);
    value_event_predict_result->setReadOnly(true);
    value_event_predict_instruction->setAlignment(Qt::AlignCenter);
    value_event_predict_address->setAlignment(Qt::AlignCenter);
    value_event_predict_index->setAlignment(Qt::AlignCenter);
    value_event_predict_result->setAlignment(Qt::AlignCenter);

    // Update
    label_event_update_header->setText("Last update");
    label_event_update_header->setStyleSheet("font-weight: bold;");
    label_event_update_instruction->setText("Instruction:");
    label_event_update_address->setText("Instruction Address:");
    label_event_update_index->setText("Computed index:");
    label_event_update_result->setText("Branch result:");
    value_event_update_instruction->setReadOnly(true);
    value_event_update_address->setReadOnly(true);
    value_event_update_index->setReadOnly(true);
    value_event_update_result->setReadOnly(true);
    value_event_update_instruction->setAlignment(Qt::AlignCenter);
    value_event_update_address->setAlignment(Qt::AlignCenter);
    value_event_update_index->setAlignment(Qt::AlignCenter);
    value_event_update_result->setAlignment(Qt::AlignCenter);

    // BHR
    label_bhr->setText("Branch History Register:");
    value_bhr->setReadOnly(true);
    value_bhr->setAlignment(Qt::AlignCenter);
    value_bhr->setText("");
    value_bhr->setFixedWidth(120);
    // TODO set tooltips
    // value_bhr->setToolTip("TEST");

    // BHT
    bht->setRowCount(0);
    bht->setColumnCount(5); // Index, History, Correct, Incorrect, Accuracy
    bht->setHorizontalHeaderLabels({ "Index", "State", "Correct", "Incorrect", "Accuracy" });
    bht->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bht->resizeRowsToContents();
    bht->verticalHeader()->hide();
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

void DockPredictorBHT::init_table(machine::PredictorState initial_state) {
    for (uint16_t row_index = 0; row_index < bht->rowCount(); row_index++) {
        QTableWidgetItem *item;

        item = get_bht_cell_item(row_index, DOCK_BHT_COL_INDEX);
        item->setData(Qt::DisplayRole, QString::number(row_index));

        item = get_bht_cell_item(row_index, DOCK_BHT_COL_STATE);
        item->setData(Qt::DisplayRole, machine::predictor_state_to_string(initial_state, true).toString());

        item = get_bht_cell_item(row_index, DOCK_BHT_COL_CORRECT);
        item->setData(Qt::DisplayRole, QString::number(0));

        item = get_bht_cell_item(row_index, DOCK_BHT_COL_INCORRECT);
        item->setData(Qt::DisplayRole, QString::number(0));

        item = get_bht_cell_item(row_index, DOCK_BHT_COL_ACCURACY);
        item->setData(Qt::DisplayRole, QString("100 %"));
    }
}

void DockPredictorBHT::set_table_color(QColor color) {
    for (uint16_t row_index = 0; row_index < bht->rowCount(); row_index++) {
        for (uint16_t column_index = 0; column_index < bht->columnCount(); column_index++) {
            QTableWidgetItem *item { get_bht_cell_item(row_index, column_index) };
            item->setBackground(QBrush(color));
        }
    }
}

void DockPredictorBHT::set_row_color(uint16_t row_index, QColor color) {
    for (uint16_t column_index = 0; column_index < bht->columnCount(); column_index++) {
        QTableWidgetItem *item { get_bht_cell_item(row_index, column_index) };
        item->setBackground(QBrush(color));
    }
}

void DockPredictorBHT::set_predict_widget_color(QString color_stylesheet) {
    value_event_predict_instruction->setStyleSheet(color_stylesheet);
    value_event_predict_address->setStyleSheet(color_stylesheet);
    value_event_predict_index->setStyleSheet(color_stylesheet);
    value_event_predict_result->setStyleSheet(color_stylesheet);
}

void DockPredictorBHT::set_update_widget_color(QString color_stylesheet) {
    value_event_update_instruction->setStyleSheet(color_stylesheet);
    value_event_update_address->setStyleSheet(color_stylesheet);
    value_event_update_index->setStyleSheet(color_stylesheet);
    value_event_update_result->setStyleSheet(color_stylesheet);
}

void DockPredictorBHT::setup(
    const machine::BranchPredictor *branch_predictor,
    const machine::Core *core) {
    connect(
        branch_predictor, &machine::BranchPredictor::bhr_updated,
        this, &DockPredictorBHT::update_bhr);
    connect(
        branch_predictor, &machine::BranchPredictor::prediction_done,
        this, &DockPredictorBHT::show_new_prediction);
    connect(
        branch_predictor, &machine::BranchPredictor::update_done,
        this, &DockPredictorBHT::show_new_update);
    connect(
        branch_predictor, &machine::BranchPredictor::predictor_stats_updated,
        this, &DockPredictorBHT::update_predictor_stats);
    connect(
        branch_predictor, &machine::BranchPredictor::predictor_bht_row_updated,
        this, &DockPredictorBHT::update_bht_row);
    connect(
        branch_predictor, &machine::BranchPredictor::cleared,
        this, &DockPredictorBHT::clear);
    connect(
        core, &machine::Core::step_started,
        this, &DockPredictorBHT::reset_colors);

    number_of_bhr_bits = branch_predictor->get_number_of_bhr_bits();
    number_of_bht_bits = branch_predictor->get_number_of_bht_bits();
    initial_state = branch_predictor->get_initial_state();
    const machine::PredictorType predictor_type { branch_predictor->get_predictor_type() };
    const bool is_predictor_dynamic { predictor_type == machine::PredictorType::SMITH_1_BIT
                                      || predictor_type == machine::PredictorType::SMITH_2_BIT
                                      || predictor_type
                                             == machine::PredictorType::SMITH_2_BIT_HYSTERESIS };
    const bool is_predictor_enabled { branch_predictor->get_enabled() };

    if (is_predictor_enabled) {
        content->setDisabled(false);
    } else {
        content->setDisabled(true);
    }

    // Init BHT
    if (is_predictor_dynamic) {
        bht->setDisabled(false);
        bht->setRowCount(qPow(2, number_of_bht_bits));
    } else {
        bht->setDisabled(true);
        bht->setRowCount(0);
    }
    init_table(initial_state);
    bht->resizeRowsToContents();
    set_table_color(Q_COLOR_DEFAULT);

    // Init name
    if (is_predictor_enabled) {
        label_type_value->setText(branch_predictor->get_predictor_name().toString());
    } else {
        label_type_value->setText("None");
    }

    // Init stats
    label_stats_correct_value->setText("0");
    label_stats_wrong_value->setText("0");
    label_stats_accuracy_value->setText("100 %");

    // Init last prediction
    value_event_predict_instruction->setText("");
    value_event_predict_address->setText("");
    value_event_predict_index->setText("");
    value_event_predict_result->setText("");
    set_predict_widget_color(STYLESHEET_COLOR_DEFAULT);

    // Init last update
    value_event_update_instruction->setText("");
    value_event_update_address->setText("");
    value_event_update_index->setText("");
    value_event_update_result->setText("");
    set_update_widget_color(STYLESHEET_COLOR_DEFAULT);
    if (is_predictor_dynamic) {
        group_event_update->setDisabled(false);
    } else {
        group_event_update->setDisabled(true);
    }

    // Init BHR
    if (number_of_bhr_bits > 0) {
        QString bhr_initial_value;
        bhr_initial_value.fill('0', number_of_bhr_bits);
        value_bhr->setText("0b" + bhr_initial_value);
        value_bhr->setDisabled(false);
    } else {
        value_bhr->setText("");
        value_bhr->setDisabled(true);
    }
}

void DockPredictorBHT::update_bhr(uint8_t number_of_bhr_bits, uint16_t register_value) {
    if (number_of_bhr_bits > 0) {
        QString binary_value, zero_padding;
        binary_value = QString::number(register_value, 2);
        zero_padding.fill('0', number_of_bhr_bits - binary_value.count());
        value_bhr->setText("0b" + zero_padding + binary_value);
    }
}

void DockPredictorBHT::show_new_prediction(
    uint16_t index,
    machine::PredictionInput input,
    machine::BranchResult result) {
    value_event_predict_instruction->setText(input.instruction.to_str());
    value_event_predict_address->setText(addr_to_hex_str(input.instruction_address));
    value_event_predict_index->setText(QString::number(index));
    value_event_predict_result->setText(machine::branch_result_to_string(result).toString());

    set_row_color(index, Q_COLOR_PREDICT);
    set_predict_widget_color(STYLESHEET_COLOR_PREDICT);
}

void DockPredictorBHT::show_new_update(uint16_t index, machine::PredictionFeedback feedback) {
    value_event_update_instruction->setText(feedback.instruction.to_str());
    value_event_update_address->setText(addr_to_hex_str(feedback.instruction_address));
    value_event_update_index->setText(QString::number(index));
    value_event_update_result->setText(
        machine::branch_result_to_string(feedback.result).toString());

    set_row_color(index, Q_COLOR_UPDATE);
    set_update_widget_color(STYLESHEET_COLOR_UPDATE);
}

void DockPredictorBHT::update_predictor_stats(machine::PredictionStatistics stats) {
    label_stats_correct_value->setText(QString::number(stats.number_of_correct_predictions));
    label_stats_wrong_value->setText(QString::number(stats.number_of_wrong_predictions));
    label_stats_accuracy_value->setText(QString::number(stats.accuracy) + " %");
}

void DockPredictorBHT::update_bht_row(uint16_t row_index, machine::BranchHistoryTableEntry bht_entry) {
    if (row_index >= bht->rowCount()) {
        WARN("BHT dock update received invalid row index: %u", row_index);
        return;
    }

    for (uint16_t column_index = 0; column_index < bht->columnCount(); column_index++) {
        QTableWidgetItem *item;

        item = get_bht_cell_item(row_index, DOCK_BHT_COL_STATE);
        item->setData(Qt::DisplayRole, machine::predictor_state_to_string(bht_entry.state, true).toString());

        item = get_bht_cell_item(row_index, DOCK_BHT_COL_CORRECT);
        item->setData(Qt::DisplayRole, QString::number(bht_entry.stats.number_of_correct_predictions));

        item = get_bht_cell_item(row_index, DOCK_BHT_COL_INCORRECT);
        item->setData(Qt::DisplayRole, QString::number(bht_entry.stats.number_of_wrong_predictions));

        item = get_bht_cell_item(row_index, DOCK_BHT_COL_ACCURACY);
        item->setData(Qt::DisplayRole, QString::number(bht_entry.stats.accuracy) + " %");
    }
}

void DockPredictorBHT::reset_colors() {
    set_table_color(Q_COLOR_DEFAULT);
    set_predict_widget_color(STYLESHEET_COLOR_DEFAULT);
    set_update_widget_color(STYLESHEET_COLOR_DEFAULT);
}

void DockPredictorBHT::clear() {
    // Clear BHT and colors
    reset_colors();
    init_table(initial_state);

    // Clear stats
    label_stats_correct_value->setText("0");
    label_stats_wrong_value->setText("0");
    label_stats_accuracy_value->setText("100 %");
    
    // Clear BHR
    if (number_of_bhr_bits > 0) {
        QString bhr_initial_value;
        bhr_initial_value.fill('0', number_of_bhr_bits);
        value_bhr->setText("0b" + bhr_initial_value);
    } else {
        value_bhr->setText("");
    }

    // Clear prediction display
    value_event_predict_instruction->setText("");
    value_event_predict_address->setText("");
    value_event_predict_index->setText("");
    value_event_predict_result->setText("");

    // Clear update display
    value_event_update_instruction->setText("");
    value_event_update_address->setText("");
    value_event_update_index->setText("");
    value_event_update_result->setText("");
}
