#include "machineconfig.h"

#include "common/endian.h"

#include <QJsonValue>
#include <QMap>
#include <cmath>
#include <limits>
#include <utility>

using namespace machine;

namespace {

constexpr auto MACHINE_CONFIG_SCHEMA = "qtrvsim.machine-config";
constexpr unsigned MACHINE_CONFIG_VERSION = 1;

bool is_power_of_two(unsigned value) {
    return value != 0 && (value & (value - 1)) == 0;
}

bool json_error(QString *error, const QString &message) {
    if (error != nullptr) { *error = message; }
    return false;
}

bool read_object(
    const QJsonObject &object,
    const QString &key,
    QJsonObject &value,
    QString *error) {
    auto json_value = object.value(key);
    if (!json_value.isObject()) {
        return json_error(error, QString("Field '%1' must be an object.").arg(key));
    }
    value = json_value.toObject();
    return true;
}

bool read_string(
    const QJsonObject &object,
    const QString &key,
    QString &value,
    QString *error) {
    auto json_value = object.value(key);
    if (!json_value.isString()) {
        return json_error(error, QString("Field '%1' must be a string.").arg(key));
    }
    value = json_value.toString();
    return true;
}

bool read_bool(const QJsonObject &object, const QString &key, bool &value, QString *error) {
    auto json_value = object.value(key);
    if (!json_value.isBool()) {
        return json_error(error, QString("Field '%1' must be a boolean.").arg(key));
    }
    value = json_value.toBool();
    return true;
}

template<typename T>
bool read_unsigned(
    const QJsonObject &object,
    const QString &key,
    T &value,
    QString *error) {
    auto json_value = object.value(key);
    if (!json_value.isDouble()) {
        return json_error(error, QString("Field '%1' must be an unsigned integer.").arg(key));
    }
    double number = json_value.toDouble();
    if (number < 0 || std::floor(number) != number
        || number > static_cast<double>(std::numeric_limits<T>::max())) {
        return json_error(error, QString("Field '%1' must be an unsigned integer.").arg(key));
    }
    value = static_cast<T>(number);
    return true;
}

QString cache_replacement_to_json(CacheConfig::ReplacementPolicy policy) {
    switch (policy) {
    case CacheConfig::RP_RAND: return "random";
    case CacheConfig::RP_LRU: return "lru";
    case CacheConfig::RP_LFU: return "lfu";
    case CacheConfig::RP_PLRU: return "plru";
    case CacheConfig::RP_NMRU: return "nmru";
    }
    Q_UNREACHABLE();
}

bool cache_replacement_from_json(
    const QString &value,
    CacheConfig::ReplacementPolicy &policy,
    QString *error) {
    if (value == "random") {
        policy = CacheConfig::RP_RAND;
    } else if (value == "lru") {
        policy = CacheConfig::RP_LRU;
    } else if (value == "lfu") {
        policy = CacheConfig::RP_LFU;
    } else if (value == "plru") {
        policy = CacheConfig::RP_PLRU;
    } else if (value == "nmru") {
        policy = CacheConfig::RP_NMRU;
    } else {
        return json_error(
            error, QString("Field 'replacement_policy' has unknown value '%1'.").arg(value));
    }
    return true;
}

QString cache_write_to_json(CacheConfig::WritePolicy policy) {
    switch (policy) {
    case CacheConfig::WP_THROUGH_NOALLOC: return "through-no-allocate";
    case CacheConfig::WP_THROUGH_ALLOC: return "through-allocate";
    case CacheConfig::WP_BACK: return "write-back";
    }
    Q_UNREACHABLE();
}

bool cache_write_from_json(
    const QString &value,
    CacheConfig::WritePolicy &policy,
    QString *error) {
    if (value == "through-no-allocate") {
        policy = CacheConfig::WP_THROUGH_NOALLOC;
    } else if (value == "through-allocate") {
        policy = CacheConfig::WP_THROUGH_ALLOC;
    } else if (value == "write-back") {
        policy = CacheConfig::WP_BACK;
    } else {
        return json_error(
            error, QString("Field 'write_policy' has unknown value '%1'.").arg(value));
    }
    return true;
}

QString tlb_replacement_to_json(TLBConfig::ReplacementPolicy policy) {
    switch (policy) {
    case TLBConfig::RP_RAND: return "random";
    case TLBConfig::RP_LRU: return "lru";
    case TLBConfig::RP_LFU: return "lfu";
    case TLBConfig::RP_PLRU: return "plru";
    }
    Q_UNREACHABLE();
}

bool tlb_replacement_from_json(
    const QString &value,
    TLBConfig::ReplacementPolicy &policy,
    QString *error) {
    if (value == "random") {
        policy = TLBConfig::RP_RAND;
    } else if (value == "lru") {
        policy = TLBConfig::RP_LRU;
    } else if (value == "lfu") {
        policy = TLBConfig::RP_LFU;
    } else if (value == "plru") {
        policy = TLBConfig::RP_PLRU;
    } else {
        return json_error(
            error, QString("Field 'replacement_policy' has unknown value '%1'.").arg(value));
    }
    return true;
}

QString hazard_unit_to_json(MachineConfig::HazardUnit hazard_unit) {
    switch (hazard_unit) {
    case MachineConfig::HU_NONE: return "none";
    case MachineConfig::HU_STALL: return "stall";
    case MachineConfig::HU_STALL_FORWARD: return "forward";
    }
    Q_UNREACHABLE();
}

bool hazard_unit_from_json(
    const QString &value,
    MachineConfig::HazardUnit &hazard_unit,
    QString *error) {
    if (value == "none") {
        hazard_unit = MachineConfig::HU_NONE;
    } else if (value == "stall") {
        hazard_unit = MachineConfig::HU_STALL;
    } else if (value == "forward") {
        hazard_unit = MachineConfig::HU_STALL_FORWARD;
    } else {
        return json_error(
            error, QString("Field 'hazard_unit' has unknown value '%1'.").arg(value));
    }
    return true;
}

QString predictor_type_to_json(PredictorType type) {
    switch (type) {
    case PredictorType::ALWAYS_NOT_TAKEN: return "always_not_taken";
    case PredictorType::ALWAYS_TAKEN: return "always_taken";
    case PredictorType::BTFNT: return "btfnt";
    case PredictorType::SMITH_1_BIT: return "smith_1_bit";
    case PredictorType::SMITH_2_BIT: return "smith_2_bit";
    case PredictorType::SMITH_2_BIT_HYSTERESIS: return "smith_2_bit_hysteresis";
    case PredictorType::UNDEFINED: return "undefined";
    }
    Q_UNREACHABLE();
}

bool predictor_type_from_json(
    const QString &value,
    PredictorType &type,
    QString *error) {
    if (value == "always_not_taken") {
        type = PredictorType::ALWAYS_NOT_TAKEN;
    } else if (value == "always_taken") {
        type = PredictorType::ALWAYS_TAKEN;
    } else if (value == "btfnt") {
        type = PredictorType::BTFNT;
    } else if (value == "smith_1_bit") {
        type = PredictorType::SMITH_1_BIT;
    } else if (value == "smith_2_bit") {
        type = PredictorType::SMITH_2_BIT;
    } else if (value == "smith_2_bit_hysteresis") {
        type = PredictorType::SMITH_2_BIT_HYSTERESIS;
    } else if (value == "undefined") {
        type = PredictorType::UNDEFINED;
    } else {
        return json_error(
            error, QString("Field 'branch_predictor.type' has unknown value '%1'.").arg(value));
    }
    return true;
}

QString predictor_state_to_json(PredictorState state) {
    switch (state) {
    case PredictorState::NOT_TAKEN: return "not_taken";
    case PredictorState::TAKEN: return "taken";
    case PredictorState::STRONGLY_NOT_TAKEN: return "strongly_not_taken";
    case PredictorState::WEAKLY_NOT_TAKEN: return "weakly_not_taken";
    case PredictorState::WEAKLY_TAKEN: return "weakly_taken";
    case PredictorState::STRONGLY_TAKEN: return "strongly_taken";
    case PredictorState::UNDEFINED: return "undefined";
    }
    Q_UNREACHABLE();
}

bool predictor_state_from_json(
    const QString &value,
    PredictorState &state,
    QString *error) {
    if (value == "not_taken") {
        state = PredictorState::NOT_TAKEN;
    } else if (value == "taken") {
        state = PredictorState::TAKEN;
    } else if (value == "strongly_not_taken") {
        state = PredictorState::STRONGLY_NOT_TAKEN;
    } else if (value == "weakly_not_taken") {
        state = PredictorState::WEAKLY_NOT_TAKEN;
    } else if (value == "weakly_taken") {
        state = PredictorState::WEAKLY_TAKEN;
    } else if (value == "strongly_taken") {
        state = PredictorState::STRONGLY_TAKEN;
    } else if (value == "undefined") {
        state = PredictorState::UNDEFINED;
    } else {
        return json_error(
            error,
            QString("Field 'branch_predictor.initial_state' has unknown value '%1'.").arg(value));
    }
    return true;
}

bool validate_predictor_config(
    PredictorType type,
    PredictorState state,
    QString *error) {
    switch (type) {
    case PredictorType::ALWAYS_NOT_TAKEN:
    case PredictorType::ALWAYS_TAKEN:
    case PredictorType::BTFNT: return true;
    case PredictorType::SMITH_1_BIT:
        if (state == PredictorState::NOT_TAKEN || state == PredictorState::TAKEN) { return true; }
        return json_error(
            error, "Smith 1-bit predictor requires 'not_taken' or 'taken' initial state.");
    case PredictorType::SMITH_2_BIT:
    case PredictorType::SMITH_2_BIT_HYSTERESIS:
        if (state == PredictorState::STRONGLY_NOT_TAKEN
            || state == PredictorState::WEAKLY_NOT_TAKEN
            || state == PredictorState::WEAKLY_TAKEN
            || state == PredictorState::STRONGLY_TAKEN) {
            return true;
        }
        return json_error(error, "Smith 2-bit predictor requires a 2-bit initial state.");
    case PredictorType::UNDEFINED:
        return json_error(error, "Branch predictor type must not be 'undefined'.");
    }
    Q_UNREACHABLE();
}

} // namespace

