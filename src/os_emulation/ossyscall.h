#ifndef OSSYCALL_H
#define OSSYCALL_H

#include "machine/core.h"
#include "machine/instruction.h"
#include "machine/machineconfig.h"
#include "machine/memory/backend/memory.h"
#include "machine/memory/frontend_memory.h"
#include "machine/registers.h"
#include "machine/simulator_exception.h"

#include <QObject>
#include <QString>
#include <QVector>

namespace osemu {

#define OSSYCALL_HANDLER_DECLARE(name)                                                             \
    int name(                                                                                      \
        uint64_t &result, machine::Core *core, uint64_t syscall_num, uint64_t a1, uint64_t a2,     \
        uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6)

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
        machine::Address mem_ref_addr) override;
    OSSYCALL_HANDLER_DECLARE(syscall_default_handler);
    OSSYCALL_HANDLER_DECLARE(do_sys_exit);
    OSSYCALL_HANDLER_DECLARE(do_sys_set_thread_area);
    OSSYCALL_HANDLER_DECLARE(do_sys_writev);
    OSSYCALL_HANDLER_DECLARE(do_sys_write);
    OSSYCALL_HANDLER_DECLARE(do_sys_readv);
    OSSYCALL_HANDLER_DECLARE(do_sys_read);
    OSSYCALL_HANDLER_DECLARE(do_sys_openat);
    OSSYCALL_HANDLER_DECLARE(do_sys_close);
    OSSYCALL_HANDLER_DECLARE(do_sys_ftruncate);
    OSSYCALL_HANDLER_DECLARE(do_sys_brk);
    OSSYCALL_HANDLER_DECLARE(do_sys_mmap);

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
    int32_t read_io(int fd, QVector<uint8_t> &data, uint32_t count, bool add_nl_at_eof = false);
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
