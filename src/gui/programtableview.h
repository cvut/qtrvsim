#ifndef PROGRAMTABLEVIEW_H
#define PROGRAMTABLEVIEW_H

#include "machine/memory/address.h"

#include <QObject>
#include <QSettings>
#include <QSharedPointer>
#include <QTableView>

class ProgramTableView : public QTableView {
    Q_OBJECT

    using Super = QTableView;

public:
    ProgramTableView(QWidget *parent, QSettings *settings);

    void resizeEvent(QResizeEvent *event) override;
signals:
    void address_changed(uint32_t address);
    void adjust_scroll_pos_queue();
public slots:
    void go_to_address(machine::Address address);
    void focus_address(machine::Address address);
    void focus_address_with_save(machine::Address address);

protected:
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void adjust_scroll_pos_check();
    void adjust_scroll_pos_process();

private:
    void go_to_address_priv(machine::Address address);
    void addr0_save_change(machine::Address val);
    void adjustColumnCount();
    QSettings *settings;

    machine::Address initial_address;
    bool adjust_scroll_pos_in_progress;
    bool need_addr0_save;
};

#endif // PROGRAMTABLEVIEW_H
