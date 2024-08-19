#ifndef PREDICTOR_BTB_DOCK_H
#define PREDICTOR_BTB_DOCK_H

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

#define DOCK_BTB_COL_INDEX 0
#define DOCK_BTB_COL_INSTR_ADDR 1
#define DOCK_BTB_COL_TARGET_ADDR 2
#define DOCK_BTB_COL_TYPE 3

#define Q_COLOR_DEFAULT QColor(255, 255, 255)
#define Q_COLOR_PREDICT QColor(255, 173, 173)
#define Q_COLOR_UPDATE QColor(173, 255, 229)

// Branch Target Buffer Dock
class DockPredictorBTB : public QDockWidget {
    Q_OBJECT

    using Super = QDockWidget;

public: // Constructors & Destructor
    DockPredictorBTB(QWidget *parent);
    ~DockPredictorBTB();

private: // Internal functions
    uint8_t init_number_of_bits(const uint8_t b) const;
    QTableWidgetItem* get_btb_cell_item(uint8_t row_index, uint8_t col_index);
    void init_table();
    void set_table_color(QColor color);
    void set_row_color(uint16_t row_index, QColor color);
    

public: // General functions
    void setup(const machine::BranchPredictor *branch_predictor, const machine::Core *core);

public slots:
    void update_btb_row(uint16_t row_index, machine::BranchTargetBufferEntry btb_entry);
    void highligh_row_after_prediction(uint16_t index);
    void reset_colors();
    void clear();

private: // Internal variables
    uint8_t number_of_bits;
    QTableWidget *btb;
    QVBoxLayout *layout;
    QWidget *content;
};

#endif // PREDICTOR_BTB_DOCK_H