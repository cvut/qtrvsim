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

#ifndef OSSYCALL_H
#define OSSYCALL_H

#include "qtmips_machine/alu.h"
#include "qtmips_machine/core.h"
#include "qtmips_machine/instruction.h"
#include "qtmips_machine/machineconfig.h"
#include "qtmips_machine/memory/backend/memory.h"
#include "qtmips_machine/memory/frontend_memory.h"
#include "qtmips_machine/qtmipsexception.h"
#include "qtmips_machine/registers.h"

#include <QObject>
#include <QString>
#include <QVector>

namespace osemu {

#define OSSYCALL_HANDLER_DECLARE(name)                                         \
    int name(                                                                  \
        uint32_t &result, machine::Core *core, uint32_t syscall_num,           \
        uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5,       \
        uint32_t a6, uint32_t a7, uint32_t a8)

class OsSyscallExceptionHandler : public machine::ExceptionHandler {
    Q_OBJECT
public:
    explicit OsSyscallExceptionHandler(
        bool known_syscall_stop = false,
        bool unknown_syscall_stop = false,
        QString fs_root = "");
    bool handle_exception(
        machine::Core *core,
        machine::Registers *regs,
        machine::ExceptionCause excause,
        machine::Address inst_addr,
        machine::Address next_addr,
        machine::Address jump_branch_pc,
        bool in_delay_slot,
        machine::Address mem_ref_addr) override;
    OSSYCALL_HANDLER_DECLARE(syscall_default_handler);
    OSSYCALL_HANDLER_DECLARE(do_sys_exit);
    OSSYCALL_HANDLER_DECLARE(do_sys_set_thread_area);
    OSSYCALL_HANDLER_DECLARE(do_sys_writev);
    OSSYCALL_HANDLER_DECLARE(do_sys_write);
    OSSYCALL_HANDLER_DECLARE(do_sys_readv);
    OSSYCALL_HANDLER_DECLARE(do_sys_read);
    OSSYCALL_HANDLER_DECLARE(do_sys_open);
    OSSYCALL_HANDLER_DECLARE(do_sys_close);
    OSSYCALL_HANDLER_DECLARE(do_sys_ftruncate);
    OSSYCALL_HANDLER_DECLARE(do_sys_brk);
    OSSYCALL_HANDLER_DECLARE(do_sys_mmap2);

    OSSYCALL_HANDLER_DECLARE(do_spim_print_integer);
    OSSYCALL_HANDLER_DECLARE(do_spim_print_string);
    OSSYCALL_HANDLER_DECLARE(do_spim_read_string);
    OSSYCALL_HANDLER_DECLARE(do_spim_sbrk);
    OSSYCALL_HANDLER_DECLARE(do_spim_exit);
    OSSYCALL_HANDLER_DECLARE(do_spim_print_character);
    OSSYCALL_HANDLER_DECLARE(do_spim_read_character);

signals:
    void char_written(int fd, unsigned int val);
    void rx_byte_pool(int fd, unsigned int &data, bool &available);

private:
    enum FdMapping {
        FD_UNUSED = -1,
        FD_INVALID = -1,
        FD_TERMINAL = -2,
    };
    int32_t write_mem(
        machine::FrontendMemory *mem,
        machine::Address addr,
        const QVector<uint8_t> &data,
        uint32_t count);
    int32_t read_mem(
        machine::FrontendMemory *mem,
        machine::Address addr,
        QVector<uint8_t> &data,
        uint32_t count);
    int32_t write_io(int fd, const QVector<uint8_t> &data, uint32_t count);
    int32_t read_io(
        int fd,
        QVector<uint8_t> &data,
        uint32_t count,
        bool add_nl_at_eof = false);
    int allocate_fd(int val = FD_UNUSED);
    int file_open(QString fname, int flags, int mode);
    int targetfd_to_fd(int targetfd);
    void close_fd(int targetfd);
    QString filepath_to_host(QString path);

    QVector<int> fd_mapping;
    uint32_t brk_limit;
    uint32_t anonymous_base;
    uint32_t anonymous_last;
    bool known_syscall_stop;
    bool unknown_syscall_stop;
    QString fs_root;
};

#undef OSSYCALL_HANDLER_DECLARE

} // namespace osemu

#endif // OSSYCALL_H
