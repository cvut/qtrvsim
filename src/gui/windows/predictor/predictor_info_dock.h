#ifndef PREDICTOR_INFO_DOCK_H
#define PREDICTOR_INFO_DOCK_H

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
#include <QSpacerItem>

#define STYLESHEET_COLOR_DEFAULT "background: rgb(255,255,255);"
#define STYLESHEET_COLOR_PREDICT "background: rgb(255,173,173);"
#define STYLESHEET_COLOR_UPDATE "background: rgb(173,255,229);"
#define Q_COLOR_DEFAULT QColor(255, 255, 255)
#define Q_COLOR_PREDICT QColor(255, 173, 173)
#define Q_COLOR_UPDATE QColor(173, 255, 229)

class DockPredictorInfo : public QDockWidget {
    Q_OBJECT

    using Super = QDockWidget;

public: // Constructors & Destructor
    DockPredictorInfo(QWidget *parent);

private: // Internal functions
    void set_predict_widget_color(QString color_stylesheet);
    void set_update_widget_color(QString color_stylesheet);

public: // General functions
    void setup(const machine::BranchPredictor *branch_predictor, const machine::Core *core);

public slots:
    void update_bhr(uint8_t number_of_bhr_bits, uint16_t register_value);
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
    void update_stats(machine::PredictionStatistics stats);
    void reset_colors();
    void clear_stats();
    void clear_bhr();
    void clear_predict_widget();
    void clear_update_widget();
    void clear();

private: // Internal variables
    bool is_predictor_enabled{ false };
    bool is_predictor_dynamic{ false };
    uint8_t number_of_bhr_bits{ 0 };
    machine::PredictorState initial_state{ machine::PredictorState::UNDEFINED };

    QT_OWNED QGroupBox *content{ new QGroupBox() };
    QT_OWNED QVBoxLayout *layout_main{ new QVBoxLayout() };
    QT_OWNED QSpacerItem *vertical_spacer{ new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding) };

    // Stats
    QT_OWNED QGridLayout *layout_stats{ new QGridLayout() };
    QT_OWNED QLabel *label_stats_total_text{ new QLabel() };
    QT_OWNED QLabel *label_stats_miss_text{ new QLabel() };
    QT_OWNED QLabel *label_stats_accuracy_text{ new QLabel() };
    QT_OWNED QLabel *label_stats_total_value{ new QLabel() };
    QT_OWNED QLabel *label_stats_miss_value{ new QLabel() };
    QT_OWNED QLabel *label_stats_accuracy_value{ new QLabel() };

    // Prediction & Update
    QT_OWNED QHBoxLayout *layout_event{ new QHBoxLayout() };

    // Prediction
    QT_OWNED QGroupBox *group_event_predict{ new QGroupBox() };
    QT_OWNED QVBoxLayout *layout_event_predict{ new QVBoxLayout() };
    QT_OWNED QHBoxLayout *layout_event_predict_index{ new QHBoxLayout() };
    QT_OWNED QVBoxLayout *layout_event_predict_index_btb{ new QVBoxLayout() };
    QT_OWNED QVBoxLayout *layout_event_predict_index_bht{ new QVBoxLayout() };
    QT_OWNED QLabel *label_event_predict_header{ new QLabel() };
    QT_OWNED QLabel *label_event_predict_instruction{ new QLabel() };
    QT_OWNED QLabel *label_event_predict_address{ new QLabel() };
    QT_OWNED QLabel *label_event_predict_index_btb{ new QLabel() };
    QT_OWNED QLabel *label_event_predict_index_bht{ new QLabel() };
    QT_OWNED QLabel *label_event_predict_result{ new QLabel() };
    QT_OWNED QLineEdit *value_event_predict_instruction{ new QLineEdit() };
    QT_OWNED QLineEdit *value_event_predict_address{ new QLineEdit() };
    QT_OWNED QLineEdit *value_event_predict_index_btb{ new QLineEdit() };
    QT_OWNED QLineEdit *value_event_predict_index_bht{ new QLineEdit() };
    QT_OWNED QLineEdit *value_event_predict_result{ new QLineEdit() };

    // Update
    QT_OWNED QGroupBox *group_event_update{ new QGroupBox() };
    QT_OWNED QVBoxLayout *layout_event_update{ new QVBoxLayout() };
    QT_OWNED QHBoxLayout *layout_event_update_index{ new QHBoxLayout() };
    QT_OWNED QVBoxLayout *layout_event_update_index_btb{ new QVBoxLayout() };
    QT_OWNED QVBoxLayout *layout_event_update_index_bht{ new QVBoxLayout() };
    QT_OWNED QLabel *label_event_update_header{ new QLabel() };
    QT_OWNED QLabel *label_event_update_instruction{ new QLabel() };
    QT_OWNED QLabel *label_event_update_address{ new QLabel() };
    QT_OWNED QLabel *label_event_update_index_btb{ new QLabel() };
    QT_OWNED QLabel *label_event_update_index_bht{ new QLabel() };
    QT_OWNED QLabel *label_event_update_result{ new QLabel() };
    QT_OWNED QLineEdit *value_event_update_instruction{ new QLineEdit() };
    QT_OWNED QLineEdit *value_event_update_address{ new QLineEdit() };
    QT_OWNED QLineEdit *value_event_update_index_btb{ new QLineEdit() };
    QT_OWNED QLineEdit *value_event_update_index_bht{ new QLineEdit() };
    QT_OWNED QLineEdit *value_event_update_result{ new QLineEdit() };

    // BHR
    QT_OWNED QHBoxLayout *layout_bhr{ new QHBoxLayout() };
    QT_OWNED QLabel *label_bhr{ new QLabel() };
    QT_OWNED QLineEdit *value_bhr{ new QLineEdit() };
};

#endif // PREDICTOR_INFO_DOCK_H