//////////////////////////////////////////////////////////////////////////////
/// Default config of MachineConfig
#define DF_PIPELINE             false
#define DF_DELAYSLOT            true
#define DF_HUNIT                HU_STALL_FORWARD
#define DF_EXEC_PROTEC          false
#define DF_WRITE_PROTEC         false
#define DF_MEM_ACC_READ         10
#define DF_MEM_ACC_WRITE        10
#define DF_MEM_ACC_BURST        0
#define DF_MEM_ACC_LEVEL2       2
#define DF_MEM_ACC_BURST_ENABLE false
#define DF_ELF                  QString("")
/// Default config of branch predictor
#define DFC_BP_ENABLED       false
#define DFC_BP_TYPE          PredictorType::SMITH_1_BIT
#define DFC_BP_INIT_STATE    PredictorState::NOT_TAKEN
#define DFC_BP_BTB_BITS      2
#define DFC_BP_BHR_BITS      0
#define DFC_BP_BHT_ADDR_BITS 2
/// Default config of Virtual Memory
#define DFC_VM_ENABLED false
#define DFC_TLB_SETS   16
#define DFC_TLB_ASSOC  1
#define DFC_TLB_REPLAC RP_LRU
//////////////////////////////////////////////////////////////////////////////
/// Default config of CacheConfig
#define DFC_EN     false
#define DFC_SETS   1
#define DFC_BLOCKS 1
#define DFC_ASSOC  1
#define DFC_REPLAC RP_RAND
#define DFC_WRITE  WP_THROUGH_NOALLOC
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

