#include "predictor_info_dock.h"

#include <QApplication>
#include <QLabel>
#include <QObject>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

LOG_CATEGORY("gui.DockPredictorInfo");

DockPredictorInfo::DockPredictorInfo(QWidget *parent) : Super(parent) {
    setObjectName("PredictorInfo");
    setWindowTitle("Predictor Info");

    /////////////////////////
    // Assign layout

    // Stats
    layout_stats->addWidget(label_stats_total_text, 0, 0);
    layout_stats->addWidget(label_stats_total_value, 0, 1);
    layout_stats->addWidget(label_stats_miss_text, 1, 0);
    layout_stats->addWidget(label_stats_miss_value, 1, 1);
    layout_stats->addWidget(label_stats_accuracy_text, 2, 0);
    layout_stats->addWidget(label_stats_accuracy_value, 2, 1);

    // BHR
    layout_bhr->addWidget(label_bhr);
    layout_bhr->addWidget(value_bhr);

    // Prediction - BTB index
    layout_event_predict_index_btb->addWidget(label_event_predict_index_btb);
    layout_event_predict_index_btb->addWidget(value_event_predict_index_btb);

    // Prediction - BHT index
    layout_event_predict_index_bht->addWidget(label_event_predict_index_bht);
    layout_event_predict_index_bht->addWidget(value_event_predict_index_bht);

    // Prediction - Indexes
    layout_event_predict_index->addLayout(layout_event_predict_index_btb);
    layout_event_predict_index->addLayout(layout_event_predict_index_bht);

    // Prediction
    layout_event->addWidget(group_event_predict);
    group_event_predict->setLayout(layout_event_predict);
    layout_event_predict->addWidget(label_event_predict_header);
    layout_event_predict->addWidget(label_event_predict_instruction);
    layout_event_predict->addWidget(value_event_predict_instruction);
    layout_event_predict->addWidget(label_event_predict_address);
    layout_event_predict->addWidget(value_event_predict_address);
    layout_event_predict->addLayout(layout_event_predict_index);
    layout_event_predict->addWidget(label_event_predict_result);
    layout_event_predict->addWidget(value_event_predict_result);

    // Update - BTB index
    layout_event_update_index_btb->addWidget(label_event_update_index_btb);
    layout_event_update_index_btb->addWidget(value_event_update_index_btb);

    // Update - BHT index
    layout_event_update_index_bht->addWidget(label_event_update_index_bht);
    layout_event_update_index_bht->addWidget(value_event_update_index_bht);

    // Update - Indexes
    layout_event_update_index->addLayout(layout_event_update_index_btb);
    layout_event_update_index->addLayout(layout_event_update_index_bht);

    // Update
    layout_event->addWidget(group_event_update);
    group_event_update->setLayout(layout_event_update);
    layout_event_update->addWidget(label_event_update_header);
    layout_event_update->addWidget(label_event_update_instruction);
    layout_event_update->addWidget(value_event_update_instruction);
    layout_event_update->addWidget(label_event_update_address);
    layout_event_update->addWidget(value_event_update_address);
    layout_event_update->addLayout(layout_event_update_index);
    layout_event_update->addWidget(label_event_update_result);
    layout_event_update->addWidget(value_event_update_result);

    // Main layout
    layout_main->addLayout(layout_stats);
    layout_main->addLayout(layout_bhr);
    layout_main->addLayout(layout_event);
    layout_main->addSpacerItem(vertical_spacer);

    content->setLayout(layout_main);
    setWidget(content);

    /////////////////////////
    // Init widget properties

    // Stats
    label_stats_total_text->setText("Jump/Branch count:");
    label_stats_miss_text->setText("Misprediction count:");
    label_stats_accuracy_text->setText("Total accuracy:");
    clear_stats();

    // BHR
    label_bhr->setText("Branch History Register:");
    value_bhr->setReadOnly(true);
    value_bhr->setAlignment(Qt::AlignCenter);
    value_bhr->setFixedWidth(120);
    clear_bhr();

    // Prediction
    label_event_predict_header->setText("Last prediction");
    label_event_predict_header->setStyleSheet("font-weight: bold;");
    label_event_predict_instruction->setText("Instruction:");
    label_event_predict_address->setText("Instruction Address:");
    label_event_predict_index_btb->setText("BTB index:");
    label_event_predict_index_bht->setText("BHT index:");
    label_event_predict_result->setText("Prediction result:");
    value_event_predict_instruction->setReadOnly(true);
    value_event_predict_address->setReadOnly(true);
    value_event_predict_index_btb->setReadOnly(true);
    value_event_predict_index_bht->setReadOnly(true);
    value_event_predict_result->setReadOnly(true);
    value_event_predict_instruction->setAlignment(Qt::AlignCenter);
    value_event_predict_address->setAlignment(Qt::AlignCenter);
    value_event_predict_index_btb->setAlignment(Qt::AlignCenter);
    value_event_predict_index_bht->setAlignment(Qt::AlignCenter);
    value_event_predict_result->setAlignment(Qt::AlignCenter);
    set_predict_widget_color(STYLESHEET_COLOR_DEFAULT);
    clear_predict_widget();

    // Update
    label_event_update_header->setText("Last update");
    label_event_update_header->setStyleSheet("font-weight: bold;");
    label_event_update_instruction->setText("Instruction:");
    label_event_update_address->setText("Instruction Address:");
    label_event_update_index_btb->setText("BTB index:");
    label_event_update_index_bht->setText("BHT index:");
    label_event_update_result->setText("Branch result:");
    value_event_update_instruction->setReadOnly(true);
    value_event_update_address->setReadOnly(true);
    value_event_update_index_btb->setReadOnly(true);
    value_event_update_index_bht->setReadOnly(true);
    value_event_update_result->setReadOnly(true);
    value_event_update_instruction->setAlignment(Qt::AlignCenter);
    value_event_update_address->setAlignment(Qt::AlignCenter);
    value_event_update_index_btb->setAlignment(Qt::AlignCenter);
    value_event_update_index_bht->setAlignment(Qt::AlignCenter);
    value_event_update_result->setAlignment(Qt::AlignCenter);
    set_update_widget_color(STYLESHEET_COLOR_DEFAULT);
    clear_update_widget();
}

