#include "machineconfig.h"

MachineConfig::MachineConfig() {
    pipeline = false;
    jumppred = false;
}

MachineConfig::MachineConfig(MachineConfig *cc) {
    pipeline = cc->pipelined();
    jumppred = cc->jump_prediction();
}

void MachineConfig::set_pipelined(bool v) {
    pipeline = v;
}

void MachineConfig::set_jump_prediction(bool v) {
    jumppred = v;
    if (jumppred)
        pipeline = true;
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

bool MachineConfig::jump_prediction() const {
    return jumppred;
}

enum MachineConfig::CacheType MachineConfig::cache() const {
    return cache_type;
}

QString MachineConfig::elf() const {
    return elf_path;
}