QJsonObject CacheConfig::to_json() const {
    return {
        { "enabled", enabled() },
        { "sets", static_cast<double>(set_count()) },
        { "block_size", static_cast<double>(block_size()) },
        { "associativity", static_cast<double>(associativity()) },
        { "replacement_policy", cache_replacement_to_json(replacement_policy()) },
        { "write_policy", cache_write_to_json(write_policy()) },
    };
}

std::optional<CacheConfig> CacheConfig::from_json(const QJsonObject &json, QString *error) {
    bool enabled;
    unsigned sets;
    unsigned block_size;
    unsigned associativity;
    QString replacement;
    QString write;
    if (!read_bool(json, "enabled", enabled, error)
        || !read_unsigned(json, "sets", sets, error)
        || !read_unsigned(json, "block_size", block_size, error)
        || !read_unsigned(json, "associativity", associativity, error)
        || !read_string(json, "replacement_policy", replacement, error)
        || !read_string(json, "write_policy", write, error)) {
        return std::nullopt;
    }
    if (sets == 0 || block_size == 0 || associativity == 0) {
        json_error(error, "Cache sets, block size and associativity must be greater than zero.");
        return std::nullopt;
    }

    ReplacementPolicy replacement_policy;
    WritePolicy write_policy;
    if (!cache_replacement_from_json(replacement, replacement_policy, error)
        || !cache_write_from_json(write, write_policy, error)) {
        return std::nullopt;
    }

    CacheConfig config;
    config.set_enabled(enabled);
    config.set_set_count(sets);
    config.set_block_size(block_size);
    config.set_associativity(associativity);
    config.set_replacement_policy(replacement_policy);
    config.set_write_policy(write_policy);
    return config;
}

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
//////////////////////////////////////////////////////////////////////////////

