#include "machineconfig.h"

#include "common/endian.h"

#include <QMap>
#include <utility>

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
#define DF_MEM_ACC_BURST 0
#define DF_MEM_ACC_LEVEL2 2
#define DF_MEM_ACC_BURST_ENABLE false
#define DF_ELF QString("")
/// Default config of branch predictor
#define DFC_BP_ENABLED false
#define DFC_BP_TYPE PredictorType::SMITH_1_BIT
#define DFC_BP_INIT_STATE PredictorState::NOT_TAKEN
#define DFC_BP_BTB_BITS 2
#define DFC_BP_BHR_BITS 0
#define DFC_BP_BHT_ADDR_BITS 2
//////////////////////////////////////////////////////////////////////////////
/// Default config of CacheConfig
#define DFC_EN false
#define DFC_SETS 1
#define DFC_BLOCKS 1
#define DFC_ASSOC 1
#define DFC_REPLAC RP_RAND
#define DFC_WRITE WP_THROUGH_NOALLOC
//////////////////////////////////////////////////////////////////////////////

CacheConfig::CacheConfig() {
    en = DFC_EN;
    n_sets = DFC_SETS;
    n_blocks = DFC_BLOCKS;
    d_associativity = DFC_ASSOC;
    replac_pol = DFC_REPLAC;
    write_pol = DFC_WRITE;
}

CacheConfig::CacheConfig(const CacheConfig *cc) {
    en = cc->enabled();
    n_sets = cc->set_count();
    n_blocks = cc->block_size();
    d_associativity = cc->associativity();
    replac_pol = cc->replacement_policy();
    write_pol = cc->write_policy();
}

#define N(STR) (prefix + QString(STR))

CacheConfig::CacheConfig(const QSettings *sts, const QString &prefix) {
    en = sts->value(N("Enabled"), DFC_EN).toBool();
    n_sets = sts->value(N("Sets"), DFC_SETS).toUInt();
    n_blocks = sts->value(N("Blocks"), DFC_BLOCKS).toUInt();
    d_associativity = sts->value(N("Associativity"), DFC_ASSOC).toUInt();
    replac_pol = (enum ReplacementPolicy)sts->value(N("Replacement"), DFC_REPLAC).toUInt();
    write_pol = (enum WritePolicy)sts->value(N("Write"), DFC_WRITE).toUInt();
}

void CacheConfig::store(QSettings *sts, const QString &prefix) const {
    sts->setValue(N("Enabled"), enabled());
    sts->setValue(N("Sets"), set_count());
    sts->setValue(N("Blocks"), block_size());
    sts->setValue(N("Associativity"), associativity());
    sts->setValue(N("Replacement"), (unsigned)replacement_policy());
    sts->setValue(N("Write"), (unsigned)write_policy());
}

#undef N

void CacheConfig::preset(enum ConfigPresets p) {
    switch (p) {
    case CP_PIPE:
    case CP_SINGLE_CACHE:
        set_enabled(true);
        set_set_count(4);
        set_block_size(2);
        set_associativity(2);
        set_replacement_policy(RP_RAND);
        set_write_policy(WP_THROUGH_NOALLOC);
        break;
    case CP_SINGLE:
    case CP_PIPE_NO_HAZARD: set_enabled(false);
    }
}

void CacheConfig::set_enabled(bool v) {
    en = v;
}

void CacheConfig::set_set_count(unsigned v) {
    n_sets = v > 0 ? v : 1;
}

void CacheConfig::set_block_size(unsigned v) {
    n_blocks = v > 0 ? v : 1;
}

void CacheConfig::set_associativity(unsigned v) {
    d_associativity = v > 0 ? v : 1;
}

void CacheConfig::set_replacement_policy(enum ReplacementPolicy v) {
    replac_pol = v;
}

void CacheConfig::set_write_policy(enum WritePolicy v) {
    write_pol = v;
}

bool CacheConfig::enabled() const {
    return en;
}

unsigned CacheConfig::set_count() const {
    return n_sets;
}

unsigned CacheConfig::block_size() const {
    return n_blocks;
}

