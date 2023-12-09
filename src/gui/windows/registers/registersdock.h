#ifndef REGISTERSDOCK_H
#define REGISTERSDOCK_H

#include "machine/machine.h"
#include "statictable.h"

#include <QDockWidget>
#include <QLabel>
#include <QPalette>
#include <QScrollArea>
#include <bitset>

using std::array;
using std::bitset;

/**
 * NOTE: RV64 ready
 */
class RegistersDock final : public QDockWidget {
    Q_OBJECT
public:
    explicit RegistersDock(QWidget *parent, machine::Xlen xlen);

    void connectToMachine(machine::Machine *machine);

private slots:
    void pc_changed(machine::Address val);
    void gp_changed(machine::RegisterId i, machine::RegisterValue val);
    void gp_read(machine::RegisterId i, machine::RegisterValue val);
    void clear_highlights();

private:
    machine::Xlen xlen;

    const char *sizeHintText();

    Box<QScrollArea> scroll_area;
    Box<StaticTable> table_widget;

    BORROWED QLabel *pc {};
    array<BORROWED QLabel *, machine::REGISTER_COUNT> gp {};

    bitset<machine::REGISTER_COUNT> gp_highlighted { false };

    QPalette pal_normal;
    QPalette pal_updated;
    QPalette pal_read;

private:
    void setRegisterValueToLabel(QLabel *label, machine::RegisterValue value);
    BORROWED QLabel *addRegisterLabel(const QString &title);
    [[nodiscard]] QPalette createPalette(const QColor &color) const;
};

#endif // REGISTERSDOCK_H
