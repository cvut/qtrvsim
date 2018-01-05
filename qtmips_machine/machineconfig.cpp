#include "machineconfig.h"

using namespace machine;

MachineConfig::MachineConfig() {
    pipeline = false;
    delayslot = true;
    cache_type = CCT_NONE;
    elf_path = QString("");
}

MachineConfig::MachineConfig(const MachineConfig *cc) {
    pipeline = cc->pipelined();
    delayslot = cc->delay_slot();
    cache_type = cc->cache();
    elf_path = cc->elf();
}

void MachineConfig::set_pipelined(bool v) {
    pipeline = v;
}

void MachineConfig::set_delay_slot(bool v) {
    delayslot = v;
    if (!delayslot)
        pipeline = false;
}

void MachineConfig::set_cache(enum CacheType cc) {
    cache_type = cc;
}

void MachineConfig::set_elf(QString path) {
    elf_path = path;
}

bool MachineConfig::pipelined() const {
    return pipeline;
}

bool MachineConfig::delay_slot() const {
    return delayslot;
}

enum MachineConfig::CacheType MachineConfig::cache() const {
    return cache_type;
}

QString MachineConfig::elf() const {
    return elf_path;
}
