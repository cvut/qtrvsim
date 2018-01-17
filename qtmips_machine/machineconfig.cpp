#include "machineconfig.h"

using namespace machine;

//////////////////////////////////////////////////////////////////////////////
/// Default config of MachineConfig
#define DF_PIPELINE false
#define DF_DELAYSLOT true
#define DF_HUNIT HU_STALL_FORWARD
#define DF_EXEC_PROTEC false
#define DF_WRITE_PROTEC false
#define DF_MEM_ACC_READ 10
#define DF_MEM_ACC_WRITE 10
#define DF_ELF QString("")
//////////////////////////////////////////////////////////////////////////////
/// Default config of MachineConfigCache
#define DFC_EN false
//////////////////////////////////////////////////////////////////////////////

MachineConfigCache::MachineConfigCache() {
    en = DFC_EN;
}

MachineConfigCache::MachineConfigCache(const MachineConfigCache *cc) {
    en = cc->enabled();
}

#define N(STR) (prefix + QString(STR))

MachineConfigCache::MachineConfigCache(const QSettings *sts, const QString &prefix) {
    en = sts->value(N("Enabled"), DFC_EN).toBool();
}

void MachineConfigCache::store(QSettings *sts, const QString &prefix) {
    sts->setValue(N("Enabled"), en);
}

#undef N

void MachineConfigCache::preset(enum ConfigPresets p) {
    switch (p) {
    case CP_SINGLE:
        set_enabled(false);
        break;
    case CP_PIPE_WITH_CACHE:
        set_enabled(false);
        break;
    }
}

void MachineConfigCache::set_enabled(bool v) {
    en = v;
}

bool MachineConfigCache::enabled() const {
    return en;
}

bool MachineConfigCache::operator==(const MachineConfigCache &c) const {
#define CMP(GETTER) (GETTER)() == (c.GETTER)()
    return CMP(enabled);
#undef CMP
}

bool MachineConfigCache::operator!=(const MachineConfigCache &c) const {
    return !operator==(c);
}

MachineConfig::MachineConfig() {
    pipeline = DF_PIPELINE;
    delayslot = DF_DELAYSLOT;
    hunit = DF_HUNIT;
    exec_protect = DF_EXEC_PROTEC;
    write_protect = DF_WRITE_PROTEC;
    mem_acc_read = DF_MEM_ACC_READ;
    mem_acc_write = DF_MEM_ACC_WRITE;
    elf_path = DF_ELF;
    cch_program = MachineConfigCache();
    cch_data = MachineConfigCache();
}

MachineConfig::MachineConfig(const MachineConfig *cc) {
    pipeline = cc->pipelined();
    delayslot = cc->delay_slot();
    hunit = cc->hazard_unit();
    exec_protect = cc->memory_execute_protection();
    write_protect = cc->memory_write_protection();
    mem_acc_read = cc->memory_access_time_read();
    mem_acc_write = cc->memory_access_time_write();
    elf_path = cc->elf();
    cch_program = cc->cache_program();
    cch_data = cc->cache_data();
}

#define N(STR) (prefix + QString(STR))

MachineConfig::MachineConfig(const QSettings *sts, const QString &prefix) {
    pipeline = sts->value(N("Pipelined"), DF_PIPELINE).toBool();
    delayslot = sts->value(N("DelaySlot"), DF_DELAYSLOT).toBool();
    hunit = (enum HazardUnit)sts->value(N("HazardUnit"), DF_HUNIT).toUInt(); // TODO probably rather save as string
    exec_protect = sts->value(N("MemoryExecuteProtection"), DF_EXEC_PROTEC).toBool();
    write_protect = sts->value(N("MemoryWriteProtection"), DF_WRITE_PROTEC).toBool();
    mem_acc_read = sts->value(N("MemoryRead"), DF_MEM_ACC_READ).toUInt();
    mem_acc_write = sts->value(N("MemoryWrite"), DF_MEM_ACC_WRITE).toUInt();
    elf_path = sts->value(N("Elf"), DF_ELF).toString();
    cch_program = MachineConfigCache(sts, N("ProgramCache_"));
    cch_data = MachineConfigCache(sts, N("DataCache_"));
}