TLBConfig::TLBConfig() {
    vm_asid = 0;
    n_sets = DFC_TLB_SETS;
    d_associativity = DFC_TLB_ASSOC;
    replac_pol = (enum ReplacementPolicy)DFC_TLB_REPLAC;
}

TLBConfig::TLBConfig(const TLBConfig *tc) {
    if (tc == nullptr) {
        vm_asid = 0;
        n_sets = DFC_TLB_SETS;
        d_associativity = DFC_TLB_ASSOC;
        replac_pol = (enum ReplacementPolicy)DFC_TLB_REPLAC;
        return;
    }
    vm_asid = tc->get_vm_asid();
    n_sets = tc->get_tlb_num_sets();
    d_associativity = tc->get_tlb_associativity();
    replac_pol = tc->get_tlb_replacement_policy();
}

#define N(STR) (prefix + QString(STR))

TLBConfig::TLBConfig(const QSettings *sts, const QString &prefix) {
    vm_asid = sts->value(N("VM_ASID"), 0u).toUInt();
    n_sets = sts->value(N("NumSets"), DFC_TLB_SETS).toUInt();
    d_associativity = sts->value(N("Associativity"), DFC_TLB_ASSOC).toUInt();
    replac_pol = (enum ReplacementPolicy)sts->value(N("Policy"), DFC_TLB_REPLAC).toUInt();
}

void TLBConfig::store(QSettings *sts, const QString &prefix) const {
    sts->setValue(N("VM_ASID"), get_vm_asid());
    sts->setValue(N("NumSets"), get_tlb_num_sets());
    sts->setValue(N("Associativity"), get_tlb_associativity());
    sts->setValue(N("Policy"), (unsigned)get_tlb_replacement_policy());
}

#undef N

QJsonObject TLBConfig::to_json() const {
    return {
        { "asid", static_cast<double>(get_vm_asid()) },
        { "sets", static_cast<double>(get_tlb_num_sets()) },
        { "associativity", static_cast<double>(get_tlb_associativity()) },
        { "replacement_policy", tlb_replacement_to_json(get_tlb_replacement_policy()) },
    };
}

std::optional<TLBConfig> TLBConfig::from_json(const QJsonObject &json, QString *error) {
    uint32_t asid;
    unsigned sets;
    unsigned associativity;
    QString replacement;
    if (!read_unsigned(json, "asid", asid, error)
        || !read_unsigned(json, "sets", sets, error)
        || !read_unsigned(json, "associativity", associativity, error)
        || !read_string(json, "replacement_policy", replacement, error)) {
        return std::nullopt;
    }
    if (!is_power_of_two(sets)) {
        json_error(error, "TLB sets must be a power of two.");
        return std::nullopt;
    }
    if (associativity == 0) {
        json_error(error, "TLB associativity must be greater than zero.");
        return std::nullopt;
    }

    ReplacementPolicy replacement_policy;
    if (!tlb_replacement_from_json(replacement, replacement_policy, error)) {
        return std::nullopt;
    }

    TLBConfig config;
    config.set_vm_asid(asid);
    config.set_tlb_num_sets(sets);
    config.set_tlb_associativity(associativity);
    config.set_tlb_replacement_policy(replacement_policy);
    return config;
}

