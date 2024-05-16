#ifndef MACHINE_H
#define MACHINE_H

#include "core.h"
#include "machineconfig.h"
#include "memory/backend/lcddisplay.h"
#include "memory/backend/peripheral.h"
#include "memory/backend/peripspiled.h"
#include "memory/backend/serialport.h"
#include "memory/backend/aclintmtimer.h"
#include "memory/backend/aclintmswi.h"
#include "memory/backend/aclintsswi.h"
#include "memory/cache/cache.h"
#include "memory/memory_bus.h"
#include "predictor.h"
#include "registers.h"
#include "simulator_exception.h"
#include "symboltable.h"

#include <QObject>
#include <QTimer>
#include <cstdint>

namespace machine {

class Machine : public QObject {
    Q_OBJECT
public:
    explicit Machine(MachineConfig config, bool load_symtab = false, bool load_executable = true);
    ~Machine() override;

    const MachineConfig &config();
    void set_speed(unsigned int ips, unsigned int time_chunk = 0);

    const Registers *registers();
    const CSR::ControlState *control_state();
    const Memory *memory();
    Memory *memory_rw();
    const Cache *cache_program();
    const Cache *cache_data();
    const Cache *cache_level2();
    Cache *cache_data_rw();
    void cache_sync();
    const MemoryDataBus *memory_data_bus();
    MemoryDataBus *memory_data_bus_rw();
    SerialPort *serial_port();
    PeripSpiLed *peripheral_spi_led();
    LcdDisplay *peripheral_lcd_display();
    const SymbolTable *symbol_table(bool create = false);
    SymbolTable *symbol_table_rw(bool create = false);
    void set_symbol(
        const QString &name,
        uint32_t value,
        uint32_t size,
        unsigned char info = 0,
        unsigned char other = 0);
    const Core *core();
    const CoreSingle *core_singe();
    const CorePipelined *core_pipelined();
    bool executable_loaded() const;

    enum Status {
        ST_READY,   // Machine is ready to be started or step to be called
        ST_RUNNING, // Machine is running
        ST_BUSY,    // Machine is calculating step
        ST_EXIT,    // Machine exited
        ST_TRAPPED  // Machine exited with failure
    };
    enum Status status();
    bool exited();

    void register_exception_handler(ExceptionCause excause, ExceptionHandler *exhandler);
    bool memory_bus_insert_range(
        BackendMemory *mem_acces,
        Address start_addr,
        Address last_addr,
        bool move_ownership);

    void insert_hwbreak(Address address);
    void remove_hwbreak(Address address);
    bool is_hwbreak(Address address);
    void set_stop_on_exception(enum ExceptionCause excause, bool value);
    bool get_stop_on_exception(enum ExceptionCause excause) const;
    void set_step_over_exception(enum ExceptionCause excause, bool value);
    bool get_step_over_exception(enum ExceptionCause excause) const;
    enum ExceptionCause get_exception_cause() const;

public slots:
    void play();
    void pause();
    void step();
    void restart();

signals:
    void program_exit();
    void program_trap(machine::SimulatorException &e);
    void status_change(enum machine::Machine::Status st);
    void tick();      // Time tick
    void post_tick(); // Emitted after tick to allow updates
    void set_interrupt_signal(uint irq_num, bool active);

private slots:
    void step_timer();

private:
    void step_internal(bool skip_break = false);
    MachineConfig machine_config;

    Registers *regs = nullptr;
    Memory *mem = nullptr;
    /**
     * Memory with loaded program only.
     * It is not used for execution, only for quick
     * simulation reset without repeated ELF file loading.
     */
    Memory *mem_program_only = nullptr;
    MemoryDataBus *data_bus = nullptr;
    SerialPort *ser_port = nullptr;
    PeripSpiLed *perip_spi_led = nullptr;
    LcdDisplay *perip_lcd_display = nullptr;
    aclint::AclintMtimer *aclint_mtimer = nullptr;
    aclint::AclintMswi *aclint_mswi = nullptr;
    aclint::AclintSswi *aclint_sswi = nullptr;
    Cache *cch_program = nullptr;
    Cache *cch_data = nullptr;
    Cache *cch_level2 = nullptr;
    CSR::ControlState *controlst = nullptr;
    BranchPredictor *predictor = nullptr;
    Core *cr = nullptr;

    QTimer *run_t = nullptr;
    unsigned int time_chunk = { 0 };

    SymbolTable *symtab = nullptr;
    Address program_end = 0xffff0000_addr;
    enum Status stat = ST_READY;
    void set_status(enum Status st);
    void setup_serial_port();
    void setup_perip_spi_led();
    void setup_lcd_display();
    void setup_aclint_mtime();
    void setup_aclint_mswi();
    void setup_aclint_sswi();
};

} // namespace machine

#endif // MACHINE_H