unsigned CacheConfig::associativity() const {
    return d_associativity;
}

enum CacheConfig::ReplacementPolicy CacheConfig::replacement_policy() const {
    return replac_pol;
}

enum CacheConfig::WritePolicy CacheConfig::write_policy() const {
    return write_pol;
}

bool CacheConfig::operator==(const CacheConfig &c) const {
#define CMP(GETTER) (GETTER)() == (c.GETTER)()
    return CMP(enabled) && CMP(set_count) && CMP(block_size) && CMP(associativity)
           && CMP(replacement_policy) && CMP(write_policy);
#undef CMP
}

bool CacheConfig::operator!=(const CacheConfig &c) const {
    return !operator==(c);
}

MachineConfig::MachineConfig() {
    simulated_endian = LITTLE;
    simulated_xlen = Xlen::_32;
    isa_word = config_isa_word_default;
    pipeline = DF_PIPELINE;
    delayslot = DF_DELAYSLOT;
    hunit = DF_HUNIT;
    exec_protect = DF_EXEC_PROTEC;
    write_protect = DF_WRITE_PROTEC;
    mem_acc_read = DF_MEM_ACC_READ;
    mem_acc_write = DF_MEM_ACC_WRITE;
    mem_acc_burst = DF_MEM_ACC_BURST;
    mem_acc_level2 = DF_MEM_ACC_LEVEL2;
    mem_acc_enable_burst = DF_MEM_ACC_BURST_ENABLE;
    osem_enable = true;
    osem_known_syscall_stop = true;
    osem_unknown_syscall_stop = true;
    osem_interrupt_stop = true;
    osem_exception_stop = true;
    osem_fs_root = "";
    res_at_compile = true;
    elf_path = DF_ELF;
    cch_program = CacheConfig();
    cch_data = CacheConfig();
    cch_level2 = CacheConfig();

    // Branch predictor
    bp_enabled = DFC_BP_ENABLED;
    bp_type = DFC_BP_TYPE;
    bp_init_state = DFC_BP_INIT_STATE;
    bp_btb_bits = DFC_BP_BTB_BITS;
    bp_bhr_bits = DFC_BP_BHR_BITS;
    bp_bht_addr_bits = DFC_BP_BHT_ADDR_BITS;
    bp_bht_bits = bp_bhr_bits + bp_bht_addr_bits;
}

MachineConfig::MachineConfig(const MachineConfig *config) {
    simulated_endian = config->get_simulated_endian();
    simulated_xlen = config->get_simulated_xlen();
    isa_word = config->get_isa_word();
    pipeline = config->pipelined();
    delayslot = config->delay_slot();
    hunit = config->hazard_unit();
    exec_protect = config->memory_execute_protection();
    write_protect = config->memory_write_protection();
    mem_acc_read = config->memory_access_time_read();
    mem_acc_write = config->memory_access_time_write();
    mem_acc_burst = config->memory_access_time_burst();
    mem_acc_level2 = config->memory_access_time_level2();
    mem_acc_enable_burst = config->memory_access_enable_burst();
    osem_enable = config->osemu_enable();
    osem_known_syscall_stop = config->osemu_known_syscall_stop();
    osem_unknown_syscall_stop = config->osemu_unknown_syscall_stop();
    osem_interrupt_stop = config->osemu_interrupt_stop();
    osem_exception_stop = config->osemu_exception_stop();
    osem_fs_root = config->osemu_fs_root();
    res_at_compile = config->reset_at_compile();
    elf_path = config->elf();
    cch_program = config->cache_program();
    cch_data = config->cache_data();
    cch_level2 = config->cache_level2();

    // Branch predictor
    bp_enabled = config->get_bp_enabled();
    bp_type = config->get_bp_type();
    bp_init_state = config->get_bp_init_state();
    bp_btb_bits = config->get_bp_btb_bits();
    bp_bhr_bits = config->get_bp_bhr_bits();
    bp_bht_addr_bits = config->get_bp_bht_addr_bits();
    bp_bht_bits = bp_bhr_bits + bp_bht_addr_bits;
}

