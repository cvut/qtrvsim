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

#include "utils.h"
#include "core.h"
#include "ossyscall.h"
#include "syscall_nr.h"
#include "errno.h"
#include "target_errno.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

using namespace machine;
using namespace osemu;

// The copyied from musl-libc

#define TARGET_O_CREAT        0400
#define TARGET_O_EXCL        02000
#define TARGET_O_NOCTTY      04000
#define TARGET_O_TRUNC       01000
#define TARGET_O_APPEND       0010
#define TARGET_O_NONBLOCK     0200
#define TARGET_O_DSYNC        0020
#define TARGET_O_SYNC       040020
#define TARGET_O_RSYNC      040020
#define TARGET_O_DIRECTORY 0200000
#define TARGET_O_NOFOLLOW  0400000
#define TARGET_O_CLOEXEC  02000000

#define TARGET_O_PATH    010000000

#define TARGET_O_SYNC1      040000

#define TARGET_O_ACCMODE (03|TARGET_O_PATH)
#define TARGET_O_RDONLY  00
#define TARGET_O_WRONLY  01
#define TARGET_O_RDWR    02

static const QMap<int, int> map_target_o_flags_to_o_flags = {
    #ifdef O_CREAT
        {TARGET_O_CREAT, O_CREAT},
    #endif
    #ifdef O_EXCL
        {TARGET_O_EXCL, O_EXCL},
    #endif
    #ifdef O_NOCTTY
        {TARGET_O_NOCTTY, O_NOCTTY},
    #endif
    #ifdef O_TRUNC
        {TARGET_O_TRUNC, O_TRUNC},
    #endif
    #ifdef O_APPEND
        {TARGET_O_APPEND, O_APPEND},
    #endif
    #ifdef O_NONBLOCK
        {TARGET_O_NONBLOCK, O_NONBLOCK},
    #endif
    #ifdef O_DSYNC
        {TARGET_O_DSYNC, O_DSYNC},
    #endif
    #ifdef O_SYNC
        {TARGET_O_SYNC1, O_SYNC},
    #endif
    #ifdef O_RSYNC
        {TARGET_O_SYNC1, O_RSYNC},
    #endif
    #ifdef O_DIRECTORY
        {TARGET_O_DIRECTORY, O_DIRECTORY},
    #endif
    #ifdef O_NOFOLLOW
        {TARGET_O_NOFOLLOW, O_NOFOLLOW},
    #endif
    #ifdef O_CLOEXEC
        {TARGET_O_CLOEXEC, O_CLOEXEC},
    #endif
};

#if defined(S_IRUSR) & defined(S_IWUSR) & defined(S_IRGRP) & defined(S_IWGRP) & defined(S_IROTH) & defined(S_IWOTH)
#define OPEN_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
#else
#define OPEN_MODE 0
#endif

// The list copyied from musl-libc