void DockPredictorInfo::set_predict_widget_color(QString color_stylesheet) {
    value_event_predict_instruction->setStyleSheet(color_stylesheet);
    value_event_predict_address->setStyleSheet(color_stylesheet);
    value_event_predict_index_btb->setStyleSheet(color_stylesheet);
    if (is_predictor_dynamic) {
        value_event_predict_index_bht->setStyleSheet(color_stylesheet);
    }
    value_event_predict_result->setStyleSheet(color_stylesheet);
}

void DockPredictorInfo::set_update_widget_color(QString color_stylesheet) {
    value_event_update_instruction->setStyleSheet(color_stylesheet);
    value_event_update_address->setStyleSheet(color_stylesheet);
    value_event_update_index_btb->setStyleSheet(color_stylesheet);
    if (is_predictor_dynamic) {
        value_event_update_index_bht->setStyleSheet(color_stylesheet);
    }
    value_event_update_result->setStyleSheet(color_stylesheet);
}

void DockPredictorInfo::setup(
    const machine::BranchPredictor *branch_predictor,
    const machine::Core *core) {

    clear();

    number_of_bhr_bits = branch_predictor->get_number_of_bhr_bits();
    initial_state = branch_predictor->get_initial_state();
    const machine::PredictorType predictor_type { branch_predictor->get_predictor_type() };
    is_predictor_dynamic = machine::is_predictor_type_dynamic(predictor_type);
    is_predictor_enabled = branch_predictor->get_enabled();

    if (is_predictor_enabled) {
        connect(
            branch_predictor, &machine::BranchPredictor::total_stats_updated,
            this, &DockPredictorInfo::update_stats);
        connect(
            branch_predictor, &machine::BranchPredictor::prediction_done,
            this, &DockPredictorInfo::show_new_prediction);
        connect(
            core, &machine::Core::step_started,
            this, &DockPredictorInfo::reset_colors);
        connect(
            branch_predictor, &machine::BranchPredictor::update_done,
            this, &DockPredictorInfo::show_new_update);

        if (is_predictor_dynamic) {
            connect(
                branch_predictor, &machine::BranchPredictor::bhr_updated,
                this, &DockPredictorInfo::update_bhr);
        }
    }

    // Toggle BHT index display
    if (is_predictor_dynamic) {
        label_event_predict_index_bht->setEnabled(true);
        value_event_predict_index_bht->setEnabled(true);
        label_event_update_index_bht->setEnabled(true);
        value_event_update_index_bht->setEnabled(true);
    } else {
        label_event_predict_index_bht->setEnabled(false);
        value_event_predict_index_bht->setEnabled(false);
        label_event_update_index_bht->setEnabled(false);
        value_event_update_index_bht->setEnabled(false);
    }

    // Toggle BHR display
    if (is_predictor_dynamic && number_of_bhr_bits > 0) {
        label_bhr->setEnabled(true);
        value_bhr->setEnabled(true);
    } else {
        label_bhr->setEnabled(false);
        value_bhr->setEnabled(false);
    }

    // Toggle whole widget
    if (is_predictor_enabled) {
        content->setDisabled(false);
    } else {
        content->setDisabled(true);
    }

    clear_bhr();
}

