#ifndef PREDICTOR_BHT_DOCK_H
#define PREDICTOR_BHT_DOCK_H

#include "common/polyfills/qt5/qtableview.h"
#include "machine/machine.h"
#include "machine/memory/address.h"
#include "machine/predictor.h"
#include "machine/predictor_types.h"
#include "ui/hexlineedit.h"

#include <QDockWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QSettings>
#include <QSharedPointer>
#include <QTableView>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QtMath>

#define DOCK_BHT_COL_INDEX 0
#define DOCK_BHT_COL_STATE 1
#define DOCK_BHT_COL_CORRECT 2
#define DOCK_BHT_COL_INCORRECT 3
#define DOCK_BHT_COL_ACCURACY 4

#define STYLESHEET_COLOR_DEFAULT "background: rgb(255,255,255);"
#define STYLESHEET_COLOR_PREDICT "background: rgb(255,173,173);"
#define STYLESHEET_COLOR_UPDATE "background: rgb(173,255,229);"
#define Q_COLOR_DEFAULT QColor(255, 255, 255)
#define Q_COLOR_PREDICT QColor(255, 173, 173)
#define Q_COLOR_UPDATE QColor(173, 255, 229)

class DockPredictorBHT : public QDockWidget {
    Q_OBJECT

    using Super = QDockWidget;

public: // Constructors & Destructor
    DockPredictorBHT(QWidget *parent);

private: // Internal functions
    QTableWidgetItem* get_bht_cell_item(uint8_t row_index, uint8_t col_index);
    void init_table(machine::PredictorState initial_state = machine::PredictorState::UNDEFINED);
    void set_table_color(QColor color);
    void set_row_color(uint16_t row_index, QColor color);
    void set_predict_widget_color(QString color_stylesheet);
    void set_update_widget_color(QString color_stylesheet);

public: // General functions
    void setup(const machine::BranchPredictor *branch_predictor, const machine::Core *core);

public slots:
    void update_bhr(uint8_t number_of_bhr_bits, uint16_t register_value);
    void show_new_prediction(
        uint16_t index,
        machine::PredictionInput input,
        machine::BranchResult result);
    void show_new_update(uint16_t index, machine::PredictionFeedback feedback);
    void update_predictor_stats(machine::PredictionStatistics stats);
    void update_bht_row(uint16_t row_index, machine::BranchHistoryTableEntry bht_entry);
    void reset_colors();
    void clear();

private: // Internal variables
    uint8_t number_of_bhr_bits;
    uint8_t number_of_bht_bits;
    machine::PredictorState initial_state;

    QT_OWNED QGroupBox *content;

    QT_OWNED QVBoxLayout *layout_main;

    // Name
    QT_OWNED QHBoxLayout *layout_type;
    QT_OWNED QLabel *label_type;
    QT_OWNED QLabel *label_type_value;

    // Stats
    QT_OWNED QGridLayout *layout_stats;
    QT_OWNED QLabel *label_stats_correct_text;
    QT_OWNED QLabel *label_stats_wrong_text;
    QT_OWNED QLabel *label_stats_accuracy_text;
    QT_OWNED QLabel *label_stats_correct_value;
    QT_OWNED QLabel *label_stats_wrong_value;
    QT_OWNED QLabel *label_stats_accuracy_value;

    // Prediction & Update
    QT_OWNED QHBoxLayout *layout_event;

    // Prediction
    QT_OWNED QGroupBox *group_event_predict;
    QT_OWNED QVBoxLayout *layout_event_predict;
    QT_OWNED QLabel *label_event_predict_header;
    QT_OWNED QLabel *label_event_predict_instruction;
    QT_OWNED QLabel *label_event_predict_address;
    QT_OWNED QLabel *label_event_predict_index;
    QT_OWNED QLabel *label_event_predict_result;
    QT_OWNED QLineEdit *value_event_predict_instruction;
    QT_OWNED QLineEdit *value_event_predict_address;
    QT_OWNED QLineEdit *value_event_predict_index;
    QT_OWNED QLineEdit *value_event_predict_result;

    // Update
    QT_OWNED QGroupBox *group_event_update;
    QT_OWNED QVBoxLayout *layout_event_update;
    QT_OWNED QLabel *label_event_update_header;
    QT_OWNED QLabel *label_event_update_instruction;
    QT_OWNED QLabel *label_event_update_address;
    QT_OWNED QLabel *label_event_update_index;
    QT_OWNED QLabel *label_event_update_result;
    QT_OWNED QLineEdit *value_event_update_instruction;
    QT_OWNED QLineEdit *value_event_update_address;
    QT_OWNED QLineEdit *value_event_update_index;
    QT_OWNED QLineEdit *value_event_update_result;

    // BHR
    QT_OWNED QHBoxLayout *layout_bhr;
    QT_OWNED QLabel *label_bhr;
    QT_OWNED QLineEdit *value_bhr;

    // BHT
    QT_OWNED QTableWidget *bht;
};

#endif // PREDICTOR_BHT_DOCK_H