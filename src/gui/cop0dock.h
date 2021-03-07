#ifndef COP0DOCK_H
#define COP0DOCK_H

#include "machine/machine.h"
#include "statictable.h"

#include <QDockWidget>
#include <QFormLayout>
#include <QLabel>
#include <QPalette>
#include <QPropertyAnimation>
#include <QScrollArea>

class Cop0Dock : public QDockWidget {
    Q_OBJECT
public:
    Cop0Dock(QWidget *parent);
    ~Cop0Dock() override;

    void setup(machine::Machine *machine);

private slots:
    void
    cop0reg_changed(enum machine::Cop0State::Cop0Registers reg, uint32_t val);
    void cop0reg_read(enum machine::Cop0State::Cop0Registers reg, uint32_t val);
    void clear_highlights();

private:
    StaticTable *widg;
    QScrollArea *scrollarea;

    QLabel *cop0reg[machine::Cop0State::COP0REGS_CNT] {};
    bool cop0reg_highlighted[machine::Cop0State::COP0REGS_CNT] {};
    bool cop0reg_highlighted_any;

    QPalette pal_normal;
    QPalette pal_updated;
    QPalette pal_read;

    void labelVal(QLabel *label, uint32_t val);
};

#endif // COP0DOCK_H
