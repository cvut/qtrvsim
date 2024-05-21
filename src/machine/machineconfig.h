#ifndef MACHINECONFIG_H
#define MACHINECONFIG_H

#include "common/endian.h"
#include "config_isa.h"
#include "predictor_types.h"

#include <QSettings>
#include <QString>

namespace machine {

/**
 * There are two primary base integer variants, RV32I and RV64I, described in Chapters 2 and 5,
 * which provide 32-bit or 64-bit address spaces respectively. We use the term XLEN to refer to
 * the width of an integer register in bits (either 32 or 64). [RISC-V Unprivileged ISA, page 4]
 *
 * `RegisterValue` type is used to abstract the size of stored value. It provides methods to get
 *  correct size. The `Core` can extract them with a special method.
 */
enum class Xlen { _32 = 32, _64 = 64 };

enum ConfigPresets {
    CP_SINGLE,         // No pipeline cpu without cache
    CP_SINGLE_CACHE,   // No pipeline cpu with cache
    CP_PIPE_NO_HAZARD, // Pipelined cpu without hazard unit and without cache
    CP_PIPE            // Full pipelined cpu
};

constexpr ConfigIsaWord config_isa_word_default = ConfigIsaWord::byChar('E') | ConfigIsaWord::byChar('I') |
        ConfigIsaWord::byChar('A') |ConfigIsaWord::byChar('M');

constexpr ConfigIsaWord config_isa_word_fixed = ConfigIsaWord::byChar('E') | ConfigIsaWord::byChar('I');

class CacheConfig {
public:
    CacheConfig();
    explicit CacheConfig(const CacheConfig *cc);
    explicit CacheConfig(const QSettings *, const QString &prefix = "");

    void store(QSettings *, const QString &prefix = "") const;

    void preset(enum ConfigPresets);

    enum ReplacementPolicy {
        RP_RAND, // Random
        RP_LRU,  // Least recently used
        RP_LFU,  // Least frequently used
        RP_PLRU  // Pseudo Least recently used
    };

    enum WritePolicy {
        WP_THROUGH_NOALLOC, // Write through
        WP_THROUGH_ALLOC,   // Write through
        WP_BACK             // Write back
    };

    // If cache should be used or not
    void set_enabled(bool);
    void set_set_count(unsigned);     // Number of sets
    void set_block_size(unsigned);    // Number of uint32_t in block
    void set_associativity(unsigned); // Degree of associativity (number of
                                      // ways)
    void set_replacement_policy(enum ReplacementPolicy);
    void set_write_policy(enum WritePolicy);

    bool enabled() const;
    unsigned set_count() const;
    unsigned block_size() const;
    unsigned associativity() const;
    enum ReplacementPolicy replacement_policy() const;
    enum WritePolicy write_policy() const;

    bool operator==(const CacheConfig &c) const;
    bool operator!=(const CacheConfig &c) const;

private:
    bool en;
    unsigned n_sets, n_blocks, d_associativity;
    enum ReplacementPolicy replac_pol;
    enum WritePolicy write_pol;
};

class MachineConfig {
public:
    MachineConfig();
    explicit MachineConfig(const MachineConfig *config);
    explicit MachineConfig(const QSettings *, const QString &prefix = "");

    void store(QSettings *, const QString &prefix = "");

    void preset(enum ConfigPresets);

    enum HazardUnit { HU_NONE, HU_STALL, HU_STALL_FORWARD };