static const QMap<int, int> errno_map = {
    #ifdef EPERM
        {EPERM, TARGET_EPERM},
    #endif
    #ifdef ENOENT
        {ENOENT, TARGET_ENOENT},
    #endif
    #ifdef ESRCH
        {ESRCH, TARGET_ESRCH},
    #endif
    #ifdef EINTR
        {EINTR, TARGET_EINTR},
    #endif
    #ifdef EIO
        {EIO, TARGET_EIO},
    #endif
    #ifdef ENXIO
        {ENXIO, TARGET_ENXIO},
    #endif
    #ifdef E2BIG
        {E2BIG, TARGET_E2BIG},
    #endif
    #ifdef ENOEXEC
        {ENOEXEC, TARGET_ENOEXEC},
    #endif
    #ifdef EBADF
        {EBADF, TARGET_EBADF},
    #endif
    #ifdef ECHILD
        {ECHILD, TARGET_ECHILD},
    #endif
    #ifdef EAGAIN
        {EAGAIN, TARGET_EAGAIN},
    #endif
    #ifdef ENOMEM
        {ENOMEM, TARGET_ENOMEM},
    #endif
    #ifdef EACCES
        {EACCES, TARGET_EACCES},
    #endif
    #ifdef EFAULT
        {EFAULT, TARGET_EFAULT},
    #endif
    #ifdef ENOTBLK
        {ENOTBLK, TARGET_ENOTBLK},
    #endif
    #ifdef EBUSY
        {EBUSY, TARGET_EBUSY},
    #endif
    #ifdef EEXIST
        {EEXIST, TARGET_EEXIST},
    #endif
    #ifdef EXDEV
        {EXDEV, TARGET_EXDEV},
    #endif
    #ifdef ENODEV
        {ENODEV, TARGET_ENODEV},
    #endif
    #ifdef ENOTDIR
        {ENOTDIR, TARGET_ENOTDIR},
    #endif
    #ifdef EISDIR
        {EISDIR, TARGET_EISDIR},
    #endif
    #ifdef EINVAL
        {EINVAL, TARGET_EINVAL},
    #endif
    #ifdef ENFILE
        {ENFILE, TARGET_ENFILE},
    #endif
    #ifdef EMFILE
        {EMFILE, TARGET_EMFILE},
    #endif
    #ifdef ENOTTY
        {ENOTTY, TARGET_ENOTTY},
    #endif
    #ifdef ETXTBSY
        {ETXTBSY, TARGET_ETXTBSY},
    #endif
    #ifdef EFBIG
        {EFBIG, TARGET_EFBIG},
    #endif
    #ifdef ENOSPC
        {ENOSPC, TARGET_ENOSPC},
    #endif
    #ifdef ESPIPE
        {ESPIPE, TARGET_ESPIPE},
    #endif
    #ifdef EROFS
        {EROFS, TARGET_EROFS},
    #endif
    #ifdef EMLINK
        {EMLINK, TARGET_EMLINK},
    #endif
    #ifdef EPIPE
        {EPIPE, TARGET_EPIPE},
    #endif
    #ifdef EDOM
        {EDOM, TARGET_EDOM},
    #endif
    #ifdef ERANGE
        {ERANGE, TARGET_ERANGE},
    #endif
    #ifdef ENOMSG
        {ENOMSG, TARGET_ENOMSG},
    #endif
    #ifdef EIDRM
        {EIDRM, TARGET_EIDRM},
    #endif
    #ifdef ECHRNG
        {ECHRNG, TARGET_ECHRNG},
    #endif
    #ifdef EL2NSYNC
        {EL2NSYNC, TARGET_EL2NSYNC},
    #endif
    #ifdef EL3HLT
        {EL3HLT, TARGET_EL3HLT},
    #endif
    #ifdef EL3RST
        {EL3RST, TARGET_EL3RST},
    #endif
    #ifdef ELNRNG
        {ELNRNG, TARGET_ELNRNG},
    #endif
    #ifdef EUNATCH
        {EUNATCH, TARGET_EUNATCH},
    #endif
    #ifdef ENOCSI
        {ENOCSI, TARGET_ENOCSI},
    #endif
    #ifdef EL2HLT
        {EL2HLT, TARGET_EL2HLT},
    #endif
    #ifdef EDEADLK
        {EDEADLK, TARGET_EDEADLK},
    #endif
    #ifdef ENOLCK
        {ENOLCK, TARGET_ENOLCK},
    #endif
    #ifdef EBADE
        {EBADE, TARGET_EBADE},
    #endif
    #ifdef EBADR
        {EBADR, TARGET_EBADR},
    #endif
    #ifdef EXFULL
        {EXFULL, TARGET_EXFULL},
    #endif
    #ifdef ENOANO
        {ENOANO, TARGET_ENOANO},
    #endif
    #ifdef EBADRQC
        {EBADRQC, TARGET_EBADRQC},
    #endif
    #ifdef EBADSLT
        {EBADSLT, TARGET_EBADSLT},
    #endif
    #ifdef EDEADLOCK
        {EDEADLOCK, TARGET_EDEADLOCK},
    #endif
    #ifdef EBFONT
        {EBFONT, TARGET_EBFONT},
    #endif
    #ifdef ENOSTR
        {ENOSTR, TARGET_ENOSTR},
    #endif
    #ifdef ENODATA
        {ENODATA, TARGET_ENODATA},
    #endif
    #ifdef ETIME
        {ETIME, TARGET_ETIME},
    #endif
    #ifdef ENOSR
        {ENOSR, TARGET_ENOSR},
    #endif
    #ifdef ENONET
        {ENONET, TARGET_ENONET},
    #endif
    #ifdef ENOPKG
        {ENOPKG, TARGET_ENOPKG},
    #endif
    #ifdef EREMOTE
        {EREMOTE, TARGET_EREMOTE},
    #endif
    #ifdef ENOLINK
        {ENOLINK, TARGET_ENOLINK},
    #endif
    #ifdef EADV
        {EADV, TARGET_EADV},
    #endif
    #ifdef ESRMNT
        {ESRMNT, TARGET_ESRMNT},
    #endif
    #ifdef ECOMM
        {ECOMM, TARGET_ECOMM},
    #endif
    #ifdef EPROTO
        {EPROTO, TARGET_EPROTO},
    #endif
    #ifdef EDOTDOT
        {EDOTDOT, TARGET_EDOTDOT},
    #endif
    #ifdef EMULTIHOP
        {EMULTIHOP, TARGET_EMULTIHOP},
    #endif
    #ifdef EBADMSG
        {EBADMSG, TARGET_EBADMSG},
    #endif
    #ifdef ENAMETOOLONG
        {ENAMETOOLONG, TARGET_ENAMETOOLONG},
    #endif
    #ifdef EOVERFLOW
        {EOVERFLOW, TARGET_EOVERFLOW},
    #endif
    #ifdef ENOTUNIQ
        {ENOTUNIQ, TARGET_ENOTUNIQ},
    #endif
    #ifdef EBADFD
        {EBADFD, TARGET_EBADFD},
    #endif
    #ifdef EREMCHG
        {EREMCHG, TARGET_EREMCHG},
    #endif
    #ifdef ELIBACC
        {ELIBACC, TARGET_ELIBACC},
    #endif
    #ifdef ELIBBAD
        {ELIBBAD, TARGET_ELIBBAD},
    #endif
    #ifdef ELIBSCN
        {ELIBSCN, TARGET_ELIBSCN},
    #endif
    #ifdef ELIBMAX
        {ELIBMAX, TARGET_ELIBMAX},
    #endif
    #ifdef ELIBEXEC
        {ELIBEXEC, TARGET_ELIBEXEC},
    #endif
    #ifdef EILSEQ
        {EILSEQ, TARGET_EILSEQ},
    #endif
    #ifdef ENOSYS
        {ENOSYS, TARGET_ENOSYS},
    #endif
    #ifdef ELOOP
        {ELOOP, TARGET_ELOOP},
    #endif
    #ifdef ERESTART
        {ERESTART, TARGET_ERESTART},
    #endif
    #ifdef ESTRPIPE
        {ESTRPIPE, TARGET_ESTRPIPE},
    #endif
    #ifdef ENOTEMPTY
        {ENOTEMPTY, TARGET_ENOTEMPTY},
    #endif
    #ifdef EUSERS
        {EUSERS, TARGET_EUSERS},
    #endif
    #ifdef ENOTSOCK
        {ENOTSOCK, TARGET_ENOTSOCK},
    #endif
    #ifdef EDESTADDRREQ
        {EDESTADDRREQ, TARGET_EDESTADDRREQ},
    #endif
    #ifdef EMSGSIZE
        {EMSGSIZE, TARGET_EMSGSIZE},
    #endif
    #ifdef EPROTOTYPE
        {EPROTOTYPE, TARGET_EPROTOTYPE},
    #endif
    #ifdef ENOPROTOOPT
        {ENOPROTOOPT, TARGET_ENOPROTOOPT},
    #endif
    #ifdef EPROTONOSUPPORT
        {EPROTONOSUPPORT, TARGET_EPROTONOSUPPORT},
    #endif
    #ifdef ESOCKTNOSUPPORT
        {ESOCKTNOSUPPORT, TARGET_ESOCKTNOSUPPORT},
    #endif
    #ifdef EOPNOTSUPP
        {EOPNOTSUPP, TARGET_EOPNOTSUPP},
    #endif
    #ifdef ENOTSUP
        {ENOTSUP, TARGET_ENOTSUP},
    #endif
    #ifdef EPFNOSUPPORT
        {EPFNOSUPPORT, TARGET_EPFNOSUPPORT},
    #endif
    #ifdef EAFNOSUPPORT
        {EAFNOSUPPORT, TARGET_EAFNOSUPPORT},
    #endif
    #ifdef EADDRINUSE
        {EADDRINUSE, TARGET_EADDRINUSE},
    #endif
    #ifdef EADDRNOTAVAIL
        {EADDRNOTAVAIL, TARGET_EADDRNOTAVAIL},
    #endif
    #ifdef ENETDOWN
        {ENETDOWN, TARGET_ENETDOWN},
    #endif
    #ifdef ENETUNREACH
        {ENETUNREACH, TARGET_ENETUNREACH},
    #endif
    #ifdef ENETRESET
        {ENETRESET, TARGET_ENETRESET},
    #endif
    #ifdef ECONNABORTED
        {ECONNABORTED, TARGET_ECONNABORTED},
    #endif
    #ifdef ECONNRESET
        {ECONNRESET, TARGET_ECONNRESET},
    #endif
    #ifdef ENOBUFS
        {ENOBUFS, TARGET_ENOBUFS},
    #endif
    #ifdef EISCONN
        {EISCONN, TARGET_EISCONN},
    #endif
    #ifdef ENOTCONN
        {ENOTCONN, TARGET_ENOTCONN},
    #endif
    #ifdef EUCLEAN
        {EUCLEAN, TARGET_EUCLEAN},
    #endif
    #ifdef ENOTNAM
        {ENOTNAM, TARGET_ENOTNAM},
    #endif
    #ifdef ENAVAIL
        {ENAVAIL, TARGET_ENAVAIL},
    #endif
    #ifdef EISNAM
        {EISNAM, TARGET_EISNAM},
    #endif
    #ifdef EREMOTEIO
        {EREMOTEIO, TARGET_EREMOTEIO},
    #endif
    #ifdef ESHUTDOWN
        {ESHUTDOWN, TARGET_ESHUTDOWN},
    #endif
    #ifdef ETOOMANYREFS
        {ETOOMANYREFS, TARGET_ETOOMANYREFS},
    #endif
    #ifdef ETIMEDOUT
        {ETIMEDOUT, TARGET_ETIMEDOUT},
    #endif
    #ifdef ECONNREFUSED
        {ECONNREFUSED, TARGET_ECONNREFUSED},
    #endif
    #ifdef EHOSTDOWN
        {EHOSTDOWN, TARGET_EHOSTDOWN},
    #endif
    #ifdef EHOSTUNREACH
        {EHOSTUNREACH, TARGET_EHOSTUNREACH},
    #endif
    #ifdef EWOULDBLOCK
        {EWOULDBLOCK, TARGET_EWOULDBLOCK},
    #endif
    #ifdef EALREADY
        {EALREADY, TARGET_EALREADY},
    #endif
    #ifdef EINPROGRESS
        {EINPROGRESS, TARGET_EINPROGRESS},
    #endif
    #ifdef ESTALE
        {ESTALE, TARGET_ESTALE},
    #endif
    #ifdef ECANCELED
        {ECANCELED, TARGET_ECANCELED},
    #endif
    #ifdef ENOMEDIUM
        {ENOMEDIUM, TARGET_ENOMEDIUM},
    #endif
    #ifdef EMEDIUMTYPE
        {EMEDIUMTYPE, TARGET_EMEDIUMTYPE},
    #endif
    #ifdef ENOKEY
        {ENOKEY, TARGET_ENOKEY},
    #endif
    #ifdef EKEYEXPIRED
        {EKEYEXPIRED, TARGET_EKEYEXPIRED},
    #endif
    #ifdef EKEYREVOKED
        {EKEYREVOKED, TARGET_EKEYREVOKED},
    #endif
    #ifdef EKEYREJECTED
        {EKEYREJECTED, TARGET_EKEYREJECTED},
    #endif
    #ifdef EOWNERDEAD
        {EOWNERDEAD, TARGET_EOWNERDEAD},
    #endif
    #ifdef ENOTRECOVERABLE
        {ENOTRECOVERABLE, TARGET_ENOTRECOVERABLE},
    #endif
    #ifdef ERFKILL
        {ERFKILL, TARGET_ERFKILL},
    #endif
    #ifdef EHWPOISON
        {EHWPOISON, TARGET_EHWPOISON},
    #endif
    #ifdef EDQUOT
        {EDQUOT, TARGET_EDQUOT},
    #endif
};

