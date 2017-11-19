#ifndef MACHINECONFIG_H
#define MACHINECONFIG_H

#include <QString>

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
    // In default disabled. When enabled it also automatically enables pipelining
    void set_jump_prediction(bool);
    // Configure cache type
    // In default CCT_NONE is used.
    void set_cache(enum CacheType);
    // Set path to source elf file. This has to be set before core is initialized.
    void set_elf(QString path);

    bool pipelined() const;
    bool jump_prediction() const;
    enum CacheType cache() const;
    QString elf() const;

private:
    bool pipeline, jumppred;
    enum CacheType cache_type;
    QString elf_path;
};

#endif // MACHINECONFIG_H