#define N(STR) (prefix + QString(STR))

MachineConfig::MachineConfig(const QSettings *sts, const QString &prefix) {
    simulated_endian = LITTLE;
    unsigned int xlen_num_bits = sts->value(N("XlenBits"), 32).toUInt();
    simulated_xlen = xlen_num_bits == 64 ? Xlen::_64 : Xlen::_32;
    isa_word
        = ConfigIsaWord(sts->value(N("IsaWord"), config_isa_word_default.toUnsigned()).toUInt());
    isa_word |= config_isa_word_default & config_isa_word_fixed;
    isa_word &= config_isa_word_default | ~config_isa_word_fixed;
    pipeline = sts->value(N("Pipelined"), DF_PIPELINE).toBool();
    delayslot = sts->value(N("DelaySlot"), DF_DELAYSLOT).toBool();
    hunit = (enum HazardUnit)sts->value(N("HazardUnit"), DF_HUNIT).toUInt();
    exec_protect = sts->value(N("MemoryExecuteProtection"), DF_EXEC_PROTEC).toBool();
    write_protect = sts->value(N("MemoryWriteProtection"), DF_WRITE_PROTEC).toBool();
    mem_acc_read = sts->value(N("MemoryRead"), DF_MEM_ACC_READ).toUInt();
    mem_acc_write = sts->value(N("MemoryWrite"), DF_MEM_ACC_WRITE).toUInt();
    mem_acc_burst = sts->value(N("MemoryBurst"), DF_MEM_ACC_BURST).toUInt();
    mem_acc_level2 = sts->value(N("MemoryLevel2"), DF_MEM_ACC_LEVEL2).toUInt();
    mem_acc_enable_burst = sts->value(N("MemoryBurstEnable"), DF_MEM_ACC_BURST_ENABLE).toBool();
    osem_enable = sts->value(N("OsemuEnable"), true).toBool();
    osem_known_syscall_stop = sts->value(N("OsemuKnownSyscallStop"), true).toBool();
    osem_unknown_syscall_stop = sts->value(N("OsemuUnknownSyscallStop"), true).toBool();
    osem_interrupt_stop = sts->value(N("OsemuInterruptStop"), true).toBool();
    osem_exception_stop = sts->value(N("OsemuExceptionStop"), true).toBool();
    osem_fs_root = sts->value(N("OsemuFilesystemRoot"), "").toString();
    res_at_compile = sts->value(N("ResetAtCompile"), true).toBool();
    elf_path = sts->value(N("Elf"), DF_ELF).toString();
    cch_program = CacheConfig(sts, N("ProgramCache_"));
    cch_data = CacheConfig(sts, N("DataCache_"));
    cch_level2 = CacheConfig(sts, N("Level2Cache_"));

    // Branch predictor
    bp_enabled = sts->value(N("BranchPredictor_Enabled"), DFC_BP_ENABLED).toBool();
    bp_type = (PredictorType)sts->value(N("BranchPredictor_Type"), (uint8_t)DFC_BP_TYPE).toUInt();
    bp_init_state
        = (PredictorState)sts->value(N("BranchPredictor_InitState"), (uint8_t)DFC_BP_INIT_STATE)
              .toUInt();
    bp_btb_bits = sts->value(N("BranchPredictor_BitsBTB"), DFC_BP_BTB_BITS).toUInt();
    bp_bhr_bits = sts->value(N("BranchPredictor_BitsBHR"), DFC_BP_BHR_BITS).toUInt();
    bp_bht_addr_bits = sts->value(N("BranchPredictor_BitsBHTAddr"), DFC_BP_BHT_ADDR_BITS).toUInt();
    bp_bht_bits = bp_bhr_bits + bp_bht_addr_bits;
}

