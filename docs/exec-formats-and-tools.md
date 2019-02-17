QtMips - Supported Executable Formats and Toolchains
====================================================

The simulator accepts ELF statically linked executables
compiled for 32-bit big-endian MISP target.
Optimal is selection of some basic/initial mips
architecture and the use of plain mips-elf GCC toolchain.

Then the next simple template can be used to compile
assembly source codes.

```
#define zero	$0
#define at	$1
#define v0 	$2
#define v1	$3
#define a0	$4
#define a1	$5
#define a2	$6
#define	a3	$7

#define t0	$8
#define t1	$9
#define t2	$10
#define t3	$11
#define t4	$12
#define t5	$13
#define t6	$14
#define t7	$15
#define t8	$24
#define t9	$25
#define k0	$26
#define k1	$27

#define s0	$16
#define s1	$17
#define s2	$18
#define s3	$19
#define s4	$20
#define s5	$21
#define s6	$22
#define s7	$23

#define gp	$28
#define sp	$29
#define fp	$30
#define ra	$31

.globl _start
.set noat
.set noreorder
.ent _start

_start:

	// write the code there

loop:	break
	beq  zero, zero, loop
	nop

.end _start

```


The simulator recognizes 'break' instruction
compiled into the program. When 'break' instruction
is fetched, exception propagates through the pipeline.
The simulator waits till the pipeline is empty
and stops the continuous execution.

PC is set to the next instruction in the program
flow. When a single step or continuous execution
is activated again then the program continues after the 'break' instruction.

Another option is to set up "hardware" breakpoint for a given
instruction address.  When the instruction from address marked
by breakpoint is fetched then the exception propagates through
the pipeline, all externally visible effects of instruction marked
by breakpoint are masked out in memory phase, all following
instructions in the pipeline are discarded and execution stops.
PC is set to the address of instruction causing the exception
or to the branch instruction address if the exception occurs in
delay slot. When the single step or continuous execution
is requested again then  the "hardware" breakpoint exception
in the fetch stage is masked for the first executed instruction which.
But then CPU accepts breakpoint exceptions again. This is why it
is not a good idea to set up breakpoint to address of an instruction
in the delay slot.

To compile simple assembly code programs invoke GCC with next options.
```
mips-elf-gcc -ggdb -c program.S -o program.o
mips-elf-gcc -ggdb -nostdlib -nodefaultlibs -nostartfiles program.o -o program
```

Compile Executables with 'mips-linux-gnu' Toolchain
---------------------------------------------------

The Linux targetting toolchains use a MIPS O32 ABI calling
convention which allows building position-independent
binaries (PIC, PIE) and overcome missing PC relative support
in the basic MIPS instruction set. The ABI uses strictly
convention equivalent to calling each function indirectly
through 't9' register ('jalr t9'). The known value pointing
to a start of called function allows computing Global Offset
Table (GOT) and global data pointers values from known offset
and 't9' register value.  The startup code which conforms
these requirements to call 'main()' function looks like this:


```
/* file crt0local.S - replacement of C library complete startup machinery */
.globl main
.globl __start
.set noat
.set noreorder
.ent __start

.text

__start:
        bal     next
        nop
next:   .set    noreorder
        .cpload $31
        .set    reorder

        addi    $a0, $zero, 0
        addi    $a1, $zero, 0
        jal     main
quit:
        addi    $a0, $zero, 0
        addi    $v0, $zero, 4001  /* SYS_exit */

        syscall

loop:   break
        beq     $zero, $zero, loop
        nop

.end __start
```

The sequence of relative jump and link to next
instruction setups return address register 'ra'
to the value of the next instruction after 'bal',
and the delay slot. Actual translated code replaces
pseudo-instruction '.cpload' by sequence adding
offset into global data area from actual instruction
address and stores result in the 'gp'


