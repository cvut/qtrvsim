#ifndef REGISTERSDOCK_H
#define REGISTERSDOCK_H

#include "machine/machine.h"
#include "statictable.h"

#include <QDockWidget>
#include <QFormLayout>
#include <QLabel>
#include <QPalette>
#include <QPropertyAnimation>
#include <QScrollArea>

class RegistersDock : public QDockWidget {
    Q_OBJECT
public:
    RegistersDock(QWidget *parent);
    ~RegistersDock() override;

    void setup(machine::Machine *machine);

private slots:
    void pc_changed(machine::Address val);
    void gp_changed(machine::RegisterId i, machine::RegisterValue val);
    void hi_lo_changed(bool hi, machine::RegisterValue val);
    void gp_read(machine::RegisterId i, machine::RegisterValue val);
    void hi_lo_read(bool hi, machine::RegisterValue val);
    void clear_highlights();

private:
    StaticTable *widg;
    QScrollArea *scrollarea;

    QLabel *pc {};
    QLabel *hi {};
    QLabel *lo {};
    QLabel *gp[32] {};

    uint32_t gp_highlighted;
    bool hi_highlighted;
    bool lo_highlighted;

    QPalette pal_normal;
    QPalette pal_updated;
    QPalette pal_read;

    void labelVal(QLabel *label, uint32_t val);
};

#endif // REGISTERSDOCK_H
