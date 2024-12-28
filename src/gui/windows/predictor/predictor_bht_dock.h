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
    void set_table_color(QColor color);
    void set_row_color(uint16_t row_index, QColor color);

public: // General functions
    void setup(const machine::BranchPredictor *branch_predictor, const machine::Core *core);

public slots:
    void show_new_prediction(
        uint16_t btb_index,
        uint16_t bht_index,
        machine::PredictionInput input,
        machine::BranchResult result,
        machine::BranchType branch_type);
    void show_new_update(
        uint16_t btb_index,
        uint16_t bht_index,
        machine::PredictionFeedback feedback);
    void update_predictor_stats(machine::PredictionStatistics stats);
    void update_bht_row(uint16_t row_index, machine::BranchHistoryTableEntry bht_entry);
    void reset_colors();
    void clear_name();
    void clear_stats();
    void clear_bht(machine::PredictorState initial_state = machine::PredictorState::UNDEFINED);
    void clear();

private: // Internal variables
    uint8_t number_of_bhr_bits{ 0 };
    uint8_t number_of_bht_bits{ 0 };
    machine::PredictorState initial_state{ machine::PredictorState::UNDEFINED };

    QT_OWNED QGroupBox *content{ new QGroupBox() };

    QT_OWNED QVBoxLayout *layout_main{ new QVBoxLayout() };

    // Name
    QT_OWNED QHBoxLayout *layout_type{ new QHBoxLayout() };
    QT_OWNED QLabel *label_type{ new QLabel() };
    QT_OWNED QLabel *label_type_value{ new QLabel() };

    // Stats
    QT_OWNED QGridLayout *layout_stats{ new QGridLayout() };
    QT_OWNED QLabel *label_stats_correct_text{ new QLabel() };
    QT_OWNED QLabel *label_stats_wrong_text{ new QLabel() };
    QT_OWNED QLabel *label_stats_accuracy_text{ new QLabel() };
    QT_OWNED QLabel *label_stats_correct_value{ new QLabel() };
    QT_OWNED QLabel *label_stats_wrong_value{ new QLabel() };
    QT_OWNED QLabel *label_stats_accuracy_value{ new QLabel() };

    // Prediction & Update
    QT_OWNED QHBoxLayout *layout_event{ new QHBoxLayout() };

    // BHT
    QT_OWNED QTableWidget *bht{ new QTableWidget() };
};

#endif // PREDICTOR_BHT_DOCK_H