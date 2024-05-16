#ifndef PREDICTOR_BHT_DOCK_H
#define PREDICTOR_BHT_DOCK_H

#include "common/polyfills/qt5/qtableview.h"
#include "machine/machine.h"
#include "machine/memory/address.h"
#include "machine/predictor.h"
#include "machine/predictor_types.h"

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
#define DOCK_BHT_COL_HISTORY 1
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
    ~DockPredictorBHT();

private: // Internal functions
    uint8_t init_number_of_bits(const uint8_t number_of_bits) const;
    void init_table(machine::PredictorState initial_state = machine::PredictorState::UNDEFINED);
    void set_table_color(QColor color);
    void set_row_color(uint16_t row_index, QColor color);
    void set_predict_widget_color(QString color_stylesheet);
    void set_update_widget_color(QString color_stylesheet);

public: // General functions
    void setup(const machine::BranchPredictor *branch_predictor, const machine::Core *core);

public slots:
    void update_bhr(uint8_t number_of_bhr_bits, uint16_t register_value);
    void update_new_prediction(machine::PredictionInput input, machine::BranchResult result);
    void update_new_update(machine::PredictionFeedback feedback);
    void update_predictor_stats(machine::PredictionStatistics stats);
    void update_bht_row(uint16_t index, machine::BranchHistoryTableEntry entry);
    void reset_colors();

private: // Internal variables
    uint8_t number_of_bhr_bits;
    uint8_t number_of_bht_bits;

    QGroupBox *content;

    QVBoxLayout *layout_main;

    // Name
    QHBoxLayout *layout_type;
    QLabel *label_type;
    QLabel *label_type_value;

    QGridLayout *layout_stats;
    QLabel *label_stats_correct_text;
    QLabel *label_stats_wrong_text;
    QLabel *label_stats_accuracy_text;
    QLabel *label_stats_correct_value;
    QLabel *label_stats_wrong_value;
    QLabel *label_stats_accuracy_value;

    QHBoxLayout *layout_event;

    QGroupBox *group_event_predict;
    QVBoxLayout *layout_event_predict;
    QLabel *label_event_predict_header;
    QLabel *label_event_predict_instruction;
    QLabel *label_event_predict_address;
    QLabel *label_event_predict_index;
    QLabel *label_event_predict_result;
    QLineEdit *value_event_predict_instruction;
    QLineEdit *value_event_predict_address;
    QLineEdit *value_event_predict_index;
    QLineEdit *value_event_predict_result;

    QGroupBox *group_event_update;
    QVBoxLayout *layout_event_update;
    QLabel *label_event_update_header;
    QLabel *label_event_update_instruction;
    QLabel *label_event_update_address;
    QLabel *label_event_update_index;
    QLabel *label_event_update_result;
    QLineEdit *value_event_update_instruction;
    QLineEdit *value_event_update_address;
    QLineEdit *value_event_update_index;
    QLineEdit *value_event_update_result;

    QHBoxLayout *layout_bhr;
    QLabel *label_bhr;
    QLineEdit *value_bhr;

    QTableWidget *bht;
};

#endif // PREDICTOR_BHT_DOCK_H