void DockPredictorInfo::update_bhr(uint8_t number_of_bhr_bits, uint16_t register_value) {
    if (number_of_bhr_bits > 0) {
        QString binary_value, zero_padding;
        binary_value = QString::number(register_value, 2);
        zero_padding.fill('0', number_of_bhr_bits - binary_value.count());
        value_bhr->setText("0b" + zero_padding + binary_value);
    } else {
        value_bhr->setText("");
    }
}

void DockPredictorInfo::show_new_prediction(
    uint16_t btb_index,
    uint16_t bht_index,
    machine::PredictionInput input,
    machine::BranchResult result,
    machine::BranchType branch_type) {
    value_event_predict_instruction->setText(input.instruction.to_str());
    value_event_predict_address->setText(addr_to_hex_str(input.instruction_address));
    value_event_predict_index_btb->setText(QString::number(btb_index));
    if (!is_predictor_dynamic) {
        value_event_predict_index_bht->setText("");
    } else if (branch_type == machine::BranchType::BRANCH) {
        value_event_predict_index_bht->setText(QString::number(bht_index));
    } else {
        value_event_predict_index_bht->setText("N/A");
    }
    value_event_predict_result->setText(machine::branch_result_to_string(result).toString());
    set_predict_widget_color(STYLESHEET_COLOR_PREDICT);
}

void DockPredictorInfo::show_new_update(
    uint16_t btb_index,
    uint16_t bht_index,
    machine::PredictionFeedback feedback) {
    value_event_update_instruction->setText(feedback.instruction.to_str());
    value_event_update_address->setText(addr_to_hex_str(feedback.instruction_address));
    value_event_update_index_btb->setText(QString::number(btb_index));
    if (!is_predictor_dynamic) {
        value_event_update_index_bht->setText("");
    } else if (feedback.branch_type == machine::BranchType::BRANCH ) {
        value_event_update_index_bht->setText(QString::number(bht_index));
    } else {
        value_event_update_index_bht->setText("N/A");
    }
    value_event_update_result->setText(
        machine::branch_result_to_string(feedback.result).toString());
    set_update_widget_color(STYLESHEET_COLOR_UPDATE);
}

void DockPredictorInfo::update_stats(machine::PredictionStatistics stats) {
    label_stats_total_value->setText(QString::number(stats.correct));
    label_stats_miss_value->setText(QString::number(stats.wrong));
    label_stats_accuracy_value->setText(QString::number(stats.accuracy) + " %");
}

void DockPredictorInfo::reset_colors() {
    set_predict_widget_color(STYLESHEET_COLOR_DEFAULT);
    set_update_widget_color(STYLESHEET_COLOR_DEFAULT);
}

void DockPredictorInfo::clear_stats() {
    label_stats_total_value->setText("0");
    label_stats_miss_value->setText("0");
    label_stats_accuracy_value->setText("N/A");
}

void DockPredictorInfo::clear_bhr() {
    if (number_of_bhr_bits > 0) {
        QString zero_padding;
        zero_padding.fill('0', number_of_bhr_bits);
        value_bhr->setText("0b" + zero_padding);
    } else {
        value_bhr->setText("");
    }
}

void DockPredictorInfo::clear_predict_widget() {
    value_event_predict_instruction->setText("");
    value_event_predict_address->setText("");
    value_event_predict_index_btb->setText("");
    value_event_predict_index_bht->setText("");
    value_event_predict_result->setText("");
    set_predict_widget_color(STYLESHEET_COLOR_DEFAULT);
}

void DockPredictorInfo::clear_update_widget() {
    value_event_update_instruction->setText("");
    value_event_update_address->setText("");
    value_event_update_index_btb->setText("");
    value_event_update_index_bht->setText("");
    value_event_update_result->setText("");
    set_update_widget_color(STYLESHEET_COLOR_DEFAULT);
}

void DockPredictorInfo::clear() {
    clear_stats();
    clear_bhr();
    clear_predict_widget();
    clear_update_widget();
}