    // Configure if CPU is pipelined
    // In default disabled.
    void set_pipelined(bool);
    // Configure if cpu should simulate delay slot in non-pipelined core
    // In default enabled. When disabled it also automatically disables
    // pipelining.
    void set_delay_slot(bool);
    // Hazard unit
    void set_hazard_unit(enum HazardUnit);
    bool set_hazard_unit(const QString &hukind);
    // Protect data memory from execution. Only program sections can be
    // executed.
    void set_memory_execute_protection(bool);
    // Protect program memory from accidental writes.
    void set_memory_write_protection(bool);
    // Set memory access times. Passed value is in cycles.
    void set_memory_access_time_read(unsigned);
    void set_memory_access_time_write(unsigned);
    void set_memory_access_time_burst(unsigned);
    void set_memory_access_time_level2(unsigned);
    void set_memory_access_enable_burst(bool);
    // Operating system and exceptions setup
    void set_osemu_enable(bool);
    void set_osemu_known_syscall_stop(bool);
    void set_osemu_unknown_syscall_stop(bool);
    void set_osemu_interrupt_stop(bool);
    void set_osemu_exception_stop(bool);
    void set_osemu_fs_root(QString v);
    // reset machine befor internal compile/reload after external make
    void set_reset_at_compile(bool);
    // Set path to source elf file. This has to be set before core is
    // initialized.
    void set_elf(QString path);
    // Configure cache
    void set_cache_program(const CacheConfig &);
    void set_cache_data(const CacheConfig &);
    void set_cache_level2(const CacheConfig &);
    void set_simulated_endian(Endian endian);
    void set_simulated_xlen(Xlen xlen);
    void set_isa_word(ConfigIsaWord bits);
    void modify_isa_word(ConfigIsaWord mask, ConfigIsaWord val);

    bool pipelined() const;
    bool delay_slot() const;
    enum HazardUnit hazard_unit() const;
    bool memory_execute_protection() const;
    bool memory_write_protection() const;
    unsigned memory_access_time_read() const;
    unsigned memory_access_time_write() const;
    unsigned memory_access_time_burst() const;
    unsigned memory_access_time_level2() const;
    bool memory_access_enable_burst() const;
    bool osemu_enable() const;
    bool osemu_known_syscall_stop() const;
    bool osemu_unknown_syscall_stop() const;
    bool osemu_interrupt_stop() const;
    bool osemu_exception_stop() const;
    QString osemu_fs_root() const;
    bool reset_at_compile() const;
    QString elf() const;
    const CacheConfig &cache_program() const;
    const CacheConfig &cache_data() const;
    const CacheConfig &cache_level2() const;
    Endian get_simulated_endian() const;
    Xlen get_simulated_xlen() const;
    ConfigIsaWord get_isa_word() const;

    // Branch predictor - Setters
    void set_bp_enabled(bool e);
    void set_bp_type(PredictorType t);
    void set_bp_init_state(PredictorState i);
    void set_bp_btb_bits(uint8_t b);
    void set_bp_bhr_bits(uint8_t b);
    void set_bp_bht_addr_bits(uint8_t b);
    // Branch predictor - Getters
    bool get_bp_enabled() const;
    PredictorType get_bp_type() const;
    PredictorState get_bp_init_state() const;
    uint8_t get_bp_btb_bits() const;
    uint8_t get_bp_bhr_bits() const;
    uint8_t get_bp_bht_addr_bits() const;
    uint8_t get_bp_bht_bits() const;

    CacheConfig *access_cache_program();
    CacheConfig *access_cache_data();
    CacheConfig *access_cache_level2();

    bool operator==(const MachineConfig &c) const;
    bool operator!=(const MachineConfig &c) const;

private:
    bool pipeline, delayslot;
    enum HazardUnit hunit;
    bool exec_protect, write_protect;
    unsigned mem_acc_read, mem_acc_write, mem_acc_burst, mem_acc_level2;
    bool mem_acc_enable_burst;
    bool osem_enable, osem_known_syscall_stop, osem_unknown_syscall_stop;
    bool osem_interrupt_stop, osem_exception_stop;
    bool res_at_compile;
    QString osem_fs_root;
    QString elf_path;
    CacheConfig cch_program, cch_data, cch_level2;
    Endian simulated_endian;
    Xlen simulated_xlen;
    ConfigIsaWord isa_word;

    // Branch predictor
    bool bp_enabled;
    PredictorType bp_type;
    PredictorState bp_init_state;
    uint8_t bp_btb_bits;
    uint8_t bp_bhr_bits;
    uint8_t bp_bht_addr_bits;
    uint8_t bp_bht_bits; // = bp_bhr_bits + bp_bht_addr_bits
};

} // namespace machine

Q_DECLARE_METATYPE(machine::CacheConfig)

#endif // MACHINECONFIG_H