void MachineConfig::store(QSettings *sts, const QString &prefix) {
    sts->setValue(N("XlenBits"), get_simulated_xlen() == Xlen::_64 ? 64 : 32);
    sts->setValue(N("IsaWord"), get_isa_word().toUnsigned());
    sts->setValue(N("Pipelined"), pipelined());
    sts->setValue(N("DelaySlot"), delay_slot());
    sts->setValue(N("HazardUnit"), (unsigned)hazard_unit());
    sts->setValue(N("MemoryRead"), memory_access_time_read());
    sts->setValue(N("MemoryWrite"), memory_access_time_write());
    sts->setValue(N("MemoryBurst"), memory_access_time_burst());
    sts->setValue(N("MemoryLevel2"), memory_access_time_level2());
    sts->setValue(N("MemoryBurstEnable"), memory_access_enable_burst());
    sts->setValue(N("OsemuEnable"), osemu_enable());
    sts->setValue(N("OsemuKnownSyscallStop"), osemu_known_syscall_stop());
    sts->setValue(N("OsemuUnknownSyscallStop"), osemu_unknown_syscall_stop());
    sts->setValue(N("OsemuInterruptStop"), osemu_interrupt_stop());
    sts->setValue(N("OsemuExceptionStop"), osemu_exception_stop());
    sts->setValue(N("OsemuFilesystemRoot"), osemu_fs_root());
    sts->setValue(N("ResetAtCompile"), reset_at_compile());
    sts->setValue(N("Elf"), elf_path);
    cch_program.store(sts, N("ProgramCache_"));
    cch_data.store(sts, N("DataCache_"));
    cch_level2.store(sts, N("Level2Cache_"));

    // Branch predictor
    sts->setValue(N("BranchPredictor_Enabled"), get_bp_enabled());
    sts->setValue(N("BranchPredictor_Type"), (uint8_t)get_bp_type());
    sts->setValue(N("BranchPredictor_InitState"), (uint8_t)get_bp_init_state());
    sts->setValue(N("BranchPredictor_BitsBTB"), get_bp_btb_bits());
    sts->setValue(N("BranchPredictor_BitsBHR"), get_bp_bhr_bits());
    sts->setValue(N("BranchPredictor_BitsBHTAddr"), get_bp_bht_addr_bits());
}

#undef N

void MachineConfig::preset(enum ConfigPresets p) {
    // Note: we set just a minimal subset to get preset (preserving as much of
    // hidden configuration as possible)
    switch (p) {
    case CP_SINGLE:
    case CP_SINGLE_CACHE:
        set_pipelined(false);
        set_delay_slot(true);
        break;
    case CP_PIPE_NO_HAZARD:
        set_pipelined(true);
        set_hazard_unit(MachineConfig::HU_NONE);
        break;
    case CP_PIPE:
        set_pipelined(true);
        set_hazard_unit(MachineConfig::HU_STALL_FORWARD);
        break;
    }
    // Some common configurations
    set_memory_execute_protection(DF_EXEC_PROTEC);
    set_memory_write_protection(DF_WRITE_PROTEC);
    set_memory_access_time_read(DF_MEM_ACC_READ);
    set_memory_access_time_write(DF_MEM_ACC_WRITE);
    set_memory_access_time_burst(DF_MEM_ACC_BURST);
    set_memory_access_time_level2(DF_MEM_ACC_LEVEL2);
    set_memory_access_enable_burst(DF_MEM_ACC_BURST_ENABLE);

    // Branch predictor
    set_bp_enabled(DFC_BP_ENABLED);
    set_bp_type(DFC_BP_TYPE);
    set_bp_init_state(DFC_BP_INIT_STATE);
    set_bp_btb_bits(DFC_BP_BTB_BITS);
    set_bp_bhr_bits(DFC_BP_BHR_BITS);
    set_bp_bht_addr_bits(DFC_BP_BHT_ADDR_BITS);

    access_cache_program()->preset(p);
    access_cache_data()->preset(p);
    access_cache_level2()->preset(p);

    set_simulated_xlen(Xlen::_32);
    set_isa_word(config_isa_word_default);

    switch (p) {
    case CP_SINGLE:
    case CP_SINGLE_CACHE:
    case CP_PIPE_NO_HAZARD:
    case CP_PIPE: access_cache_level2()->set_enabled(false); break;
    }
}

void MachineConfig::set_pipelined(bool v) {
    pipeline = v;
}