void MachineConfig::store(QSettings *sts, const QString &prefix) {
    sts->setValue(N("Pipelined"), pipelined());
    sts->setValue(N("DelaySlot"), delay_slot());
    sts->setValue(N("HazardUnit"), (unsigned)hazard_unit());
    sts->setValue(N("Elf"), elf_path);
    cch_program.store(sts, N("ProgramCache_"));
    cch_data.store(sts, N("DataCache_"));
}

#undef N

void MachineConfig::preset(enum ConfigPresets p) {
    // Note: we set just a minimal subset to get preset (preserving as much of hidden configuration as possible)
    switch (p) {
    case CP_SINGLE:
        set_pipelined(false);
        set_delay_slot(true);
        break;
    case CP_PIPE_WITH_CACHE:
        set_pipelined(true);
        set_hazard_unit(MachineConfig::HU_STALL_FORWARD);
        break;
    }
    // Some common configurations
    set_memory_execute_protection(DF_EXEC_PROTEC);
    set_memory_write_protection(DF_WRITE_PROTEC);
    set_memory_access_time_read(DF_MEM_ACC_READ);
    set_memory_access_time_write(DF_MEM_ACC_WRITE);

    access_cache_program()->preset(p);
    access_cache_data()->preset(p);
}

void MachineConfig::set_pipelined(bool v) {
    pipeline = v;
}

void MachineConfig::set_delay_slot(bool v) {
    delayslot = v;
}

void MachineConfig::set_hazard_unit(enum MachineConfig::HazardUnit hu)  {
    hunit = hu;
}

void MachineConfig::set_memory_execute_protection(bool v) {
    exec_protect = v;
}

void MachineConfig::set_memory_write_protection(bool v) {
    write_protect = v;
}

void MachineConfig::set_memory_access_time_read(unsigned v) {
    mem_acc_read = v;
}

void MachineConfig::set_memory_access_time_write(unsigned v) {
    mem_acc_write = v;
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

bool MachineConfig::pipelined() const {
    return pipeline;
}

bool MachineConfig::delay_slot() const {
    // Delay slot is always on when pipeline is enabled
    return pipeline || delayslot;
}

enum MachineConfig::HazardUnit MachineConfig::hazard_unit() const {
    // Hazard unit is always off when there is no pipeline
    return pipeline ? hunit : machine::MachineConfig::HU_NONE;
}

bool MachineConfig::memory_execute_protection() const {
    return exec_protect;
}

bool MachineConfig::memory_write_protection() const {
    return write_protect;
}

unsigned MachineConfig::memory_access_time_read() const {
    return mem_acc_read;
}

unsigned MachineConfig::memory_access_time_write() const {
    return mem_acc_write;
}

QString MachineConfig::elf() const {
    return elf_path;
}

const MachineConfigCache &MachineConfig::cache_program() const {
    return cch_program;
}

const MachineConfigCache &MachineConfig::cache_data() const {
    return cch_data;
}

MachineConfigCache *MachineConfig::access_cache_program() {
    return &cch_program;
}

MachineConfigCache *MachineConfig::access_cache_data() {
    return &cch_data;
}

bool MachineConfig::operator==(const MachineConfig &c) const {
#define CMP(GETTER) (GETTER)() == (c.GETTER)()
    return CMP(pipelined) && \
            CMP(delay_slot) && \
            CMP(hazard_unit) && \
            CMP(memory_execute_protection) && \
            CMP(memory_write_protection) && \
            CMP(memory_access_time_read) && \
            CMP(memory_access_time_write) && \
            CMP(elf) && \
            CMP(cache_program) && \
            CMP(cache_data);
#undef CMP
}

bool MachineConfig::operator!=(const MachineConfig &c) const {
    return !operator==(c);
}
