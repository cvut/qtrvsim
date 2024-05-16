#ifndef PREDICTOR_BTT_DOCK_H
#define PREDICTOR_BTT_DOCK_H

#include "common/polyfills/qt5/qtableview.h"
#include "machine/machine.h"
#include "machine/memory/address.h"
#include "machine/predictor.h"
#include "machine/predictor_types.h"

#include <QColor>
#include <QComboBox>
#include <QDockWidget>
#include <QHeaderView>
#include <QLabel>
#include <QObject>
#include <QSettings>
#include <QSharedPointer>
#include <QTableView>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QtMath>

#define DOCK_BTT_COL_INDEX 0
#define DOCK_BTT_COL_INSTR_ADDR 1
#define DOCK_BTT_COL_TARGET_ADDR 2

#define Q_COLOR_DEFAULT QColor(255, 255, 255)
#define Q_COLOR_PREDICT QColor(255, 173, 173)
#define Q_COLOR_UPDATE QColor(173, 255, 229)

// Branch Target Table Dock
class DockPredictorBTT : public QDockWidget {
    Q_OBJECT

    using Super = QDockWidget;

public: // Constructors & Destructor
    DockPredictorBTT(QWidget *parent);
    ~DockPredictorBTT();

private: // Internal functions
    uint8_t init_number_of_bits(const uint8_t number_of_bits) const;
    void init_table();
    void set_table_color(QColor color);
    void set_row_color(uint16_t row_index, QColor color);

public: // General functions
    void setup(const machine::BranchPredictor *branch_predictor, const machine::Core *core);

public slots:
    void update_row(
        uint16_t index,
        machine::Address instruction_address,
        machine::Address target_address);
    void update_new_prediction(machine::PredictionInput input, machine::BranchResult result);
    void reset_colors();

private: // Internal variables
    uint8_t number_of_bits;
    QTableWidget *btt;
    QVBoxLayout *layout;
    QWidget *content;
};

#endif // PREDICTOR_BTT_DOCK_H