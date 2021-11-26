#ifndef MEMORYTABLEVIEW_H
#define MEMORYTABLEVIEW_H

#include "common/polyfills/qt5/qtableview.h"
#include "machine/memory/address.h"

#include <QObject>
#include <QSettings>
#include <QSharedPointer>

class MemoryTableView : public Poly_QTableView {
    Q_OBJECT

    using Super = Poly_QTableView;

public:
    MemoryTableView(QWidget *parent, QSettings *settings);

    void resizeEvent(QResizeEvent *event) override;
signals:
    void address_changed(machine::Address address);
    void adjust_scroll_pos_queue();
public slots:
    void set_cell_size(int index);
    void go_to_address(machine::Address address);
    void focus_address(machine::Address address);
    void recompute_columns();

protected:
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void adjust_scroll_pos_check();
    void adjust_scroll_pos_process();

private:
    void addr0_save_change(machine::Address val);
    void adjustColumnCount();
    QSettings *settings;

    machine::Address initial_address;
    bool adjust_scroll_pos_in_progress;
};

#endif // MEMORYTABLEVIEW_H
