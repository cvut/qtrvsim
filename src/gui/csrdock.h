#ifndef CSRDOCK_H
#define CSRDOCK_H

#include "machine/csr/controlstate.h"
#include "machine/machine.h"
#include "statictable.h"

#include <QDockWidget>
#include <QFormLayout>
#include <QLabel>
#include <QPalette>
#include <QPropertyAnimation>
#include <QScrollArea>

class CsrDock : public QDockWidget {
    Q_OBJECT
public:
    CsrDock(QWidget *parent);
    ~CsrDock() override;

    void setup(machine::Machine *machine);

private slots:
    void csr_changed(size_t internal_reg_id, machine::RegisterValue val);
    void csr_read(size_t internal_reg_id, machine::RegisterValue val);
    void clear_highlights();

private:
    StaticTable *widg;
    QScrollArea *scrollarea;

    QLabel *csr_view[machine::CSR::REGISTERS.size()] {};
    bool csr_highlighted[machine::CSR::REGISTERS.size()] {};
    bool csr_highlighted_any;

    QPalette pal_normal;
    QPalette pal_updated;
    QPalette pal_read;

    void labelVal(QLabel *label, uint64_t val);
};

#endif // CSRDOCK_H
