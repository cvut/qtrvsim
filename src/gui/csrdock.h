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
    void csr_changed(enum machine::ControlState::CsrRegisters reg, uint64_t val);
    void csr_read(enum machine::ControlState::CsrRegisters reg, uint64_t val);
    void clear_highlights();

private:
    StaticTable *widg;
    QScrollArea *scrollarea;

    QLabel *csr_view[machine::ControlState::CSR_REGS_CNT] {};
    bool csr_highlighted[machine::ControlState::CSR_REGS_CNT] {};
    bool csr_highlighted_any;

    QPalette pal_normal;
    QPalette pal_updated;
    QPalette pal_read;

    void labelVal(QLabel *label, uint64_t val);
};

#endif // COP0DOCK_H