void TLBConfig::preset(enum ConfigPresets p) {
    switch (p) {
    case CP_PIPE:
    case CP_SINGLE_CACHE:
    case CP_SINGLE:
    case CP_PIPE_NO_HAZARD:
        vm_asid = 0;
        n_sets = DFC_TLB_SETS;
        d_associativity = DFC_TLB_ASSOC;
        replac_pol = (enum ReplacementPolicy)DFC_TLB_REPLAC;
    }
}

void TLBConfig::set_vm_asid(uint32_t a) {
    vm_asid = a;
}

uint32_t TLBConfig::get_vm_asid() const {
    return vm_asid;
}

void TLBConfig::set_tlb_num_sets(unsigned v) {
    n_sets = v > 0 ? v : 1;
}

void TLBConfig::set_tlb_associativity(unsigned v) {
    d_associativity = v > 0 ? v : 1;
}

void TLBConfig::set_tlb_replacement_policy(TLBConfig::ReplacementPolicy p) {
    replac_pol = p;
}

unsigned TLBConfig::get_tlb_num_sets() const {
    return n_sets;
}

unsigned TLBConfig::get_tlb_associativity() const {
    return d_associativity;
}

TLBConfig::ReplacementPolicy TLBConfig::get_tlb_replacement_policy() const {
    return replac_pol;
}

bool TLBConfig::operator==(const TLBConfig &c) const {
#define CMP(GETTER) (GETTER)() == (c.GETTER)()
    return CMP(get_vm_asid) && CMP(get_tlb_num_sets) && CMP(get_tlb_associativity)
           && CMP(get_tlb_replacement_policy);
#undef CMP
}

bool TLBConfig::operator!=(const TLBConfig &c) const {
    return !operator==(c);
}

//////////////////////////////////////////////////////////////////////////////

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

    // Virtual memory
    vm_enabled = DFC_VM_ENABLED;
    tlb_program = TLBConfig();
    tlb_data = TLBConfig();
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

    // Virtual memory
    vm_enabled = config->get_vm_enabled();
    tlb_program = config->tlbc_program();
    tlb_data = config->tlbc_data();
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

    // Virtual memory
    vm_enabled = sts->value(N("VMEnabled"), DFC_VM_ENABLED).toBool();
    tlb_data = TLBConfig(sts, N("DataTLB_"));
    tlb_program = TLBConfig(sts, N("ProgramTLB_"));
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

    // Virtual memory
    sts->setValue(N("VMEnabled"), get_vm_enabled());
    tlbc_program().store(sts, N("ProgramTLB_"));
    tlbc_data().store(sts, N("DataTLB_"));
}

#undef N

QJsonObject MachineConfig::to_json() const {
    const QString isa_word_json = QString("0x") + QString::number(isa_word.toUnsigned(), 16);
    QJsonObject core {
        { "pipelined", pipeline },
        { "delay_slot", delayslot },
        { "hazard_unit", hazard_unit_to_json(hunit) },
    };
    QJsonObject memory {
        { "execute_protection", exec_protect },
        { "write_protection", write_protect },
        { "read_time", static_cast<double>(mem_acc_read) },
        { "write_time", static_cast<double>(mem_acc_write) },
        { "burst_time", static_cast<double>(mem_acc_burst) },
        { "level2_time", static_cast<double>(mem_acc_level2) },
        { "burst_enabled", mem_acc_enable_burst },
    };
    QJsonObject os_emulation {
        { "enabled", osem_enable },
        { "stop_on_known_syscall", osem_known_syscall_stop },
        { "stop_on_unknown_syscall", osem_unknown_syscall_stop },
        { "stop_on_interrupt", osem_interrupt_stop },
        { "stop_on_exception", osem_exception_stop },
        { "filesystem_root", osem_fs_root },
    };
    QJsonObject caches {
        { "program", cch_program.to_json() },
        { "data", cch_data.to_json() },
        { "level2", cch_level2.to_json() },
    };
    QJsonObject branch_predictor {
        { "enabled", bp_enabled },
        { "type", predictor_type_to_json(bp_type) },
        { "initial_state", predictor_state_to_json(bp_init_state) },
        { "btb_bits", static_cast<double>(bp_btb_bits) },
        { "bhr_bits", static_cast<double>(bp_bhr_bits) },
        { "bht_address_bits", static_cast<double>(bp_bht_addr_bits) },
    };
    QJsonObject virtual_memory {
        { "enabled", vm_enabled },
        { "program_tlb", tlb_program.to_json() },
        { "data_tlb", tlb_data.to_json() },
    };
    QJsonObject machine {
        { "endian", simulated_endian == LITTLE ? "little" : "big" },
        { "xlen", static_cast<int>(simulated_xlen) },
        { "isa_word", isa_word_json },
        { "core", core },
        { "memory", memory },
        { "os_emulation", os_emulation },
        { "reset_at_compile", res_at_compile },
        { "elf", elf_path },
        { "caches", caches },
        { "branch_predictor", branch_predictor },
        { "virtual_memory", virtual_memory },
    };
    return {
        { "schema", MACHINE_CONFIG_SCHEMA },
        { "version", static_cast<int>(MACHINE_CONFIG_VERSION) },
        { "machine", machine },
    };
}

