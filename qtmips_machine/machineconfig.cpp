#include "machineconfig.h"

using namespace machine;

MachineConfigCache::MachineConfigCache() {
    // TODO
}

MachineConfigCache::MachineConfigCache(const MachineConfigCache *cc) {
    // TODO
}

bool MachineConfigCache::operator==(const MachineConfigCache &c) const {
    // TODO
    return true;
}

bool MachineConfigCache::operator!=(const MachineConfigCache &c) const {
    return !operator==(c);
}

MachineConfig::MachineConfig() {
    pipeline = false;
    delayslot = true;
    hunit = HU_STALL_FORWARD;
}

MachineConfig::MachineConfig(const MachineConfig *cc) {
    pipeline = cc->pipelined();
    delayslot = cc->delay_slot();
    elf_path = cc->elf();
    cch_program = cc->cache_program();
    cch_data = cc->cache_data();
    hunit = cc->hazard_unit();
}

void MachineConfig::set_pipelined(bool v) {
    pipeline = v;
}

void MachineConfig::set_delay_slot(bool v) {
    delayslot = v;
    if (!delayslot)
        pipeline = false;
}

void MachineConfig::set_elf(QString path) {
    elf_path = path;
}

void MachineConfig::set_cache_program(const MachineConfigCache &c) {
    cch_program = c;
}

void MachineConfig::set_cache_data(const MachineConfigCache &c) {
    cch_data = c;
}

void MachineConfig::set_hazard_unit(enum MachineConfig::HazardUnit hu)  {
    hunit = hu;
}

bool MachineConfig::pipelined() const {
    return pipeline;
}

bool MachineConfig::delay_slot() const {
    return delayslot;
}

QString MachineConfig::elf() const {
    return elf_path;
}

MachineConfigCache MachineConfig::cache_program() const {
    return cch_program;
}

MachineConfigCache MachineConfig::cache_data() const {
    return cch_data;
}

enum MachineConfig::HazardUnit MachineConfig::hazard_unit() const {
    return hunit;
}

bool MachineConfig::operator==(const MachineConfig &c) const {
#define CMP(GETTER) (GETTER)() == (c.GETTER)()
    return CMP(pipelined) && \
            CMP(delay_slot) && \
            CMP(elf) && \
            CMP(cache_program) && \
            CMP(cache_data) && \
            CMP(hazard_unit);
#undef CMP
}

bool MachineConfig::operator!=(const MachineConfig &c) const {
    return !operator==(c);
}
