#ifndef MACHINECONFIG_H
#define MACHINECONFIG_H

#include <QString>

namespace machine {

class MachineConfig {
public:
    MachineConfig();
    MachineConfig(MachineConfig *cc);

    enum CacheType {
        CCT_NONE,
        CCT_ASSOCIATIVE,
        // TODO
    };

    // Configure if CPU is pipelined
    // In default disabled.
    void set_pipelined(bool);
    // Configure if we want to do jump prediction
    // In default enabled. When disabled it also automatically disables pipelining.
    void set_delay_slot(bool);
    // Configure cache type
    // In default CCT_NONE is used.
    void set_cache(enum CacheType);
    // Set path to source elf file. This has to be set before core is initialized.
    void set_elf(QString path);

    bool pipelined() const;
    bool delay_slot() const;
    enum CacheType cache() const;
    QString elf() const;

private:
    bool pipeline, delayslot;
    enum CacheType cache_type;
    QString elf_path;
};

}

#endif // MACHINECONFIG_H
