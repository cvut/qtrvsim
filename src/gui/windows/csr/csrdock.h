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
    explicit CsrDock(QWidget *parent);

    void setup(machine::Machine *machine);
    void reload();

private slots:
    void csr_changed(std::size_t internal_reg_id, machine::RegisterValue val);
    void csr_read(std::size_t internal_reg_id, machine::RegisterValue val);
    void clear_highlights();

private:
    void showEvent(QShowEvent *event) override;

private:
    machine::Xlen xlen;
    // We keep this handle for batch updates when this widget was hidden.
    const machine::CSR::ControlState *csr_handle;

    const char *sizeHintText();

    QT_OWNED StaticTable *widg;
    QT_OWNED QScrollArea *scrollarea;

    std::array<QT_OWNED QLabel *, machine::CSR::REGISTERS.size()> csr_view {};
    bool csr_highlighted[machine::CSR::REGISTERS.size()] {};
    bool csr_highlighted_any;

    QPalette pal_normal;
    QPalette pal_updated;
    QPalette pal_read;

    static void labelVal(QLabel *label, uint64_t val);
};

#endif // CSRDOCK_H