std::optional<MachineConfig>
MachineConfig::from_json(const QJsonObject &json, QString *error) {
    QString schema;
    unsigned version;
    QJsonObject machine;
    if (!read_string(json, "schema", schema, error)
        || !read_unsigned(json, "version", version, error)
        || !read_object(json, "machine", machine, error)) {
        return std::nullopt;
    }
    if (schema != MACHINE_CONFIG_SCHEMA) {
        json_error(error, QString("Unknown machine config schema '%1'.").arg(schema));
        return std::nullopt;
    }
    if (version != MACHINE_CONFIG_VERSION) {
        json_error(error, QString("Unsupported machine config version %1.").arg(version));
        return std::nullopt;
    }

    QString endian;
    unsigned xlen;
    QString isa_word;
    QJsonObject core;
    QJsonObject memory;
    QJsonObject os_emulation;
    bool reset_at_compile;
    QString elf;
    QJsonObject caches;
    QJsonObject branch_predictor;
    QJsonObject virtual_memory;
    if (!read_string(machine, "endian", endian, error)
        || !read_unsigned(machine, "xlen", xlen, error)
        || !read_string(machine, "isa_word", isa_word, error)
        || !read_object(machine, "core", core, error)
        || !read_object(machine, "memory", memory, error)
        || !read_object(machine, "os_emulation", os_emulation, error)
        || !read_bool(machine, "reset_at_compile", reset_at_compile, error)
        || !read_string(machine, "elf", elf, error)
        || !read_object(machine, "caches", caches, error)
        || !read_object(machine, "branch_predictor", branch_predictor, error)
        || !read_object(machine, "virtual_memory", virtual_memory, error)) {
        return std::nullopt;
    }

    Endian parsed_endian;
    if (endian == "little") {
        parsed_endian = LITTLE;
    } else if (endian == "big") {
        parsed_endian = BIG;
    } else {
        json_error(error, QString("Field 'endian' has unknown value '%1'.").arg(endian));
        return std::nullopt;
    }
    Xlen parsed_xlen;
    if (xlen == 32) {
        parsed_xlen = Xlen::_32;
    } else if (xlen == 64) {
        parsed_xlen = Xlen::_64;
    } else {
        json_error(error, "Field 'xlen' must be 32 or 64.");
        return std::nullopt;
    }
    bool isa_ok = false;
    quint64 parsed_isa_word = isa_word.toULongLong(&isa_ok, 0);
    if (!isa_ok) {
        json_error(error, QString("Field 'isa_word' has invalid value '%1'.").arg(isa_word));
        return std::nullopt;
    }

    bool pipelined;
    bool delay_slot;
    QString hazard_unit;
    if (!read_bool(core, "pipelined", pipelined, error)
        || !read_bool(core, "delay_slot", delay_slot, error)
        || !read_string(core, "hazard_unit", hazard_unit, error)) {
        return std::nullopt;
    }
    HazardUnit parsed_hazard_unit;
    if (!hazard_unit_from_json(hazard_unit, parsed_hazard_unit, error)) {
        return std::nullopt;
    }

    bool execute_protection;
    bool write_protection;
    unsigned read_time;
    unsigned write_time;
    unsigned burst_time;
    unsigned level2_time;
    bool burst_enabled;
    if (!read_bool(memory, "execute_protection", execute_protection, error)
        || !read_bool(memory, "write_protection", write_protection, error)
        || !read_unsigned(memory, "read_time", read_time, error)
        || !read_unsigned(memory, "write_time", write_time, error)
        || !read_unsigned(memory, "burst_time", burst_time, error)
        || !read_unsigned(memory, "level2_time", level2_time, error)
        || !read_bool(memory, "burst_enabled", burst_enabled, error)) {
        return std::nullopt;
    }

    bool osemu_enabled;
    bool stop_on_known_syscall;
    bool stop_on_unknown_syscall;
    bool stop_on_interrupt;
    bool stop_on_exception;
    QString filesystem_root;
    if (!read_bool(os_emulation, "enabled", osemu_enabled, error)
        || !read_bool(os_emulation, "stop_on_known_syscall", stop_on_known_syscall, error)
        || !read_bool(os_emulation, "stop_on_unknown_syscall", stop_on_unknown_syscall, error)
        || !read_bool(os_emulation, "stop_on_interrupt", stop_on_interrupt, error)
        || !read_bool(os_emulation, "stop_on_exception", stop_on_exception, error)
        || !read_string(os_emulation, "filesystem_root", filesystem_root, error)) {
        return std::nullopt;
    }

    QJsonObject program_cache_json;
    QJsonObject data_cache_json;
    QJsonObject level2_cache_json;
    if (!read_object(caches, "program", program_cache_json, error)
        || !read_object(caches, "data", data_cache_json, error)
        || !read_object(caches, "level2", level2_cache_json, error)) {
        return std::nullopt;
    }
    auto program_cache = CacheConfig::from_json(program_cache_json, error);
    auto data_cache = CacheConfig::from_json(data_cache_json, error);
    auto level2_cache = CacheConfig::from_json(level2_cache_json, error);
    if (!program_cache || !data_cache || !level2_cache) { return std::nullopt; }

    bool predictor_enabled;
    QString predictor_type;
    QString predictor_initial_state;
    uint8_t btb_bits;
    uint8_t bhr_bits;
    uint8_t bht_address_bits;
    if (!read_bool(branch_predictor, "enabled", predictor_enabled, error)
        || !read_string(branch_predictor, "type", predictor_type, error)
        || !read_string(branch_predictor, "initial_state", predictor_initial_state, error)
        || !read_unsigned(branch_predictor, "btb_bits", btb_bits, error)
        || !read_unsigned(branch_predictor, "bhr_bits", bhr_bits, error)
        || !read_unsigned(branch_predictor, "bht_address_bits", bht_address_bits, error)) {
        return std::nullopt;
    }
    if (btb_bits > BP_MAX_BTB_BITS) {
        json_error(
            error,
            QString("Field 'btb_bits' must not exceed %1.").arg(BP_MAX_BTB_BITS));
        return std::nullopt;
    }
    if (bhr_bits > BP_MAX_BHR_BITS) {
        json_error(
            error,
            QString("Field 'bhr_bits' must not exceed %1.").arg(BP_MAX_BHR_BITS));
        return std::nullopt;
    }
    if (bht_address_bits > BP_MAX_BHT_ADDR_BITS) {
        json_error(
            error,
            QString("Field 'bht_address_bits' must not exceed %1.")
                .arg(BP_MAX_BHT_ADDR_BITS));
        return std::nullopt;
    }
    PredictorType parsed_predictor_type;
    PredictorState parsed_predictor_state;
    if (!predictor_type_from_json(predictor_type, parsed_predictor_type, error)
        || !predictor_state_from_json(predictor_initial_state, parsed_predictor_state, error)
        || !validate_predictor_config(parsed_predictor_type, parsed_predictor_state, error)) {
        return std::nullopt;
    }

    bool vm_enabled;
    QJsonObject program_tlb_json;
    QJsonObject data_tlb_json;
    if (!read_bool(virtual_memory, "enabled", vm_enabled, error)
        || !read_object(virtual_memory, "program_tlb", program_tlb_json, error)
        || !read_object(virtual_memory, "data_tlb", data_tlb_json, error)) {
        return std::nullopt;
    }
    auto program_tlb = TLBConfig::from_json(program_tlb_json, error);
    auto data_tlb = TLBConfig::from_json(data_tlb_json, error);
    if (!program_tlb || !data_tlb) { return std::nullopt; }

    MachineConfig config;
    config.set_simulated_endian(parsed_endian);
    config.set_simulated_xlen(parsed_xlen);
    config.set_isa_word(parsed_isa_word);
    config.set_pipelined(pipelined);
    config.set_delay_slot(delay_slot);
    config.set_hazard_unit(parsed_hazard_unit);
    config.set_memory_execute_protection(execute_protection);
    config.set_memory_write_protection(write_protection);
    config.set_memory_access_time_read(read_time);
    config.set_memory_access_time_write(write_time);
    config.set_memory_access_time_burst(burst_time);
    config.set_memory_access_time_level2(level2_time);
    config.set_memory_access_enable_burst(burst_enabled);
    config.set_osemu_enable(osemu_enabled);
    config.set_osemu_known_syscall_stop(stop_on_known_syscall);
    config.set_osemu_unknown_syscall_stop(stop_on_unknown_syscall);
    config.set_osemu_interrupt_stop(stop_on_interrupt);
    config.set_osemu_exception_stop(stop_on_exception);
    config.set_osemu_fs_root(filesystem_root);
    config.set_reset_at_compile(reset_at_compile);
    config.set_elf(elf);
    config.set_cache_program(*program_cache);
    config.set_cache_data(*data_cache);
    config.set_cache_level2(*level2_cache);
    config.set_bp_enabled(predictor_enabled);
    config.set_bp_type(parsed_predictor_type);
    config.set_bp_init_state(parsed_predictor_state);
    config.set_bp_btb_bits(btb_bits);
    config.set_bp_bhr_bits(bhr_bits);
    config.set_bp_bht_addr_bits(bht_address_bits);
    config.set_vm_enabled(vm_enabled);
    *config.access_tlb_program() = *program_tlb;
    *config.access_tlb_data() = *data_tlb;
    return config;
}

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

    set_vm_enabled(DFC_VM_ENABLED);
    access_tlb_program()->preset(p);
    access_tlb_data()->preset(p);

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

