#ifndef MACHINECONFIG_H
#define MACHINECONFIG_H

#include <QString>

namespace machine {

class MachineConfigCache {
public:
    MachineConfigCache();
    MachineConfigCache(const MachineConfigCache *cc);

    // TODO

    bool operator ==(const MachineConfigCache &c) const;
    bool operator !=(const MachineConfigCache &c) const;

private:
    // TODO
};

class MachineConfig {
public:
    MachineConfig();
    MachineConfig(const MachineConfig *cc);

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
    // Set path to source elf file. This has to be set before core is initialized.
    void set_elf(QString path);
    // Configure cache
    void set_cache_program(const MachineConfigCache&);
    void set_cache_data(const MachineConfigCache&);
    // Hazard unit
    void set_hazard_unit(enum HazardUnit);

    bool pipelined() const;
    bool delay_slot() const;
    QString elf() const;
    MachineConfigCache cache_program() const;
    MachineConfigCache cache_data() const;
    enum HazardUnit hazard_unit() const;

    bool operator ==(const MachineConfig &c) const;
    bool operator !=(const MachineConfig &c) const;

private:
    bool pipeline, delayslot;
    QString elf_path;
    MachineConfigCache cch_program, cch_data;
    enum HazardUnit hunit;
};

}

#endif // MACHINECONFIG_H
