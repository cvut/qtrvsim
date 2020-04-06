// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#ifndef MACHINECONFIG_H
#define MACHINECONFIG_H

#include <QString>
#include <QSettings>

namespace machine {

enum ConfigPresets {
    CP_SINGLE, // No pipeline cpu without cache
    CP_SINGLE_CACHE, // No pipeline cpu with cache
    CP_PIPE_NO_HAZARD, // Pipelined cpu without hazard unit and without cache
    CP_PIPE // Full pipelined cpu
};

class MachineConfigCache {
public:
    MachineConfigCache();
    MachineConfigCache(const MachineConfigCache *cc);
    MachineConfigCache(const QSettings*, const QString &prefix = "");

    void store(QSettings*, const QString &prefix = "");

    void preset(enum ConfigPresets);

    enum ReplacementPolicy {
        RP_RAND, // Random
        RP_LRU, // Least recently used
        RP_LFU // Least frequently used
    };

    enum WritePolicy {
        WP_THROUGH_NOALLOC, // Write through
        WP_THROUGH_ALLOC, // Write through
        WP_BACK // Write back
    };

    // If cache should be used or not
    void set_enabled(bool);
    void set_sets(unsigned); // Number of sets
    void set_blocks(unsigned); // Number of blocks
    void set_associativity(unsigned); // Degree of associativity
    void set_replacement_policy(enum ReplacementPolicy);
    void set_write_policy(enum WritePolicy);

    bool enabled() const;
    unsigned sets() const;
    unsigned blocks() const;
    unsigned associativity() const;
    enum ReplacementPolicy replacement_policy() const;
    enum WritePolicy write_policy() const;

    bool operator ==(const MachineConfigCache &c) const;
    bool operator !=(const MachineConfigCache &c) const;

private:
    bool en;
    unsigned n_sets, n_blocks, d_associativity;
    enum ReplacementPolicy replac_pol;
    enum WritePolicy write_pol;
};

class MachineConfig {
public:
    MachineConfig();
    MachineConfig(const MachineConfig *cc);
    MachineConfig(const QSettings*, const QString &prefix = "");

    void store(QSettings*, const QString &prefix = "");

    void preset(enum ConfigPresets);

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
    // Hazard unit
    void set_hazard_unit(enum HazardUnit);
    bool set_hazard_unit(QString hukind);
    // Protect data memory from execution. Only program sections can be executed.
    void set_memory_execute_protection(bool);
    // Protect program memory from accidental writes.
    void set_memory_write_protection(bool);
    // Set memory access times. Passed value is in cycles.
    void set_memory_access_time_read(unsigned);
    void set_memory_access_time_write(unsigned);
    void set_memory_access_time_burst(unsigned);
    // Operating system and exceptions setup
    void set_osemu_enable(bool);
    void set_osemu_known_syscall_stop(bool);
    void set_osemu_unknown_syscall_stop(bool);
    void set_osemu_interrupt_stop(bool);
    void set_osemu_exception_stop(bool);
    void set_osemu_fs_root(QString v);
    // reset machine befor internal compile/reload after external make
    void set_reset_at_compile(bool);
    // Set path to source elf file. This has to be set before core is initialized.
    void set_elf(QString path);
    // Configure cache
    void set_cache_program(const MachineConfigCache&);
    void set_cache_data(const MachineConfigCache&);

    bool pipelined() const;
    bool delay_slot() const;
    enum HazardUnit hazard_unit() const;
    bool memory_execute_protection() const;
    bool memory_write_protection() const;
    unsigned memory_access_time_read() const;
    unsigned memory_access_time_write() const;
    unsigned memory_access_time_burst() const;
    bool osemu_enable() const;
    bool osemu_known_syscall_stop() const;
    bool osemu_unknown_syscall_stop() const;
    bool osemu_interrupt_stop() const;
    bool osemu_exception_stop() const;
    QString osemu_fs_root() const;
    bool reset_at_compile() const;
    QString elf() const;
    const MachineConfigCache &cache_program() const;
    const MachineConfigCache &cache_data() const;

    MachineConfigCache *access_cache_program();
    MachineConfigCache *access_cache_data();

    bool operator ==(const MachineConfig &c) const;
    bool operator !=(const MachineConfig &c) const;

private:
    bool pipeline, delayslot;
    enum HazardUnit hunit;
    bool exec_protect, write_protect;
    unsigned mem_acc_read, mem_acc_write, mem_acc_burst;
    bool osem_enable, osem_known_syscall_stop, osem_unknown_syscall_stop;
    bool osem_interrupt_stop, osem_exception_stop;
    bool res_at_compile;
    QString osem_fs_root;
    QString elf_path;
    MachineConfigCache cch_program, cch_data;
};

}

Q_DECLARE_METATYPE(machine::MachineConfigCache)

#endif // MACHINECONFIG_H
