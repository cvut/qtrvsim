#ifndef MACHINECONFIG_H
#define MACHINECONFIG_H

#include <QString>
#include <QSettings>

namespace machine {

enum ConfigPresets {
    CP_SINGLE, // No pipeline cpu without cache
    CP_PIPE_NO_HAZARD, // Pipelined cpu without hazard unit
    CP_PIPE_NO_CACHE, // Pipelined cpu without cache
    CP_PIPE_CACHE // Full pipelined cpu
};

class MachineConfigCache {
public:
    MachineConfigCache();
    MachineConfigCache(const MachineConfigCache *cc);
    MachineConfigCache(const QSettings*, const QString &prefix = "");

    void store(QSettings*, const QString &prefix = "");

    void preset(enum ConfigPresets);

    enum ReplacementPolicy {
        RP_RAND, // Random
        RP_LRU, // Least recently used
        RP_LFU, // Least frequently used
        RP_ARC // Adaptive replacement cache
    };

    enum WritePolicy {
        WP_TROUGH, // Write trough
        WP_BACK // Write back
    };

    // If cache should be used or not
    void set_enabled(bool);
    void set_sets(unsigned); // Number of sets bits used in cache
    void set_blocks(unsigned); // Number of blocks
    void set_associativity(unsigned); // Degree of associativity
    void set_replacement_policy(enum ReplacementPolicy);
    void set_write_policy(enum WritePolicy);

    bool enabled() const;
    unsigned sets() const;
    unsigned blocks() const;
    unsigned associativity() const;
    enum ReplacementPolicy replacement_policy() const;
    enum WritePolicy write_policy() const;

    bool operator ==(const MachineConfigCache &c) const;
    bool operator !=(const MachineConfigCache &c) const;

private:
    bool en;
    unsigned n_sets, n_blocks, d_associativity;
    enum ReplacementPolicy replac_pol;
    enum WritePolicy write_pol;
};

class MachineConfig {
public:
    MachineConfig();
    MachineConfig(const MachineConfig *cc);
    MachineConfig(const QSettings*, const QString &prefix = "");

    void store(QSettings*, const QString &prefix = "");

    void preset(enum ConfigPresets);

    enum HazardUnit {
        HU_NONE,
        HU_STALL,
        HU_STALL_FORWARD
    };

    // Configure if CPU is pipelined
    // In default disabled.
    void set_pipelined(bool);
    // Configure if cpu should simulate delay slot in non-pipelined core
    // In default enabled. When disabled it also automatically disables pipelining.
    void set_delay_slot(bool);
    // Hazard unit
    void set_hazard_unit(enum HazardUnit);
    // Protect data memory from execution. Only program sections can be executed.
    void set_memory_execute_protection(bool);
    // Protect program memory from accidental writes.
    void set_memory_write_protection(bool);
    // Set memory access times. Passed value is in cycles.
    void set_memory_access_time_read(unsigned);
    void set_memory_access_time_write(unsigned);
    // Set path to source elf file. This has to be set before core is initialized.
    void set_elf(QString path);
    // Configure cache
    void set_cache_program(const MachineConfigCache&);
    void set_cache_data(const MachineConfigCache&);

    bool pipelined() const;
    bool delay_slot() const;
    enum HazardUnit hazard_unit() const;
    bool memory_execute_protection() const;
    bool memory_write_protection() const;
    unsigned memory_access_time_read() const;
    unsigned memory_access_time_write() const;
    QString elf() const;
    const MachineConfigCache &cache_program() const;
    const MachineConfigCache &cache_data() const;

    MachineConfigCache *access_cache_program();
    MachineConfigCache *access_cache_data();

    bool operator ==(const MachineConfig &c) const;
    bool operator !=(const MachineConfig &c) const;

private:
    bool pipeline, delayslot;
    enum HazardUnit hunit;
    bool exec_protect, write_protect;
    unsigned mem_acc_read, mem_acc_write;
    QString elf_path;
    MachineConfigCache cch_program, cch_data;
};

}

#endif // MACHINECONFIG_H
