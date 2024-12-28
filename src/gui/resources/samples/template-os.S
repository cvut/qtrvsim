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
#pragma qtrvsim show csrdock
#pragma qtrvsim show memory

.globl _start
.globl __start
.option norelax

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
	la    a1, text_1                  // load text start address
	la    a2, text_1_e                // load text end address
	sub   a2, a2, a1                  // compute text length
	ecall                             // print the text

	addi  a7, zero, __NR_exit         // load syscall numver
	addi  a0, zero, 0                 // load status argument
	ecall                             // exit

final:
	ebreak                            // request developer interaction
	jal   zero, final

.data
.org 0x400

data_1:	.word	1, 2, 3, 4

text_1:	.ascii	"Hello world.\n"	// store ASCII text, no termination
text_1_e:

// The sample can be compiled by full-featured riscv64-unknown-elf GNU tool-chain
// for RV32IMA use
// riscv64-unknown-elf-gcc -c -march=rv64ima -mabi=lp64 template-os.S
// riscv64-unknown-elf-gcc -march=rv64ima -mabi=lp64 -nostartfiles -nostdlib template-os.o
// for RV64IMA use
// riscv64-unknown-elf-gcc -c -march=rv32ima -mabi=ilp32 template-os.S
// riscv64-unknown-elf-gcc -march=rv32ima -mabi=ilp32 -nostartfiles -nostdlib template-os.o
// add "-o template-os" to change default "a.out" output file name
