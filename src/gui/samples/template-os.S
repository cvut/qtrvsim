//  Template file with defines of system calls
//  QtRVSim simulator https://github.com/cvut/qtrvsim/
//
//  template-os.S       - example file
//
//  (C) 2021 by Pavel Pisa
//      e-mail:   pisa@cmp.felk.cvut.cz
//      homepage: http://cmp.felk.cvut.cz/~pisa
//      work:     http://www.pikron.com/
//      license:  public domain

// Directives to make interesting windows visible
#pragma qtrvsim show terminal
#pragma qtrvsim show registers
#pragma qtrvsim show cop0dock
#pragma qtrvsim show memory

.globl _start
.globl __start
.set noreorder
.ent _start

// Linux kernel compatible system calls subset

.equ __NR_exit,        93  // void exit(int status)
.equ __NR_read,        63  // ssize_t read(int fd, void *buf, size_t count)
.equ __NR_write,       64  // ssize_t write(int fd, const void *buf, size_t count)
.equ __NR_close,       57  // int close(int fd)
.equ __NR_openat,      56  // int openat(int fd, const char *pathname, int flags, mode_t mode)
	// use fd = -100 for normal open behaviour. Full openat not supported.
.equ __NR_brk,         214 // void * brk(void *addr)
.equ __NR_ftruncate64, 46  // int ftruncate64(int fd, off_t length)
.equ __NR_readv,       65  // ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
.equ __NR_writev,      66  // ssize_t writev(int fd, const struct iovec *iov, int iovcnt)

.text

__start:
_start:
	addi  a7, zero, __NR_write        // load syscall number
	addi  a0, zero, 1                 // load file descriptor
	addi  a1, zero, text_1            // load text address
	addi  a2, zero, text_1_e - text_1 // load text length
	ecall                             // print the text

	addi  a7, zero, __NR_exit         // load syscall numver
	addi  a0, zero, 0                 // load status argument
	ecall                             // exit

final:
	ebreak                            // request developer interaction
	jal   zero, final

.end _start

.data
.org 0x200

data_1:	.word	1, 2, 3, 4

text_1:	.ascii	"Hello world.\n"	// store ASCII text, no termination
text_1_e:
