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
#define DF_MEM_ACC_BURST 0
#define DF_ELF QString("")
//////////////////////////////////////////////////////////////////////////////
/// Default config of MachineConfigCache
#define DFC_EN false
#define DFC_SETS 1
#define DFC_BLOCKS 1
#define DFC_ASSOC 1
#define DFC_REPLAC RP_RAND
#define DFC_WRITE WP_THROUGH_NOALLOC
//////////////////////////////////////////////////////////////////////////////

MachineConfigCache::MachineConfigCache() {
    en = DFC_EN;
    n_sets = DFC_SETS;
    n_blocks = DFC_BLOCKS;
    d_associativity = DFC_ASSOC;
    replac_pol = DFC_REPLAC;
    write_pol = DFC_WRITE;
}

MachineConfigCache::MachineConfigCache(const MachineConfigCache *cc) {
    en = cc->enabled();
    n_sets = cc->sets();
    n_blocks = cc->blocks();
    d_associativity = cc->associativity();
    replac_pol = cc->replacement_policy();
    write_pol = cc->write_policy();
}

#define N(STR) (prefix + QString(STR))

MachineConfigCache::MachineConfigCache(const QSettings *sts, const QString &prefix) {
    en = sts->value(N("Enabled"), DFC_EN).toBool();
    n_sets = sts->value(N("Sets"), DFC_SETS).toUInt();
    n_blocks = sts->value(N("Blocks"), DFC_BLOCKS).toUInt();
    d_associativity = sts->value(N("Associativity"), DFC_ASSOC).toUInt();
    replac_pol = (enum ReplacementPolicy)sts->value(N("Replacement"), DFC_REPLAC).toUInt();
    write_pol = (enum WritePolicy)sts->value(N("Write"), DFC_WRITE).toUInt();
}

void MachineConfigCache::store(QSettings *sts, const QString &prefix) {
    sts->setValue(N("Enabled"), enabled());
    sts->setValue(N("Sets"), sets());
    sts->setValue(N("Blocks"), blocks());
    sts->setValue(N("Associativity"), associativity());
    sts->setValue(N("Replacement"), (unsigned)replacement_policy());
    sts->setValue(N("Write"), (unsigned)write_policy());
}

#undef N

void MachineConfigCache::preset(enum ConfigPresets p) {
    switch (p) {
    case CP_PIPE:
    case CP_SINGLE_CACHE:
        set_enabled(true);
        set_sets(4);
        set_blocks(2);
        set_associativity(2);
        set_replacement_policy(RP_RAND);
        set_write_policy(WP_THROUGH_NOALLOC);
        break;
    case CP_SINGLE:
    case CP_PIPE_NO_HAZARD:
        set_enabled(false);
    }
}

void MachineConfigCache::set_enabled(bool v) {
    en = v;
}

void MachineConfigCache::set_sets(unsigned v) {
    n_sets = v > 0 ? v : 1;
}

void MachineConfigCache::set_blocks(unsigned v) {
    n_blocks = v > 0 ? v : 1;
}

void MachineConfigCache::set_associativity(unsigned v) {
    d_associativity = v > 0 ? v : 1;
}

void MachineConfigCache::set_replacement_policy(enum ReplacementPolicy v) {
    replac_pol = v;
}

void MachineConfigCache::set_write_policy(enum WritePolicy v) {
    write_pol = v;
}

bool MachineConfigCache::enabled() const {
    return en;
}

unsigned MachineConfigCache::sets() const {
    return n_sets;
}

unsigned MachineConfigCache::blocks() const {
    return n_blocks;
}

unsigned MachineConfigCache::associativity() const {
    return d_associativity;
}

enum MachineConfigCache::ReplacementPolicy MachineConfigCache::replacement_policy() const {
    return replac_pol;
}

enum MachineConfigCache::WritePolicy MachineConfigCache::write_policy() const {
    return write_pol;
}

