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

class RegistersDock final : public QDockWidget {
    Q_OBJECT
public:
    explicit RegistersDock(QWidget *parent);
    ~RegistersDock() override = default;

    void connectToMachine(machine::Machine *machine);

private slots:
    void pc_changed(machine::Address val);
    void gp_changed(machine::RegisterId i, machine::RegisterValue val);
    void gp_read(machine::RegisterId i, machine::RegisterValue val);
    void clear_highlights();

private:
    Box<QScrollArea> scroll_area;
    Box<StaticTable> table_widget;

    BORROWED QLabel *pc {};
    array<BORROWED QLabel *, machine::REGISTER_COUNT> gp {};

    bitset<machine::REGISTER_COUNT> gp_highlighted { false };

    QPalette pal_normal;
    QPalette pal_updated;
    QPalette pal_read;

private:
    static void setLabelHexValue(QLabel *label, uint32_t val);
    BORROWED QLabel *addRegisterLabel(const QString &title);
    QPalette createPalette(const QColor &color) const;
};

#endif // REGISTERSDOCK_H
