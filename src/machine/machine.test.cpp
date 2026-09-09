#include "machine.h"
#include "memory/memory_utils.h"

#include <QtTest>

using namespace machine;

class TestMachine : public QObject {
    Q_OBJECT

private slots:
    void cycle_limit();
    void instruction_limit();
    void program_exit();
    void restart_resets_execution_state();
};

static void write_nops(Machine &machine, unsigned count = 16) {
    const uint64_t base = machine.registers()->read_pc().get_raw();
    for (unsigned i = 0; i < count; ++i) {
        memory_write_u32(machine.memory_rw(), base + uint64_t(i * 4), 0x00000013);
    }
}

static void fail_on_trap(Machine &machine, QString &trap_message) {
    QObject::connect(
        &machine, &Machine::program_trap, &machine,
        [&](SimulatorException &exception) { trap_message = exception.msg(false); });
}

void TestMachine::cycle_limit() {
    Machine machine(MachineConfig(), false, false);
    QString trap_message;
    fail_on_trap(machine, trap_message);
    write_nops(machine);
    machine.set_cycle_limit(3);

    unsigned limit_signals = 0;
    Machine::StopReason signal_reason = Machine::SR_NONE;
    connect(
        &machine, &Machine::execution_limit_reached, this,
        [&](Machine::StopReason reason) {
            ++limit_signals;
            signal_reason = reason;
        });

    machine.step();
    machine.step();
    machine.step();

    QVERIFY2(machine.status() != Machine::ST_TRAPPED, qPrintable(trap_message));
    QCOMPARE(machine.status(), Machine::ST_EXIT);
    QCOMPARE(machine.stop_reason(), Machine::SR_CYCLE_LIMIT);
    QCOMPARE(signal_reason, Machine::SR_CYCLE_LIMIT);
    QCOMPARE(limit_signals, 1U);
    QCOMPARE(machine.core()->get_cycle_count(), UINT64_C(3));
    QCOMPARE(machine.core()->get_instruction_count(), UINT64_C(3));
}

void TestMachine::instruction_limit() {
    Machine machine(MachineConfig(), false, false);
    QString trap_message;
    fail_on_trap(machine, trap_message);
    write_nops(machine);
    machine.set_instruction_limit(2);

    machine.step();
    machine.step();

    QVERIFY2(machine.status() != Machine::ST_TRAPPED, qPrintable(trap_message));
    QCOMPARE(machine.status(), Machine::ST_EXIT);
    QCOMPARE(machine.stop_reason(), Machine::SR_INSTRUCTION_LIMIT);
    QCOMPARE(machine.core()->get_cycle_count(), UINT64_C(2));
    QCOMPARE(machine.core()->get_instruction_count(), UINT64_C(2));
}

void TestMachine::program_exit() {
    Machine machine(MachineConfig(), false, false);
    unsigned exit_signals = 0;
    connect(&machine, &Machine::program_exit, this, [&]() { ++exit_signals; });

    machine.terminate_program(7);

    QCOMPARE(machine.status(), Machine::ST_EXIT);
    QCOMPARE(machine.stop_reason(), Machine::SR_PROGRAM_EXIT);
    QCOMPARE(machine.exit_code(), 7);
    QCOMPARE(exit_signals, 1U);
}

void TestMachine::restart_resets_execution_state() {
    Machine machine(MachineConfig(), false, false);
    QString trap_message;
    fail_on_trap(machine, trap_message);
    write_nops(machine);
    machine.set_cycle_limit(1);
    machine.step();
    QVERIFY2(machine.status() != Machine::ST_TRAPPED, qPrintable(trap_message));
    QCOMPARE(machine.status(), Machine::ST_EXIT);

    machine.restart();

    QCOMPARE(machine.status(), Machine::ST_READY);
    QCOMPARE(machine.stop_reason(), Machine::SR_NONE);
    QCOMPARE(machine.exit_code(), 0);
    QCOMPARE(machine.core()->get_cycle_count(), UINT64_C(0));
    QCOMPARE(machine.core()->get_instruction_count(), UINT64_C(0));
    QCOMPARE(machine.core()->get_stall_count(), UINT64_C(0));
}

QTEST_APPLESS_MAIN(TestMachine)

#include "machine.test.moc"