std::uint32_t result_errno_if_error(std::uint32_t result) {
    if (result < (std::uint32_t)-4096)
        return result;
    result = (std::uint32_t)-errno_map.value(errno);
    if (!result)
        result = (std::uint32_t)-1;
    return result;
}

int status_from_result(std::uint32_t result) {
    if (result < (std::uint32_t)-4096)
        return 0;
    else
        return -result;
}

typedef int (OsSyscallExceptionHandler::*syscall_handler_t)
              (std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8);

struct mips_syscall_desc_t {
    const char *name;
    unsigned int args;
    syscall_handler_t handler;
};

// The list copyied from QEMU

#  define MIPS_SYS(name, args, handler) {#name, args, \
        &OsSyscallExceptionHandler::handler},
static const mips_syscall_desc_t mips_syscall_args[] = {
        MIPS_SYS(sys_syscall    , 8, syscall_default_handler)    /* 4000 */
        MIPS_SYS(sys_exit       , 1, do_sys_exit)
        MIPS_SYS(sys_fork       , 0, syscall_default_handler)
        MIPS_SYS(sys_read       , 3, do_sys_read)
        MIPS_SYS(sys_write      , 3, do_sys_write)
        MIPS_SYS(sys_open       , 3, do_sys_open)    /* 4005 */
        MIPS_SYS(sys_close      , 1, do_sys_close)
        MIPS_SYS(sys_waitpid    , 3, syscall_default_handler)
        MIPS_SYS(sys_creat      , 2, syscall_default_handler)
        MIPS_SYS(sys_link       , 2, syscall_default_handler)
        MIPS_SYS(sys_unlink     , 1, syscall_default_handler)    /* 4010 */
        MIPS_SYS(sys_execve     , 0, syscall_default_handler)
        MIPS_SYS(sys_chdir      , 1, syscall_default_handler)
        MIPS_SYS(sys_time       , 1, syscall_default_handler)
        MIPS_SYS(sys_mknod      , 3, syscall_default_handler)
        MIPS_SYS(sys_chmod      , 2, syscall_default_handler)    /* 4015 */
        MIPS_SYS(sys_lchown     , 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_stat */
        MIPS_SYS(sys_lseek      , 3, syscall_default_handler)
        MIPS_SYS(sys_getpid     , 0, syscall_default_handler)    /* 4020 */
        MIPS_SYS(sys_mount      , 5, syscall_default_handler)
        MIPS_SYS(sys_umount     , 1, syscall_default_handler)
        MIPS_SYS(sys_setuid     , 1, syscall_default_handler)
        MIPS_SYS(sys_getuid     , 0, syscall_default_handler)
        MIPS_SYS(sys_stime      , 1, syscall_default_handler)    /* 4025 */
        MIPS_SYS(sys_ptrace     , 4, syscall_default_handler)
        MIPS_SYS(sys_alarm      , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_fstat */
        MIPS_SYS(sys_pause      , 0, syscall_default_handler)
        MIPS_SYS(sys_utime      , 2, syscall_default_handler)    /* 4030 */
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_access     , 2, syscall_default_handler)
        MIPS_SYS(sys_nice       , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* 4035 */
        MIPS_SYS(sys_sync       , 0, syscall_default_handler)
        MIPS_SYS(sys_kill       , 2, syscall_default_handler)
        MIPS_SYS(sys_rename     , 2, syscall_default_handler)
        MIPS_SYS(sys_mkdir      , 2, syscall_default_handler)
        MIPS_SYS(sys_rmdir      , 1, syscall_default_handler)    /* 4040 */
        MIPS_SYS(sys_dup        , 1, syscall_default_handler)
        MIPS_SYS(sys_pipe       , 0, syscall_default_handler)
        MIPS_SYS(sys_times      , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_brk        , 1, do_sys_brk)    /* 4045 */
        MIPS_SYS(sys_setgid     , 1, syscall_default_handler)
        MIPS_SYS(sys_getgid     , 0, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was signal(2, syscall_default_handler) */
        MIPS_SYS(sys_geteuid    , 0, syscall_default_handler)
        MIPS_SYS(sys_getegid    , 0, syscall_default_handler)    /* 4050 */
        MIPS_SYS(sys_acct       , 0, syscall_default_handler)
        MIPS_SYS(sys_umount2    , 2, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_ioctl      , 3, syscall_default_handler)
        MIPS_SYS(sys_fcntl      , 3, syscall_default_handler)    /* 4055 */
        MIPS_SYS(sys_ni_syscall , 2, syscall_default_handler)
        MIPS_SYS(sys_setpgid    , 2, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_olduname   , 1, syscall_default_handler)
        MIPS_SYS(sys_umask      , 1, syscall_default_handler)    /* 4060 */
        MIPS_SYS(sys_chroot     , 1, syscall_default_handler)
        MIPS_SYS(sys_ustat      , 2, syscall_default_handler)
        MIPS_SYS(sys_dup2       , 2, syscall_default_handler)
        MIPS_SYS(sys_getppid    , 0, syscall_default_handler)
        MIPS_SYS(sys_getpgrp    , 0, syscall_default_handler)    /* 4065 */
        MIPS_SYS(sys_setsid     , 0, syscall_default_handler)
        MIPS_SYS(sys_sigaction  , 3, syscall_default_handler)
        MIPS_SYS(sys_sgetmask   , 0, syscall_default_handler)
        MIPS_SYS(sys_ssetmask   , 1, syscall_default_handler)
        MIPS_SYS(sys_setreuid   , 2, syscall_default_handler)    /* 4070 */
        MIPS_SYS(sys_setregid   , 2, syscall_default_handler)
        MIPS_SYS(sys_sigsuspend , 0, syscall_default_handler)
        MIPS_SYS(sys_sigpending , 1, syscall_default_handler)
        MIPS_SYS(sys_sethostname, 2, syscall_default_handler)
        MIPS_SYS(sys_setrlimit  , 2, syscall_default_handler)    /* 4075 */
        MIPS_SYS(sys_getrlimit  , 2, syscall_default_handler)
        MIPS_SYS(sys_getrusage  , 2, syscall_default_handler)
        MIPS_SYS(sys_gettimeofday, 2, syscall_default_handler)
        MIPS_SYS(sys_settimeofday, 2, syscall_default_handler)
        MIPS_SYS(sys_getgroups  , 2, syscall_default_handler)    /* 4080 */
        MIPS_SYS(sys_setgroups  , 2, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* old_select */
        MIPS_SYS(sys_symlink    , 2, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_lstat */
        MIPS_SYS(sys_readlink   , 3, syscall_default_handler)    /* 4085 */
        MIPS_SYS(sys_uselib     , 1, syscall_default_handler)
        MIPS_SYS(sys_swapon     , 2, syscall_default_handler)
        MIPS_SYS(sys_reboot     , 3, syscall_default_handler)
        MIPS_SYS(old_readdir    , 3, syscall_default_handler)
        MIPS_SYS(old_mmap       , 6, syscall_default_handler)    /* 4090 */
        MIPS_SYS(sys_munmap     , 2, syscall_default_handler)
        MIPS_SYS(sys_truncate   , 2, syscall_default_handler)
        MIPS_SYS(sys_ftruncate  , 2, do_sys_ftruncate)
        MIPS_SYS(sys_fchmod     , 2, syscall_default_handler)
        MIPS_SYS(sys_fchown     , 3, syscall_default_handler)    /* 4095 */
        MIPS_SYS(sys_getpriority, 2, syscall_default_handler)
        MIPS_SYS(sys_setpriority, 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_statfs     , 2, syscall_default_handler)
        MIPS_SYS(sys_fstatfs    , 2, syscall_default_handler)    /* 4100 */
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was ioperm(2, syscall_default_handler) */
        MIPS_SYS(sys_socketcall , 2, syscall_default_handler)
        MIPS_SYS(sys_syslog     , 3, syscall_default_handler)
        MIPS_SYS(sys_setitimer  , 3, syscall_default_handler)
        MIPS_SYS(sys_getitimer  , 2, syscall_default_handler)    /* 4105 */
        MIPS_SYS(sys_newstat    , 2, syscall_default_handler)
        MIPS_SYS(sys_newlstat   , 2, syscall_default_handler)
        MIPS_SYS(sys_newfstat   , 2, syscall_default_handler)
        MIPS_SYS(sys_uname      , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* 4110 was iopl(2, syscall_default_handler) */
        MIPS_SYS(sys_vhangup    , 0, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_idle(, syscall_default_handler) */
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_vm86 */
        MIPS_SYS(sys_wait4      , 4, syscall_default_handler)
        MIPS_SYS(sys_swapoff    , 1, syscall_default_handler)    /* 4115 */
        MIPS_SYS(sys_sysinfo    , 1, syscall_default_handler)
        MIPS_SYS(sys_ipc        , 6, syscall_default_handler)
        MIPS_SYS(sys_fsync      , 1, syscall_default_handler)
        MIPS_SYS(sys_sigreturn  , 0, syscall_default_handler)
        MIPS_SYS(sys_clone      , 6, syscall_default_handler)    /* 4120 */
        MIPS_SYS(sys_setdomainname, 2, syscall_default_handler)
        MIPS_SYS(sys_newuname   , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* sys_modify_ldt */
        MIPS_SYS(sys_adjtimex   , 1, syscall_default_handler)
        MIPS_SYS(sys_mprotect   , 3, syscall_default_handler)    /* 4125 */
        MIPS_SYS(sys_sigprocmask, 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was create_module */
        MIPS_SYS(sys_init_module, 5, syscall_default_handler)
        MIPS_SYS(sys_delete_module, 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* 4130 was get_kernel_syms */
        MIPS_SYS(sys_quotactl   , 0, syscall_default_handler)
        MIPS_SYS(sys_getpgid    , 1, syscall_default_handler)
        MIPS_SYS(sys_fchdir     , 1, syscall_default_handler)
        MIPS_SYS(sys_bdflush    , 2, syscall_default_handler)
        MIPS_SYS(sys_sysfs      , 3, syscall_default_handler)    /* 4135 */
        MIPS_SYS(sys_personality        , 1, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* for afs_syscall */
        MIPS_SYS(sys_setfsuid   , 1, syscall_default_handler)
        MIPS_SYS(sys_setfsgid   , 1, syscall_default_handler)
        MIPS_SYS(sys_llseek     , 5, syscall_default_handler)    /* 4140 */
        MIPS_SYS(sys_getdents   , 3, syscall_default_handler)
        MIPS_SYS(sys_select     , 5, syscall_default_handler)
        MIPS_SYS(sys_flock      , 2, syscall_default_handler)
        MIPS_SYS(sys_msync      , 3, syscall_default_handler)
        MIPS_SYS(sys_readv      , 3, do_sys_readv)    /* 4145 */
        MIPS_SYS(sys_writev     , 3, do_sys_writev)
        MIPS_SYS(sys_cacheflush , 3, syscall_default_handler)
        MIPS_SYS(sys_cachectl   , 3, syscall_default_handler)
        MIPS_SYS(sys_sysmips    , 4, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* 4150 */
        MIPS_SYS(sys_getsid     , 1, syscall_default_handler)
        MIPS_SYS(sys_fdatasync  , 0, syscall_default_handler)
        MIPS_SYS(sys_sysctl     , 1, syscall_default_handler)
        MIPS_SYS(sys_mlock      , 2, syscall_default_handler)
        MIPS_SYS(sys_munlock    , 2, syscall_default_handler)    /* 4155 */
        MIPS_SYS(sys_mlockall   , 1, syscall_default_handler)
        MIPS_SYS(sys_munlockall , 0, syscall_default_handler)
        MIPS_SYS(sys_sched_setparam, 2, syscall_default_handler)
        MIPS_SYS(sys_sched_getparam, 2, syscall_default_handler)
        MIPS_SYS(sys_sched_setscheduler, 3, syscall_default_handler)     /* 4160 */
        MIPS_SYS(sys_sched_getscheduler, 1, syscall_default_handler)
        MIPS_SYS(sys_sched_yield, 0, syscall_default_handler)
        MIPS_SYS(sys_sched_get_priority_max, 1, syscall_default_handler)
        MIPS_SYS(sys_sched_get_priority_min, 1, syscall_default_handler)
        MIPS_SYS(sys_sched_rr_get_interval, 2, syscall_default_handler)  /* 4165 */
        MIPS_SYS(sys_nanosleep, 2, syscall_default_handler)
        MIPS_SYS(sys_mremap     , 5, syscall_default_handler)
        MIPS_SYS(sys_accept     , 3, syscall_default_handler)
        MIPS_SYS(sys_bind       , 3, syscall_default_handler)
        MIPS_SYS(sys_connect    , 3, syscall_default_handler)    /* 4170 */
        MIPS_SYS(sys_getpeername, 3, syscall_default_handler)
        MIPS_SYS(sys_getsockname, 3, syscall_default_handler)
        MIPS_SYS(sys_getsockopt , 5, syscall_default_handler)
        MIPS_SYS(sys_listen     , 2, syscall_default_handler)
        MIPS_SYS(sys_recv       , 4, syscall_default_handler)    /* 4175 */
        MIPS_SYS(sys_recvfrom   , 6, syscall_default_handler)
        MIPS_SYS(sys_recvmsg    , 3, syscall_default_handler)
        MIPS_SYS(sys_send       , 4, syscall_default_handler)
        MIPS_SYS(sys_sendmsg    , 3, syscall_default_handler)
        MIPS_SYS(sys_sendto     , 6, syscall_default_handler)    /* 4180 */
        MIPS_SYS(sys_setsockopt , 5, syscall_default_handler)
        MIPS_SYS(sys_shutdown   , 2, syscall_default_handler)
        MIPS_SYS(sys_socket     , 3, syscall_default_handler)
        MIPS_SYS(sys_socketpair , 4, syscall_default_handler)
        MIPS_SYS(sys_setresuid  , 3, syscall_default_handler)    /* 4185 */
        MIPS_SYS(sys_getresuid  , 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* was sys_query_module */
        MIPS_SYS(sys_poll       , 3, syscall_default_handler)
        MIPS_SYS(sys_nfsservctl , 3, syscall_default_handler)
        MIPS_SYS(sys_setresgid  , 3, syscall_default_handler)    /* 4190 */
        MIPS_SYS(sys_getresgid  , 3, syscall_default_handler)
        MIPS_SYS(sys_prctl      , 5, syscall_default_handler)
        MIPS_SYS(sys_rt_sigreturn, 0, syscall_default_handler)
        MIPS_SYS(sys_rt_sigaction, 4, syscall_default_handler)
        MIPS_SYS(sys_rt_sigprocmask, 4, syscall_default_handler) /* 4195 */
        MIPS_SYS(sys_rt_sigpending, 2, syscall_default_handler)
        MIPS_SYS(sys_rt_sigtimedwait, 4, syscall_default_handler)
        MIPS_SYS(sys_rt_sigqueueinfo, 3, syscall_default_handler)
        MIPS_SYS(sys_rt_sigsuspend, 0, syscall_default_handler)
        MIPS_SYS(sys_pread64    , 6, syscall_default_handler)    /* 4200 */
        MIPS_SYS(sys_pwrite64   , 6, syscall_default_handler)
        MIPS_SYS(sys_chown      , 3, syscall_default_handler)
        MIPS_SYS(sys_getcwd     , 2, syscall_default_handler)
        MIPS_SYS(sys_capget     , 2, syscall_default_handler)
        MIPS_SYS(sys_capset     , 2, syscall_default_handler)    /* 4205 */
        MIPS_SYS(sys_sigaltstack, 2, syscall_default_handler)
        MIPS_SYS(sys_sendfile   , 4, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_mmap2      , 6, do_sys_mmap2)               /* 4210 */
        MIPS_SYS(sys_truncate64 , 4, syscall_default_handler)
        MIPS_SYS(sys_ftruncate64, 4, syscall_default_handler)
        MIPS_SYS(sys_stat64     , 2, syscall_default_handler)
        MIPS_SYS(sys_lstat64    , 2, syscall_default_handler)
        MIPS_SYS(sys_fstat64    , 2, syscall_default_handler)    /* 4215 */
        MIPS_SYS(sys_pivot_root , 2, syscall_default_handler)
        MIPS_SYS(sys_mincore    , 3, syscall_default_handler)
        MIPS_SYS(sys_madvise    , 3, syscall_default_handler)
        MIPS_SYS(sys_getdents64 , 3, syscall_default_handler)
        MIPS_SYS(sys_fcntl64    , 3, syscall_default_handler)    /* 4220 */
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)
        MIPS_SYS(sys_gettid     , 0, syscall_default_handler)
        MIPS_SYS(sys_readahead  , 5, syscall_default_handler)
        MIPS_SYS(sys_setxattr   , 5, syscall_default_handler)
        MIPS_SYS(sys_lsetxattr  , 5, syscall_default_handler)    /* 4225 */
        MIPS_SYS(sys_fsetxattr  , 5, syscall_default_handler)
        MIPS_SYS(sys_getxattr   , 4, syscall_default_handler)
        MIPS_SYS(sys_lgetxattr  , 4, syscall_default_handler)
        MIPS_SYS(sys_fgetxattr  , 4, syscall_default_handler)
        MIPS_SYS(sys_listxattr  , 3, syscall_default_handler)    /* 4230 */
        MIPS_SYS(sys_llistxattr , 3, syscall_default_handler)
        MIPS_SYS(sys_flistxattr , 3, syscall_default_handler)
        MIPS_SYS(sys_removexattr        , 2, syscall_default_handler)
        MIPS_SYS(sys_lremovexattr, 2, syscall_default_handler)
        MIPS_SYS(sys_fremovexattr, 2, syscall_default_handler)   /* 4235 */
        MIPS_SYS(sys_tkill      , 2, syscall_default_handler)
        MIPS_SYS(sys_sendfile64 , 5, syscall_default_handler)
        MIPS_SYS(sys_futex      , 6, syscall_default_handler)
        MIPS_SYS(sys_sched_setaffinity, 3, syscall_default_handler)
        MIPS_SYS(sys_sched_getaffinity, 3, syscall_default_handler)      /* 4240 */
        MIPS_SYS(sys_io_setup   , 2, syscall_default_handler)
        MIPS_SYS(sys_io_destroy , 1, syscall_default_handler)
        MIPS_SYS(sys_io_getevents, 5, syscall_default_handler)
        MIPS_SYS(sys_io_submit  , 3, syscall_default_handler)
        MIPS_SYS(sys_io_cancel  , 3, syscall_default_handler)    /* 4245 */
        MIPS_SYS(sys_exit_group , 1, syscall_default_handler)
        MIPS_SYS(sys_lookup_dcookie, 3, syscall_default_handler)
        MIPS_SYS(sys_epoll_create, 1, syscall_default_handler)
        MIPS_SYS(sys_epoll_ctl  , 4, syscall_default_handler)
        MIPS_SYS(sys_epoll_wait , 3, syscall_default_handler)    /* 4250 */
        MIPS_SYS(sys_remap_file_pages, 5, syscall_default_handler)
        MIPS_SYS(sys_set_tid_address, 1, syscall_default_handler)
        MIPS_SYS(sys_restart_syscall, 0, syscall_default_handler)
        MIPS_SYS(sys_fadvise64_64, 7, syscall_default_handler)
        MIPS_SYS(sys_statfs64   , 3, syscall_default_handler)    /* 4255 */
        MIPS_SYS(sys_fstatfs64  , 2, syscall_default_handler)
        MIPS_SYS(sys_timer_create, 3, syscall_default_handler)
        MIPS_SYS(sys_timer_settime, 4, syscall_default_handler)
        MIPS_SYS(sys_timer_gettime, 2, syscall_default_handler)
        MIPS_SYS(sys_timer_getoverrun, 1, syscall_default_handler)       /* 4260 */
        MIPS_SYS(sys_timer_delete, 1, syscall_default_handler)
        MIPS_SYS(sys_clock_settime, 2, syscall_default_handler)
        MIPS_SYS(sys_clock_gettime, 2, syscall_default_handler)
        MIPS_SYS(sys_clock_getres, 2, syscall_default_handler)
        MIPS_SYS(sys_clock_nanosleep, 4, syscall_default_handler)        /* 4265 */
        MIPS_SYS(sys_tgkill     , 3, syscall_default_handler)
        MIPS_SYS(sys_utimes     , 2, syscall_default_handler)
        MIPS_SYS(sys_mbind      , 4, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* sys_get_mempolicy */
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* 4270 sys_set_mempolicy */
        MIPS_SYS(sys_mq_open    , 4, syscall_default_handler)
        MIPS_SYS(sys_mq_unlink  , 1, syscall_default_handler)
        MIPS_SYS(sys_mq_timedsend, 5, syscall_default_handler)
        MIPS_SYS(sys_mq_timedreceive, 5, syscall_default_handler)
        MIPS_SYS(sys_mq_notify  , 2, syscall_default_handler)    /* 4275 */
        MIPS_SYS(sys_mq_getsetattr, 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* sys_vserver */
        MIPS_SYS(sys_waitid     , 4, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall , 0, syscall_default_handler)    /* available, was setaltroot */
        MIPS_SYS(sys_add_key    , 5, syscall_default_handler)
        MIPS_SYS(sys_request_key, 4, syscall_default_handler)
        MIPS_SYS(sys_keyctl     , 5, syscall_default_handler)
        MIPS_SYS(sys_set_thread_area, 1, do_sys_set_thread_area)
        MIPS_SYS(sys_inotify_init, 0, syscall_default_handler)
        MIPS_SYS(sys_inotify_add_watch, 3, syscall_default_handler) /* 4285 */
        MIPS_SYS(sys_inotify_rm_watch, 2, syscall_default_handler)
        MIPS_SYS(sys_migrate_pages, 4, syscall_default_handler)
        MIPS_SYS(sys_openat, 4, syscall_default_handler)
        MIPS_SYS(sys_mkdirat, 3, syscall_default_handler)
        MIPS_SYS(sys_mknodat, 4, syscall_default_handler)        /* 4290 */
        MIPS_SYS(sys_fchownat, 5, syscall_default_handler)
        MIPS_SYS(sys_futimesat, 3, syscall_default_handler)
        MIPS_SYS(sys_fstatat64, 4, syscall_default_handler)
        MIPS_SYS(sys_unlinkat, 3, syscall_default_handler)
        MIPS_SYS(sys_renameat, 4, syscall_default_handler)       /* 4295 */
        MIPS_SYS(sys_linkat, 5, syscall_default_handler)
        MIPS_SYS(sys_symlinkat, 3, syscall_default_handler)
        MIPS_SYS(sys_readlinkat, 4, syscall_default_handler)
        MIPS_SYS(sys_fchmodat, 3, syscall_default_handler)
        MIPS_SYS(sys_faccessat, 3, syscall_default_handler)      /* 4300 */
        MIPS_SYS(sys_pselect6, 6, syscall_default_handler)
        MIPS_SYS(sys_ppoll, 5, syscall_default_handler)
        MIPS_SYS(sys_unshare, 1, syscall_default_handler)
        MIPS_SYS(sys_splice, 6, syscall_default_handler)
        MIPS_SYS(sys_sync_file_range, 7, syscall_default_handler) /* 4305 */
        MIPS_SYS(sys_tee, 4, syscall_default_handler)
        MIPS_SYS(sys_vmsplice, 4, syscall_default_handler)
        MIPS_SYS(sys_move_pages, 6, syscall_default_handler)
        MIPS_SYS(sys_set_robust_list, 2, syscall_default_handler)
        MIPS_SYS(sys_get_robust_list, 3, syscall_default_handler) /* 4310 */
        MIPS_SYS(sys_kexec_load, 4, syscall_default_handler)
        MIPS_SYS(sys_getcpu, 3, syscall_default_handler)
        MIPS_SYS(sys_epoll_pwait, 6, syscall_default_handler)
        MIPS_SYS(sys_ioprio_set, 3, syscall_default_handler)
        MIPS_SYS(sys_ioprio_get, 2, syscall_default_handler)
        MIPS_SYS(sys_utimensat, 4, syscall_default_handler)
        MIPS_SYS(sys_signalfd, 3, syscall_default_handler)
        MIPS_SYS(sys_ni_syscall, 0, syscall_default_handler)     /* was timerfd */
        MIPS_SYS(sys_eventfd, 1, syscall_default_handler)
        MIPS_SYS(sys_fallocate, 6, syscall_default_handler)      /* 4320 */
        MIPS_SYS(sys_timerfd_create, 2, syscall_default_handler)
        MIPS_SYS(sys_timerfd_gettime, 2, syscall_default_handler)
        MIPS_SYS(sys_timerfd_settime, 4, syscall_default_handler)
        MIPS_SYS(sys_signalfd4, 4, syscall_default_handler)
        MIPS_SYS(sys_eventfd2, 2, syscall_default_handler)       /* 4325 */
        MIPS_SYS(sys_epoll_create1, 1, syscall_default_handler)
        MIPS_SYS(sys_dup3, 3, syscall_default_handler)
        MIPS_SYS(sys_pipe2, 2, syscall_default_handler)
        MIPS_SYS(sys_inotify_init1, 1, syscall_default_handler)
        MIPS_SYS(sys_preadv, 5, syscall_default_handler)         /* 4330 */
        MIPS_SYS(sys_pwritev, 5, syscall_default_handler)
        MIPS_SYS(sys_rt_tgsigqueueinfo, 4, syscall_default_handler)
        MIPS_SYS(sys_perf_event_open, 5, syscall_default_handler)
        MIPS_SYS(sys_accept4, 4, syscall_default_handler)
        MIPS_SYS(sys_recvmmsg, 5, syscall_default_handler)       /* 4335 */
        MIPS_SYS(sys_fanotify_init, 2, syscall_default_handler)
        MIPS_SYS(sys_fanotify_mark, 6, syscall_default_handler)
        MIPS_SYS(sys_prlimit64, 4, syscall_default_handler)
        MIPS_SYS(sys_name_to_handle_at, 5, syscall_default_handler)
        MIPS_SYS(sys_open_by_handle_at, 3, syscall_default_handler) /* 4340 */
        MIPS_SYS(sys_clock_adjtime, 2, syscall_default_handler)
        MIPS_SYS(sys_syncfs, 1, syscall_default_handler)
        MIPS_SYS(sys_sendmmsg, 4, syscall_default_handler)
        MIPS_SYS(sys_setns, 2, syscall_default_handler)
        MIPS_SYS(sys_process_vm_readv, 6, syscall_default_handler) /* 345 */
        MIPS_SYS(sys_process_vm_writev, 6, syscall_default_handler)
        MIPS_SYS(sys_kcmp, 5, syscall_default_handler)
        MIPS_SYS(sys_finit_module, 3, syscall_default_handler)
        MIPS_SYS(sys_sched_setattr, 2, syscall_default_handler)
        MIPS_SYS(sys_sched_getattr, 3, syscall_default_handler)  /* 350 */
        MIPS_SYS(sys_renameat2, 5, syscall_default_handler)
        MIPS_SYS(sys_seccomp, 3, syscall_default_handler)
        MIPS_SYS(sys_getrandom, 3, syscall_default_handler)
        MIPS_SYS(sys_memfd_create, 2, syscall_default_handler)
        MIPS_SYS(sys_bpf, 3, syscall_default_handler)            /* 355 */
        MIPS_SYS(sys_execveat, 5, syscall_default_handler)
        MIPS_SYS(sys_userfaultfd, 1, syscall_default_handler)
        MIPS_SYS(sys_membarrier, 2, syscall_default_handler)
        MIPS_SYS(sys_mlock2, 3, syscall_default_handler)
        MIPS_SYS(sys_copy_file_range, 6, syscall_default_handler) /* 360 */
        MIPS_SYS(sys_preadv2, 6, syscall_default_handler)
        MIPS_SYS(sys_pwritev2, 6, syscall_default_handler)
};

const unsigned mips_syscall_args_size =
        sizeof(mips_syscall_args)/sizeof(*mips_syscall_args);

OsSyscallExceptionHandler::OsSyscallExceptionHandler(bool known_syscall_stop,
                                                     bool unknown_syscall_stop,
                                                     QString fs_root) :
                                                     fd_mapping(3, FD_TERMINAL) {
    brk_limit = 0;
    anonymous_base = 0x60000000;
    anonymous_last = anonymous_base;
    this->known_syscall_stop = known_syscall_stop;
    this->unknown_syscall_stop = unknown_syscall_stop;
    this->fs_root = fs_root;
}

bool OsSyscallExceptionHandler::handle_exception(Core *core, Registers *regs,
                            ExceptionCause excause, std::uint32_t inst_addr,
                            std::uint32_t next_addr, std::uint32_t jump_branch_pc,
                            bool in_delay_slot, std::uint32_t mem_ref_addr) {
    unsigned int syscall_num = regs->read_gp(2);
    const mips_syscall_desc_t *sdesc;
    std::uint32_t a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0, a6 = 0, a7 = 0, a8 = 0;
    std::uint32_t sp = regs->read_gp(29);
    std::uint32_t result;
    int status;

    MemoryAccess *mem_data = core->get_mem_data();
    MemoryAccess *mem_program = core->get_mem_program();
    (void)mem_program;

#if 1
    printf("Exception cause %d instruction PC 0x%08lx next PC 0x%08lx jump branch PC 0x%08lx "
           "in_delay_slot %d registers PC 0x%08lx mem ref 0x%08lx\n",
           excause, (unsigned long)inst_addr, (unsigned long)next_addr,
           (unsigned long)jump_branch_pc, (int)in_delay_slot,
           (unsigned long)regs->read_pc(), (unsigned long)mem_ref_addr);
#else
    (void)excause; (void)inst_addr; (void)next_addr; (void)mem_ref_addr; (void)regs;
    (void)jump_branch_pc; (void)in_delay_slot;
#endif

    if (syscall_num < 4000 || syscall_num >= 4000 + mips_syscall_args_size)
        throw QTMIPS_EXCEPTION(SyscallUnknown, "System call number unknown ", QString::number(syscall_num));

    syscall_num -= 4000;

    sdesc = &mips_syscall_args[syscall_num];
    a1 = regs->read_gp(4);
    a2 = regs->read_gp(5);
    a3 = regs->read_gp(6);
    a4 = regs->read_gp(7);

    switch (sdesc->args) {
    case 8:
        a8 = mem_data->read_word(sp + 28);
        FALLTROUGH
    case 7:
        a7 = mem_data->read_word(sp + 24);
        FALLTROUGH
    case 6:
        a6 = mem_data->read_word(sp + 20);
        FALLTROUGH
    case 5:
        a5 = mem_data->read_word(sp + 16);
        FALLTROUGH
    default:
        break;
    }

#if 1
    printf("Syscall %s number %d/0x%x a1 %ld a2 %ld a3 %ld a4 %ld\n",
           sdesc->name, syscall_num, syscall_num,
           (unsigned long)a1, (unsigned long)a2,
           (unsigned long)a3, (unsigned long)a4);

#endif
    status = (this->*sdesc->handler)(result, core, syscall_num,
                                      a1, a2, a3, a4, a5, a6, a7, a8);
    if (known_syscall_stop)
        emit core->stop_on_exception_reached();

    regs->write_gp(7, status);
    if (status < 0)
        regs->write_gp(2, status);
    else
        regs->write_gp(2, result);

    return true;
};

std::int32_t OsSyscallExceptionHandler::write_mem(machine::MemoryAccess *mem, std::uint32_t addr,
                   const QVector<std::uint8_t> &data, std::uint32_t count) {
    if ((std::uint32_t)data.size() < count)
        count = data.size();

    for (std::uint32_t i = 0; i < count; i++) {
        mem->write_byte(addr++, data[i]);
    }
    return count;
}

std::int32_t OsSyscallExceptionHandler::read_mem(machine::MemoryAccess *mem, std::uint32_t addr,
                   QVector<std::uint8_t> &data, std::uint32_t count) {
    data.resize(count);
    for (std::uint32_t i = 0; i < count; i++) {
        data[i] = mem->read_byte(addr++);
    }
    return count;
}

std::int32_t OsSyscallExceptionHandler::write_io(int fd, const QVector<std::uint8_t> &data,
                                                 std::uint32_t count) {
    if ((std::uint32_t)data.size() < count)
        count = data.size();
    if (fd == FD_UNUSED) {
        return -1;
    } else if (fd == FD_TERMINAL) {
        for (std::uint32_t i = 0; i < count; i++)
            emit char_written(fd, data[i]);
    } else {
        count = write(fd, data.data(), count);
    }
    return result_errno_if_error(count);
}

std::int32_t OsSyscallExceptionHandler::read_io(int fd, QVector<std::uint8_t> &data,
                                                std::uint32_t count, bool add_nl_at_eof) {
    data.resize(count);
    if ((std::uint32_t)data.size() < count)
        count = data.size();
    if (fd == FD_UNUSED) {
        return -1;
    } else if (fd == FD_TERMINAL) {
        for (std::uint32_t i = 0; i < count; i++) {
            unsigned int byte;
            bool available = false;
            emit rx_byte_pool(fd, byte, available);
            if (!available) {
                // add final newline if there are no more data
                if (add_nl_at_eof)
                    data[i] = '\n';
                count = i + 1;
                break;
            }
            data[i] = byte;
        }
    } else {
        count = read(fd, data.data(), count);
    }
    if ((std::int32_t)count >= 0)
        data.resize(count);
    return result_errno_if_error(count);
}

int OsSyscallExceptionHandler::allocate_fd(int val) {
    int i;
    for (i = 0 ; i < fd_mapping.size(); i++) {
        if (fd_mapping[i] == FD_UNUSED) {
            fd_mapping[i] = val;
            return i;
        }
    }
    i = fd_mapping.size();
    fd_mapping.resize(i + 1);
    fd_mapping[i] = val;
    return i;
}

int OsSyscallExceptionHandler::file_open(QString fname, int flags, int mode) {
    int targetfd, fd;
    int hostflags = 0;
    (void)mode;
    for (auto i = map_target_o_flags_to_o_flags.begin();
         i != map_target_o_flags_to_o_flags.end(); i++)
        if (flags & i.key())
            hostflags |= i.value();

    switch (flags & TARGET_O_ACCMODE) {
    case TARGET_O_RDONLY:
        hostflags |= O_RDONLY;
        break;
    case TARGET_O_WRONLY:
        hostflags |= O_WRONLY;
        break;
    case TARGET_O_RDWR:
        hostflags |= O_RDWR;
        break;
    }

    if (fs_root.size() == 0) {
        return allocate_fd(FD_TERMINAL);
    }

    fname = filepath_to_host(fname);

    fd = open(fname.toLatin1().data(), hostflags, OPEN_MODE);
    if (fd >= 0) {
        targetfd = allocate_fd(fd);
    } else {
        targetfd = result_errno_if_error(fd);
    }
    return targetfd;
}

int OsSyscallExceptionHandler::targetfd_to_fd(int targetfd) {
    if (targetfd < 0)
        return FD_INVALID;
    if (targetfd >= fd_mapping.size())
        return FD_INVALID;
    return fd_mapping.at(targetfd);
}

void OsSyscallExceptionHandler::close_fd(int targetfd) {
    if (targetfd <= fd_mapping.size())
        fd_mapping[targetfd] = FD_UNUSED;
}

QString OsSyscallExceptionHandler::filepath_to_host(QString path) {
    int pos = 0;
    int prev = 0;
    while(1) {
        if (((path.size() - pos == 2) && (path.mid(pos, -1) == "..")) ||
            ((path.size() - pos > 2) && (path.mid(pos, 3) == "../"))) {
            if (pos == 0) {
                prev = 0;
            } else {
                if (pos == 1)
                    prev = 0;
                else
                    prev = path.lastIndexOf('/', pos - 2) + 1;
            }
            path.remove(prev, pos + 3 - prev);
            pos = prev;
            continue;
        }
        pos = path.indexOf('/', pos);
        if (pos == -1)
            break;
        pos += 1;
    }
    if ((path.size() >= 1) && path.at(0) == '/')
        path = fs_root + path;
    else
        path = fs_root + '/' + path;
    return path;
}

int OsSyscallExceptionHandler::syscall_default_handler(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8)
{
    const mips_syscall_desc_t *sdesc = &mips_syscall_args[syscall_num];
#if 1
    printf("Unimplemented syscall %s number %d/0x%x a1 %ld a2 %ld a3 %ld a4 %ld\n",
           sdesc->name, syscall_num, syscall_num,
           (unsigned long)a1, (unsigned long)a2,
           (unsigned long)a3, (unsigned long)a4);

#endif
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;
    result = 0;
    if (unknown_syscall_stop)
        emit core->stop_on_exception_reached();
    return TARGET_ENOSYS;
}

// void exit(int status);
int OsSyscallExceptionHandler::do_sys_exit(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    int status = a1;

    printf("sys_exit status %d\n", status);
        emit core->stop_on_exception_reached();

    return 0;
}

int OsSyscallExceptionHandler::do_sys_set_thread_area(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;
    core->set_c0_userlocal(a1);
    result = 0;
    return 0;
}

// ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
int OsSyscallExceptionHandler::do_sys_writev(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    int fd = a1;
    std::uint32_t iov = a2;
    int iovcnt = a3;
    MemoryAccess *mem = core->get_mem_data();
    std::int32_t count;
    QVector<std::uint8_t> data;

    printf("sys_writev to fd %d\n", fd);

    fd = targetfd_to_fd(fd);
    if (fd == FD_INVALID) {
        result = -TARGET_EINVAL;
        return 0;
    }

    while (iovcnt-- > 0) {
        std::uint32_t iov_base = mem->read_word(iov);
        std::uint32_t iov_len = mem->read_word(iov + 4);
        iov += 8;

        read_mem(mem, iov_base, data, iov_len);
        count = write_io(fd, data, iov_len);
        if (count >= 0) {
            result += count;
        } else {
            if (result == 0)
                result = count;
        }
        if (count < (std::int32_t)iov_len)
            break;
    }

return status_from_result(result);
}

// ssize_t write(int fd, const void *buf, size_t count);
int OsSyscallExceptionHandler::do_sys_write(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    int fd = a1;
    std::uint32_t buf = a2;
    int size = a3;
    MemoryAccess *mem = core->get_mem_data();
    std::int32_t count;
    QVector<std::uint8_t> data;

    printf("sys_write to fd %d\n", fd);

    fd = targetfd_to_fd(fd);
    if (fd == FD_INVALID) {
        result = -TARGET_EINVAL;
        return 0;
    }

    read_mem(mem, buf, data, size);
    count = write_io(fd, data, size);

    result = count;

    return status_from_result(result);
}

// ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
int OsSyscallExceptionHandler::do_sys_readv(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    int fd = a1;
    std::uint32_t iov = a2;
    int iovcnt = a3;
    MemoryAccess *mem = core->get_mem_data();
    std::int32_t count;
    QVector<std::uint8_t> data;

    printf("sys_readv to fd %d\n", fd);

    fd = targetfd_to_fd(fd);
    if (fd == FD_INVALID) {
        result = -TARGET_EINVAL;
        return 0;
    }

    while (iovcnt-- > 0) {
        std::uint32_t iov_base = mem->read_word(iov);
        std::uint32_t iov_len = mem->read_word(iov + 4);
        iov += 8;

        count = read_io(fd, data, iov_len, true);
        if (count >= 0) {
            write_mem(mem, iov_base, data, count);
            result += count;
        } else {
            if (result == 0)
                result = count;
        }
        if (count < (std::int32_t)iov_len)
            break;
    }

    return status_from_result(result);
}

// ssize_t read(int fd, void *buf, size_t count);
int OsSyscallExceptionHandler::do_sys_read(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    int fd = a1;
    std::uint32_t buf = a2;
    int size = a3;
    MemoryAccess *mem = core->get_mem_data();
    std::int32_t count;
    QVector<std::uint8_t> data;

    printf("sys_read to fd %d\n", fd);

    fd = targetfd_to_fd(fd);
    if (fd == FD_INVALID) {
        result = -TARGET_EINVAL;
        return 0;
    }

    result = 0;

    count = read_io(fd, data, size, true);
    if (count >= 0) {
        write_mem(mem, buf, data, size);
    }
    result = count;

    return status_from_result(result);
}

// int open(const char *pathname, int flags, mode_t mode);
int OsSyscallExceptionHandler::do_sys_open(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    std::uint32_t pathname = a1;
    int flags = a2;
    int mode = a3;
    std::uint32_t ch;
    MemoryAccess *mem = core->get_mem_data();

    printf("sys_open filename\n");

    QString fname;
    while (true) {
        ch = mem->read_byte(pathname++);
        if (ch == 0)
            break;
        fname.append(QChar(ch));
    }

    result = file_open(fname, flags, mode);

    return status_from_result(result);
}

// int close(int fd);
int OsSyscallExceptionHandler::do_sys_close(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    int fd = a1;

    printf("sys_close fd %d\n", fd);

    int targetfd = fd;
    fd = targetfd_to_fd(fd);
    if (fd == FD_INVALID) {
        result = -TARGET_EINVAL;
        return 0;
    }

    close(fd);
    close_fd(targetfd);

    return status_from_result(result);
}

// int ftruncate(int fd, off_t length);
int OsSyscallExceptionHandler::do_sys_ftruncate(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    int fd = a1;
    std::uint64_t length = ((std::uint64_t)a2 << 32) | a3;

    printf("sys_ftruncate fd %d\n", fd);

    fd = targetfd_to_fd(fd);
    if (fd == FD_INVALID) {
        result = -TARGET_EINVAL;
        return 0;
    }

    result = result_errno_if_error(ftruncate(fd, length));

    return status_from_result(result);
}

// int or void * brk(void *addr);
int OsSyscallExceptionHandler::do_sys_brk(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    std::uint32_t new_limit = a1;
    brk_limit = new_limit;
    result = brk_limit;

    return 0;
}

#define TARGET_SYSCALL_MMAP2_UNIT 4096ULL
#define TARGET_MAP_ANONYMOUS  0x20

// void *mmap2(void *addr, size_t length, int prot,
//             int flags, int fd, off_t pgoffset);
int OsSyscallExceptionHandler::do_sys_mmap2(std::uint32_t &result, Core *core,
               std::uint32_t syscall_num,
               std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
               std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
               std::uint32_t a7, std::uint32_t a8) {
    (void)core; (void)syscall_num;
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; (void)a8;

    result = 0;
    std::uint32_t addr = a1;
    std::uint32_t lenght = a2;
    std::uint32_t prot = a3;
    std::uint32_t flags = a4;
    std::uint32_t fd = a5;
    std::uint64_t offset = a6 * TARGET_SYSCALL_MMAP2_UNIT;

    printf("sys_mmap2 addr = 0x%08lx lenght= 0x%08lx prot = 0x%08lx flags = 0x%08lx fd = %d offset = 0x%08llx\n",
           (unsigned long)addr, (unsigned long)lenght, (unsigned long)prot,
           (unsigned long)flags, (int)fd, (unsigned long long) offset);

    lenght = (lenght  + TARGET_SYSCALL_MMAP2_UNIT - 1) & ~(TARGET_SYSCALL_MMAP2_UNIT - 1);
    anonymous_last = (anonymous_last + TARGET_SYSCALL_MMAP2_UNIT - 1) &
                     ~(TARGET_SYSCALL_MMAP2_UNIT - 1);
    result = anonymous_last;
    anonymous_last += lenght;

    return 0;
}