```
00400500 <__start>:
  400500:       04110001        bal     400508 <next>
  400504:       00000000        nop
00400508 <next>:
        .cpload $31
  400508:       3c1c000a        lui     gp,0xa
  40050c:       279c6238        addiu   gp,gp,25144
  400510:       039fe021        addu    gp,gp,ra
  400514:       20040000        addi    a0,zero,0
  400518:       20050000        addi    a1,zero,0
  40051c:       8f998018        lw      t9,-32744(gp)
  400520:       0411ffc6        bal     40043c <main>
  400524:       00000000        nop
00400528 <quit>:
  400528:       20040000        addi    a0,zero,0
  40052c:       20020fa1        addi    v0,zero,4001
  400530:       0000000c        syscall
00400534 <loop>:
  400534:       0000000d        break
  400538:       1000fffe        b       400534 <loop>
  40053c:       00000000        nop
```

It can be seen that assembler not only expands '.cpload'
but even marks 'jalr    t9' instruction as a target for
link time optimization. Because 'main' function is near
(offset less than 128 kB) to the startup code, the 'jal'
instruction is replaced by 'bal main' during linking phase.


Then simple main function which outputs string to the
serial port provided by QtMisp emulator can be implemented
as:

```
/* serial-port-test.c */
#include <stdint.h>

#define SERIAL_PORT_BASE   0xffffc000
#define SERP_ST_REG_o            0x00
#define SERP_ST_REG_TX_BUSY_m     0x1
#define SERP_TX_REG_o            0x04

static inline void serp_write_reg(uint32_t base, uint32_t reg, uint32_t val)
{
  *(volatile uint32_t *)(base + reg) = val;
}
static inline uint32_t serp_read_reg(uint32_t base, uint32_t reg)
{
  return *(volatile uint32_t *)(base + reg);
}

void serp_tx_byte(int data)
{
  while (serp_read_reg(SERIAL_PORT_BASE, SERP_ST_REG_o) &
                SERP_ST_REG_TX_BUSY_m);
  serp_write_reg(SERIAL_PORT_BASE, SERP_TX_REG_o, data);
}

int main(void)
{
  const char *text = "Hello world.\nI am alive.\n";
  const char *s;

  for (s = text; *s; s++)
    serp_tx_byte(*s);

  return 0;
}
```

Compilation:


```
mips-linux-gnu-gcc -ggdb -fno-lto -c crt0local.S -o crt0local.o
mips-linux-gnu-gcc -ggdb -O3 -fno-lto  -c serial-port-test.c -o serial-port-test.o
mips-linux-gnu-gcc -ggdb -nostartfiles -static -fno-lto crt0local.o serial-port-test.o -o serial-port-test
```

The simulator implements 'rdhwr $rd, userlocal' instrcution
and allows code compiled agains musl C library to start as well.

```
mips-linux-gnu-gcc -ggdb -O1 -march=mips2 -static -specs=/opt/musl/mips-linux-gnu/lib/musl-gcc.specs  -c program.c -o program.o
mips-linux-gnu-gcc -ggdb -march=mips2 -static -specs=/opt/musl/mips-linux-gnu/lib/musl-gcc.specs programo -o program
```

The C library startup code which support both PIC/PIE and non-PIC environment

```
/* minimal replacement of crt0.o which is else provided by C library */
/* this variant support both static and PIC/PIE environments */

.globl main
.globl _start
.globl __start
.set noat
.set noreorder
.ent _start

.text

__start:
_start:
#if defined(__PIC__) || defined(__pic__)
	bal     next
	nop
next:
	.set    noreorder
	.cpload $31
	.set    reorder
#else
	la      $gp, _gp
#endif
	addi    $a0, $zero, 0
	addi    $a1, $zero, 0
	jal     main
	nop
quit:
	addi    $a0, $zero, 0
	addi    $v0, $zero, 4001  /* SYS_exit */

	syscall

loop:	break
        beq     $zero, $zero, loop
	nop

.end _start
```
