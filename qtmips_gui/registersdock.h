#ifndef REGISTERSDOCK_H
#define REGISTERSDOCK_H

#include <QDockWidget>
#include <QLabel>
#include <QFormLayout>
#include <QScrollArea>
#include <QPropertyAnimation>
#include "qtmipsmachine.h"

class RegistersDock : public QDockWidget {
    Q_OBJECT
public:
    RegistersDock(QWidget *parent);
    ~RegistersDock();

    void setup(QtMipsMachine *machine);

private slots:
    void pc_changed(std::uint32_t val);
    void gp_changed(std::uint8_t i, std::uint32_t val);
    void hi_lo_changed(bool hi, std::uint32_t val);

private:
    const Registers *regs;

    QScrollArea *widg;
    QFormLayout *layout;

    QLabel *pc;
    QLabel *hi;
    QLabel *lo;
    QLabel *gp[32];

    void labelVal(QLabel *label, std::uint32_t val, bool dec);
};

#endif // REGISTERSDOCK_H
