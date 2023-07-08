#ifndef PROGRAMDOCK_H
#define PROGRAMDOCK_H

#include "machine/machine.h"
#include "windows/peripherals/peripheralsview.h"

#include <QComboBox>
#include <QDockWidget>
#include <QLabel>

class ProgramDock : public QDockWidget {
    Q_OBJECT

    using Super = QDockWidget;

public:
    ProgramDock(QWidget *parent, QSettings *settings);

    void setup(machine::Machine *machine);

signals:
    void machine_setup(machine::Machine *machine);
    void jump_to_pc(machine::Address);
    void focus_addr(machine::Address);
    void focus_addr_with_save(machine::Address);
    void stage_addr_changed(uint stage, machine::Address addr);
    void request_update_all();
public slots:
    void set_follow_inst(int);
    void fetch_inst_addr(machine::Address addr);
    void decode_inst_addr(machine::Address addr);
    void execute_inst_addr(machine::Address addr);
    void memory_inst_addr(machine::Address addr);
    void writeback_inst_addr(machine::Address addr);
    void report_error(const QString &error);
    void update_pipeline_addrs(const machine::CoreState &p);

private:
    enum FollowSource {
        FOLLOWSRC_NONE,
        FOLLOWSRC_FETCH,
        FOLLOWSRC_DECODE,
        FOLLOWSRC_EXECUTE,
        FOLLOWSRC_MEMORY,
        FOLLOWSRC_WRITEBACK,
        FOLLOWSRC_COUNT,
    };

    void update_follow_position();

    enum FollowSource follow_source;
    machine::Address follow_addr[FOLLOWSRC_COUNT] {};
    QSettings *settings;
};

#endif // PROGRAMDOCK_H