bool MachineConfigCache::operator==(const MachineConfigCache &c) const {
#define CMP(GETTER) (GETTER)() == (c.GETTER)()
    return CMP(enabled) && \
            CMP(sets) && \
            CMP(blocks) && \
            CMP(associativity) && \
            CMP(replacement_policy) && \
            CMP(write_policy);
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
    mem_acc_burst = DF_MEM_ACC_BURST;
    osem_enable = true;
    osem_known_syscall_stop = true;
    osem_unknown_syscall_stop = true;
    osem_interrupt_stop = true;
    osem_exception_stop = true;
    osem_fs_root = "";
    res_at_compile = true;
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
    mem_acc_burst = cc->memory_access_time_burst();
    osem_enable = cc->osemu_enable();
    osem_known_syscall_stop = cc->osemu_known_syscall_stop();
    osem_unknown_syscall_stop = cc->osemu_unknown_syscall_stop();
    osem_interrupt_stop = cc->osemu_interrupt_stop();
    osem_exception_stop = cc->osemu_exception_stop();
    osem_fs_root = cc->osemu_fs_root();
    res_at_compile = cc->reset_at_compile();
    elf_path = cc->elf();
    cch_program = cc->cache_program();
    cch_data = cc->cache_data();
}

#define N(STR) (prefix + QString(STR))

MachineConfig::MachineConfig(const QSettings *sts, const QString &prefix) {
    pipeline = sts->value(N("Pipelined"), DF_PIPELINE).toBool();
    delayslot = sts->value(N("DelaySlot"), DF_DELAYSLOT).toBool();
    hunit = (enum HazardUnit)sts->value(N("HazardUnit"), DF_HUNIT).toUInt();
    exec_protect = sts->value(N("MemoryExecuteProtection"), DF_EXEC_PROTEC).toBool();
    write_protect = sts->value(N("MemoryWriteProtection"), DF_WRITE_PROTEC).toBool();
    mem_acc_read = sts->value(N("MemoryRead"), DF_MEM_ACC_READ).toUInt();
    mem_acc_write = sts->value(N("MemoryWrite"), DF_MEM_ACC_WRITE).toUInt();
    mem_acc_burst = sts->value(N("MemoryBurts"), DF_MEM_ACC_BURST).toUInt();
    osem_enable = sts->value(N("OsemuEnable"), true).toBool();
    osem_known_syscall_stop = sts->value(N("OsemuKnownSyscallStop"), true).toBool();
    osem_unknown_syscall_stop = sts->value(N("OsemuUnknownSyscallStop"), true).toBool();
    osem_interrupt_stop = sts->value(N("OsemuInterruptStop"), true).toBool();
    osem_exception_stop = sts->value(N("OsemuExceptionStop"), true).toBool();
    osem_fs_root = sts->value(N("OsemuFilesystemRoot"), "").toString();
    res_at_compile = sts->value(N("ResetAtCompile"), true).toBool();
    elf_path = sts->value(N("Elf"), DF_ELF).toString();
    cch_program = MachineConfigCache(sts, N("ProgramCache_"));
    cch_data = MachineConfigCache(sts, N("DataCache_"));
}

void MachineConfig::store(QSettings *sts, const QString &prefix) {
    sts->setValue(N("Pipelined"), pipelined());
    sts->setValue(N("DelaySlot"), delay_slot());
    sts->setValue(N("HazardUnit"), (unsigned)hazard_unit());
    sts->setValue(N("MemoryRead"), memory_access_time_read());
    sts->setValue(N("MemoryWrite"), memory_access_time_write());
    sts->setValue(N("MemoryBurts"), memory_access_time_burst());
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
}

#undef N

void MachineConfig::preset(enum ConfigPresets p) {
    // Note: we set just a minimal subset to get preset (preserving as much of hidden configuration as possible)
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

void MachineConfig::set_memory_access_time_burst(unsigned v) {
    mem_acc_burst = v;
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
    osem_fs_root = v;
}

void MachineConfig::set_reset_at_compile(bool v) {
    res_at_compile = v;
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
    return mem_acc_read > 1 ? mem_acc_read : 1;
}

unsigned MachineConfig::memory_access_time_write() const {
    return mem_acc_write > 1 ? mem_acc_write : 1;
}

unsigned MachineConfig::memory_access_time_burst() const {
    return mem_acc_burst;
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
            CMP(memory_access_time_burst) && \
            CMP(elf) && \
            CMP(cache_program) && \
            CMP(cache_data);
#undef CMP
}

bool MachineConfig::operator!=(const MachineConfig &c) const {
    return !operator==(c);
}