void MachineConfig::set_delay_slot(bool v) {
    delayslot = v;
}

void MachineConfig::set_hazard_unit(enum MachineConfig::HazardUnit hu) {
    hunit = hu;
}

bool MachineConfig::set_hazard_unit(const QString &hukind) {
    static QMap<QString, enum HazardUnit> hukind_map = {
        { "none", HU_NONE },
        { "stall", HU_STALL },
        { "forward", HU_STALL_FORWARD },
        { "stall-forward", HU_STALL_FORWARD },
    };
    if (!hukind_map.contains(hukind)) { return false; }
    set_hazard_unit(hukind_map.value(hukind));
    return true;
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

void MachineConfig::set_memory_access_time_burst(unsigned v) {
    mem_acc_burst = v;
}

void MachineConfig::set_memory_access_time_level2(unsigned v) {
    mem_acc_level2 = v;
}

void MachineConfig::set_memory_access_enable_burst(bool v) {
    mem_acc_enable_burst = v;
}

void MachineConfig::set_osemu_enable(bool v) {
    osem_enable = v;
}

void MachineConfig::set_osemu_known_syscall_stop(bool v) {
    osem_known_syscall_stop = v;
}

void MachineConfig::set_osemu_unknown_syscall_stop(bool v) {
    osem_unknown_syscall_stop = v;
}

void MachineConfig::set_osemu_interrupt_stop(bool v) {
    osem_interrupt_stop = v;
}

void MachineConfig::set_osemu_exception_stop(bool v) {
    osem_exception_stop = v;
}

void MachineConfig::set_osemu_fs_root(QString v) {
    osem_fs_root = std::move(v);
}

void MachineConfig::set_reset_at_compile(bool v) {
    res_at_compile = v;
}

void MachineConfig::set_elf(QString path) {
    elf_path = std::move(path);
}

void MachineConfig::set_cache_program(const CacheConfig &c) {
    cch_program = c;
}

void MachineConfig::set_cache_data(const CacheConfig &c) {
    cch_data = c;
}

void MachineConfig::set_cache_level2(const CacheConfig &c) {
    cch_level2 = c;
}

void MachineConfig::set_simulated_endian(Endian endian) {
    MachineConfig::simulated_endian = endian;
}

void MachineConfig::set_simulated_xlen(Xlen xlen) {
    simulated_xlen = xlen;
}

void MachineConfig::set_isa_word(ConfigIsaWord bits) {
    isa_word = bits | config_isa_word_fixed;
}
void MachineConfig::modify_isa_word(ConfigIsaWord mask, ConfigIsaWord val) {
    mask &= ~config_isa_word_fixed;
    isa_word.modify(mask, val);
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
    return mem_acc_read > 1 ? mem_acc_read : 1;
}

unsigned MachineConfig::memory_access_time_write() const {
    return mem_acc_write > 1 ? mem_acc_write : 1;
}

unsigned MachineConfig::memory_access_time_burst() const {
    return mem_acc_burst;
}

unsigned MachineConfig::memory_access_time_level2() const {
    return mem_acc_level2;
}

bool MachineConfig::memory_access_enable_burst() const {
    return mem_acc_enable_burst;
}

bool MachineConfig::osemu_enable() const {
    return osem_enable;
}
bool MachineConfig::osemu_known_syscall_stop() const {
    return osem_known_syscall_stop;
}
bool MachineConfig::osemu_unknown_syscall_stop() const {
    return osem_unknown_syscall_stop;
}
bool MachineConfig::osemu_interrupt_stop() const {
    return osem_interrupt_stop;
}
bool MachineConfig::osemu_exception_stop() const {
    return osem_exception_stop;
}

QString MachineConfig::osemu_fs_root() const {
    return osem_fs_root;
}

bool MachineConfig::reset_at_compile() const {
    return res_at_compile;
}

QString MachineConfig::elf() const {
    return elf_path;
}

const CacheConfig &MachineConfig::cache_program() const {
    return cch_program;
}

const CacheConfig &MachineConfig::cache_data() const {
    return cch_data;
}

const CacheConfig &MachineConfig::cache_level2() const {
    return cch_level2;
}

CacheConfig *MachineConfig::access_cache_program() {
    return &cch_program;
}

CacheConfig *MachineConfig::access_cache_data() {
    return &cch_data;
}

CacheConfig *MachineConfig::access_cache_level2() {
    return &cch_level2;
}

Endian MachineConfig::get_simulated_endian() const {
    return simulated_endian;
}

Xlen MachineConfig::get_simulated_xlen() const {
    return simulated_xlen;
}

ConfigIsaWord MachineConfig::get_isa_word() const {
    return isa_word;
}

void MachineConfig::set_bp_enabled(bool e) {
    bp_enabled = e;
}

void MachineConfig::set_bp_type(PredictorType t) {
    bp_type = t;
}

void MachineConfig::set_bp_init_state(PredictorState i) {
    bp_init_state = i;
}

void MachineConfig::set_bp_btb_bits(uint8_t b) {
    bp_btb_bits = b > BP_MAX_BTB_BITS ? BP_MAX_BTB_BITS : b;
}

void MachineConfig::set_bp_bhr_bits(uint8_t b) {
    bp_bhr_bits = b > BP_MAX_BHR_BITS ? BP_MAX_BHR_BITS : b;
    bp_bht_addr_bits
        = bp_bht_addr_bits > BP_MAX_BHT_ADDR_BITS ? BP_MAX_BHT_ADDR_BITS : bp_bht_addr_bits;
    bp_bht_bits = bp_bhr_bits + bp_bht_addr_bits;
    bp_bht_bits = bp_bht_bits > BP_MAX_BHT_BITS ? BP_MAX_BHT_BITS : bp_bht_bits;
}

void MachineConfig::set_bp_bht_addr_bits(uint8_t b) {
    bp_bht_addr_bits = b > BP_MAX_BHT_ADDR_BITS ? BP_MAX_BHT_ADDR_BITS : b;
    bp_bhr_bits = bp_bhr_bits > BP_MAX_BHR_BITS ? BP_MAX_BHR_BITS : bp_bhr_bits;
    bp_bht_bits = bp_bhr_bits + bp_bht_addr_bits;
    bp_bht_bits = bp_bht_bits > BP_MAX_BHT_BITS ? BP_MAX_BHT_BITS : bp_bht_bits;
}

bool MachineConfig::get_bp_enabled() const {
    return bp_enabled;
}

PredictorType MachineConfig::get_bp_type() const {
    return bp_type;
}

PredictorState MachineConfig::get_bp_init_state() const {
    return bp_init_state;
}

uint8_t MachineConfig::get_bp_btb_bits() const {
    return bp_btb_bits;
}

uint8_t MachineConfig::get_bp_bhr_bits() const {
    return bp_bhr_bits;
}

uint8_t MachineConfig::get_bp_bht_addr_bits() const {
    return bp_bht_addr_bits;
}

uint8_t MachineConfig::get_bp_bht_bits() const {
    return bp_bht_bits;
}

bool MachineConfig::operator==(const MachineConfig &c) const {
#define CMP(GETTER) (GETTER)() == (c.GETTER)()
    return CMP(pipelined) && CMP(delay_slot) && CMP(hazard_unit) && CMP(get_simulated_xlen)
           && CMP(get_isa_word) && CMP(get_bp_enabled) && CMP(get_bp_type)
           && CMP(get_bp_init_state) && CMP(get_bp_btb_bits)
           && CMP(get_bp_bhr_bits) && CMP(get_bp_bht_addr_bits)
           && CMP(memory_execute_protection) && CMP(memory_write_protection)
           && CMP(memory_access_time_read) && CMP(memory_access_time_write)
           && CMP(memory_access_time_burst) && CMP(memory_access_time_level2)
           && CMP(memory_access_enable_burst) && CMP(elf) && CMP(cache_program) && CMP(cache_data)
           && CMP(cache_level2);
#undef CMP
}

bool MachineConfig::operator!=(const MachineConfig &c) const {
    return !operator==(c);
}