TLBConfig *MachineConfig::access_tlb_program() {
    return &tlb_program;
}

TLBConfig *MachineConfig::access_tlb_data() {
    return &tlb_data;
}

const TLBConfig &MachineConfig::tlbc_program() const {
    return tlb_program;
}

const TLBConfig &MachineConfig::tlbc_data() const {
    return tlb_data;
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

void MachineConfig::set_vm_enabled(bool v) {
    vm_enabled = v;
}

bool MachineConfig::get_vm_enabled() const {
    return vm_enabled;
}

bool MachineConfig::operator==(const MachineConfig &c) const {
    return pipeline == c.pipeline && delayslot == c.delayslot && hunit == c.hunit
           && exec_protect == c.exec_protect && write_protect == c.write_protect
           && mem_acc_read == c.mem_acc_read && mem_acc_write == c.mem_acc_write
           && mem_acc_burst == c.mem_acc_burst && mem_acc_level2 == c.mem_acc_level2
           && mem_acc_enable_burst == c.mem_acc_enable_burst && osem_enable == c.osem_enable
           && osem_known_syscall_stop == c.osem_known_syscall_stop
           && osem_unknown_syscall_stop == c.osem_unknown_syscall_stop
           && osem_interrupt_stop == c.osem_interrupt_stop
           && osem_exception_stop == c.osem_exception_stop && res_at_compile == c.res_at_compile
           && osem_fs_root == c.osem_fs_root && elf_path == c.elf_path
           && cch_program == c.cch_program && cch_data == c.cch_data
           && cch_level2 == c.cch_level2 && simulated_endian == c.simulated_endian
           && simulated_xlen == c.simulated_xlen && isa_word == c.isa_word
           && bp_enabled == c.bp_enabled && bp_type == c.bp_type
           && bp_init_state == c.bp_init_state && bp_btb_bits == c.bp_btb_bits
           && bp_bhr_bits == c.bp_bhr_bits && bp_bht_addr_bits == c.bp_bht_addr_bits
           && bp_bht_bits == c.bp_bht_bits && vm_enabled == c.vm_enabled
           && tlb_program == c.tlb_program && tlb_data == c.tlb_data;
}

bool MachineConfig::operator!=(const MachineConfig &c) const {
    return !operator==(c);
}
