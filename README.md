# QtRVSim

![logo](data/icons/gui.svg)

RISC-V CPU simulator for education purposes.

## Ongoing Development

* QtRVSim - [RISC-V](https://riscv.org/) architecture based edition ([https://github.com/cvut/qtrvsim](https://github.com/cvut/qtrvsim)), suggested for future contributions
* QtMips development branch with little/endian support - [devel](https://github.com/cvut/QtMips/tree/devel)
* Space for QtMips examples and students' contributions [https://github.com/cvut/QtMips-Playground](https://github.com/cvut/QtMips-Playground)

Implemented to support following courses:

- [B35APO - Computer Architectures](https://cw.fel.cvut.cz/wiki/courses/b35apo)
- [B4M35PAP - Advanced Computer Architectures](https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start)

[Faculty of Electrical Engineering](http://www.fel.cvut.cz) [Czech Technical University](http://www.cvut.cz/)

## Table of contents

- [QtRVSim](#qtrvsim)
  - [Try it out! (WebAssembly)](#try-it-out-webassembly)
  - [Build and packages](#build-and-packages)
     - [Build Dependencies](#build-dependencies)
     - [General Compilation](#general-compilation)
     - [Building from source on macOS](#building-from-source-on-macos)
     - [Download Binary Packages](#download-binary-packages)
     - [Nix package](#nix-package)
     - [Tests](#tests)
  - [Documentation](#documentation)
  - [Accepted Binary Formats](#accepted-binary-formats)
     - [LLVM toolchain usage](#llvm-toolchain-usage)
     - [GNU toolchain usage](#gnu-toolchain-usage)
  - [Integrated Assembler](#integrated-assembler)
  - [Support to call external make utility](#support-to-call-external-make-utility)
  - [Advanced functionalities](#advanced-functionalities)
     - [Peripherals](#peripherals)
     - [Interrupts and Coprocessor 0 Support](#interrupts-and-coprocessor-0-support)
     - [System Calls Support](#system-calls-support)
  - [Special instructions support](#special-instructions-support)
  - [Limitations of the Implementation](#limitations-of-the-implementation)
     - [QtMips original limitations](#qtmips-original-limitations)
  - [List of Actually Supported Instructions](#list-of-actually-supported-instructions)
  - [Links to Resources and Similar Projects](#links-to-resources-and-similar-projects)
  - [Copyright](#copyright)
  - [License](#license)

## Try it out! (WebAssembly)

QtRVSim is experimentally available for [WebAssembly](https://webassembly.org/) and it can be run in most browsers
without installation. **[QtRVSim online](http://dev.jakubdupak.com/qtrvsim)**

**Note, that WebAssembly version is experimental.**
Please, report any difficulties via [GitHub issues](https://github.com/cvut/qtrvsim/issues/new).

## Build and packages

### Build Dependencies

- Qt 5 (minimal tested version is 5.9.5)
- elfutils (optional; libelf works too but there can be some problems)

### General Compilation

```shell
cmake -DCMAKE_BUILD_TYPE=Release /path/to/qtrvsim
make
```

Where `/path/to/qtrvsim` is path to this project root. The built binaries are to be found in the directory `target`
in the build directory (the one, where cmake was called).

### Building from source on macOS

Install the latest version of **Xcode** from the App Store. Then open a terminal and execute xcode-select --install to
install Command Line Tools. Then open Xcode, accept the license agreement and wait for it to install any additional
components. After you finally see the "Welcome to Xcode" screen, from the top bar choose Xcode -> Preferences ->
Locations -> Command Line Tools and select an SDK version.

Install [Homebrew](https://brew.sh/) and use it to install Qt and libelf. (__Installing libelf is optional. If libelf is
not found in the system, local fallback is used.__)

```shell
brew install qt libelf
```

Now build the project the same way as in general compilation (above).

### Download Binary Packages

- [https://github.com/cvut/qtrvsim/releases](https://github.com/cvut/qtrvsim/releases)
    - archives with Windows and generic GNU/Linux binaries
- [https://build.opensuse.org/repositories/home:jdupak/qtrvsim](https://build.opensuse.org/repositories/home:jdupak/qtrvsim)
- [https://software.opensuse.org/download.html?project=home%3Ajdupak&package=qtrvsim](https://software.opensuse.org/download.html?project=home%3Ajdupak&package=qtrvsim)
    - Open Build Service binary packages

### Nix package

QtRVSim provides a Nix package as a part of the repository. You can build and install it by a command bellow. Updates
have to be done manually by checking out the git. NIXPKGS package is planned.

```shell
nix-env -if .
```

### Tests

Tests are managed by CTest (part of CMake). To build and run all tests, use this commands:

```bash
cmake -DCMAKE_BUILD_TYPE=Release /path/to/qtRVSim
make
ctest
```

## Documentation

Main documentation is provided in this README and in subdirectories [`docs/user`](docs/user)
and [`docs/developer`](docs/developer).

The project has started as diploma thesis work of Karel Kočí. The complete text of the
thesis [Graphical CPU Simulator with Cache Visualization](https://dspace.cvut.cz/bitstream/handle/10467/76764/F3-DP-2018-Koci-Karel-diploma.pdf)
is available from the online archive of the [Czech Technical University in Prague](https://www.cvut.cz/). The document
provides analysis of available alternative simulators, overview of the project architecture and basic usage information.

The project was extended as bachelor's theses of Jakub Dupak and Max Hollmann. Links will be provided as soon as the
theses are released (**TODO**)

## Accepted Binary Formats

The simulator accepts ELF statically linked executables compiled for RISC-V target (`--march=rv64g`). The simulator will
automatically select endianness based on the ELF header. Simulation will execute as XLEN=32.

- 64bit simulation is not yet supported.
- Compressed instructions are not yet supported.

You can use compile the code for simulation using specialized RISC-V GCC/Binutils toolchain (`riscv32-elf`) or using
unified Clang/LLVM toolchain with [LLD](https://lld.llvm.org/). If you have Clang installed, you don't need any
additional tools. Clang can be used on Linux, Windows, macOS and others...

### LLVM toolchain usage

```shell
clang --target=riscv32 -march=rv64g -nostdlib -static -fuse-ld=lld test.S -o test
llvm-objdump -S test
```

### GNU toolchain usage

```shell
riscv32-elf-as test.S -o test.o
riscv32-elf-ld test.o -o test
riscv32-elf-objdump -S test
```

or

```shell
riscv32-elf-gcc test.S -o test
riscv32-elf-objdump -S test
```

## Integrated Assembler

Basic integrated assembler is included in the simulator. Small subset of
[GNU assembler](https://sourceware.org/binutils/docs/as/) directives is recognized as well. Next directives are
recognized: `.word`, `.orig`, `.set`
/`.equ`, `.ascii` and `.asciz`. Some other directives are simply ignored: `.data`, `.text`, `.globl`, `.end` and `.ent`.
This allows to write code which can be compiled by both - integrated and full-featured assembler. Addresses are assigned
to labels/symbols which are stored in symbol table. Addition, subtraction, multiplication, divide and bitwise and or are
recognized.

## Support to call external make utility

The action "Build executable by external make" call "make" program. If the action is invoked, and some source editors
selected in main windows tabs then the "make" is started in the corresponding directory. Else directory of last selected
editor is chosen. If no editor is open then directory of last loaded ELF executable are used as "make" start path. If
even that is not an option then default directory when the emulator has been started is used.

## Advanced functionalities

**THIS PART IS FROM MIPS EDITION AND HAS NOT BEEN TESTED ON RISC-V**

### Peripherals


The simulator implements emulation of two peripherals for now. Base addresses are selected such way that they are
accessible by 16 immediate offset which uses register 0 (`zero`) as base.

The first is simple serial port (UART). It support transmission
(Tx) and reception (Rx). Receiver status register (`SERP_RX_ST_REG`)
implements two bits. Read-only bit 0 (`SERP_RX_ST_REG_READY`)
is set to one if there is unread character available in the receiver data register (`SERP_RX_DATA_REG`). The bit 1
(`SERP_RX_ST_REG_IE`) can be written to 1 to enable interrupt request when unread character is available. The
transmitter status register (`SERP_TX_ST_REG`) bit 0
(SERP_TX_ST_REG_READY) signals by value 1 that UART is ready and can accept next character to be sent. The bit 1
(`SERP_TX_ST_REG_IE`) enables generation of interrupt. The register `SERP_TX_DATA_REG` is actual Tx buffer. The LSB byte
of written word is transmitted to the terminal window. Definition of peripheral base address and registers
offsets (`_o`) and individual fields masks (`_m`) follows

```
#define SERIAL_PORT_BASE   0xffffc000

#define SERP_RX_ST_REG_o           0x00
#define SERP_RX_ST_REG_READY_m      0x1
#define SERP_RX_ST_REG_IE_m         0x2

#define SERP_RX_DATA_REG_o         0x04

#define SERP_TX_ST_REG_o           0x08
#define SERP_TX_ST_REG_READY_m      0x1
#define SERP_TX_ST_REG_IE_m         0x2

#define SERP_TX_DATA_REG_o         0x0c
```

The UART registers region is mirrored on the address 0xffff0000 to enable use of programs initially written
for [SPIM](http://spimsimulator.sourceforge.net/)
or [MARS](http://courses.missouristate.edu/KenVollmar/MARS/) emulators.

The another peripheral allows to set three byte values concatenated to single word (read-only KNOBS_8BIT register)
from user panel set by knobs and display one word in hexadecimal, decimal and binary format (`LED_LINE` register). There
are two other words writable which control color of RGB LED 1 and 2
(registers `LED_RGB1` and `LED_RGB2`).

```
#define SPILED_REG_BASE    0xffffc100

#define SPILED_REG_LED_LINE_o           0x004
#define SPILED_REG_LED_RGB1_o           0x010
#define SPILED_REG_LED_RGB2_o           0x014
#define SPILED_REG_LED_KBDWR_DIRECT_o   0x018

#define SPILED_REG_KBDRD_KNOBS_DIRECT_o 0x020
#define SPILED_REG_KNOBS_8BIT_o         0x024
```

The simple 16-bit per pixel (RGB565) framebuffer and LCD are implemented. The framebuffer is mapped into range starting
at `LCD_FB_START`
address. The display size is 480 x 320 pixel. Pixel format RGB565 expect red component in bits 11..15, green component
in bits 5..10 and blue component in bits 0..4.

```
#define LCD_FB_START       0xffe00000
#define LCD_FB_END         0xffe4afff
```

Limitation: actual concept of memory view updates and access does not allow to reliably read peripheral registers, and
I/O memory content. It is possible to write into framebuffer memory when cached (from CPU perspective) access to memory
is selected.

### Interrupts and Coprocessor 0 Support

(NOTICE: Coprocessor0 will have to be replaced with RISC-V status registers)

List of interrupt sources:

| Irq number | Cause/Status Bit | Source                                       |
|-----------:|-----------------:|:---------------------------------------------|
| 2 / HW0    | 10               | Serial port ready to accept character to Tx  |
| 3 / HW1    | 11               | There is received character ready to be read |
| 7 / HW5    | 15               | Counter reached value in Compare register    |

Following coprocessor 0 registers are recognized

| Number | Name       | Description |
|-------:|:-----------|:------------|
|  $4,2  | UserLocal  | Used as TLS base by operating system usually |
|  $8,0  | BadVAddr   | Reports the address for the most recent address-related exception |
|  $9,0  | Count      | Processor cycle count |
| $11,0  | Compare    | Timer interrupt control |
| $12,0  | Status     | Processor status and control |
| $13,0  | Cause      | Cause of last exception |
| $14,0  | EPC        | Program counter at last exception |
| $15,1  | EBase      | Exception vector base register |
| $16,0  | Config     | Configuration registers |

`mtc0` and `mfc0` are used to copy value from/to general puropose registers to/from comprocessor 0 register.

Hardware/special registers implemented:

| Number | Name       | Description |
|-------:|:-----------|:------------|
|  0     | CPUNum     | CPU number, fixed to 0  |
|  1     | SYNCI_Step | Increment required for instruction cache synchronization |
|  2     | CC         | Cycle counter |
|  3     | CCRes      | Cycle counter resolution, fixed on 1 |
| 29     | UserLocal  | Read only value of Coprocessor 0 $4,2 register |

Sequence to enable serial port receive interrupt:

Decide location of interrupt service routine the first. The default address is 0x80000180. The base can be
changed (`EBase` register) and then PC is set to address EBase + 0x180. This is in accordance with MIPS release 1 and 2
manuals.

Enable bit 11 (interrupt mask) in the Status register. Ensure that bit 1 (`EXL`)
is zero and bit 0 (`IE`) is set to one.

Enable interrupt in the receiver status register (bit 1 of `SERP_RX_ST_REG`).

Write character to the terminal. It should be immediately consumed by the serial port receiver if interrupt is enabled
in `SERP_RX_ST_REG`. CPU should report interrupt exception and when it propagates to the execution phase `PC` is set to
the interrupt routine start address.

Some hints how to direct linker to place interrupt handler routine at appropriate address. Implement interrupt routine
in new section

```
.section .irq_handler, "ax"
```

Use next linker option to place section start at right address

```
 -Wl,--section-start=.irq_handler=0x80000180
```

### System Calls Support

(Not tested for RISC-V; coprocessor0 will have to be replaced.)

The emulator includes support for a few Linux kernel systemcalls. The MIPS O32 ABI is used.

| Register                           | use on input          | use on output                     | Note
|:-----------------------------------|:----------------------|:----------------------------------|:-------
| $at ($1)                           | —                     | (caller saved)                    |
| $v0 ($2)                           | syscall number        | return value                      |
| $v1 ($3)                           | —                     | 2nd fd only for pipe(2)           |
| $a0 ... $a2 ($4 ... $6)            | syscall arguments     | returned unmodified               |
| $a3 ($7)                           | 4th syscall argument  | $a3 set to 0/1 for success/error  |
| $t0 ... $t9 ($8 ... $15, $24, $25) | —                     | (caller saved)                    |
| $s0 ... $s7 ($16 ... $23)          | —                     | (callee saved)                    |
| $k0, $k1 ($26, $27)                |                       |                                   |
| $gp ($28)                          |                       | (callee saved)                    |
| $sp ($29)                          |                       | (callee saved)                    |
| $fp or $s8 ($30)                   |                       | (callee saved)                    |
| $ra ($31)                          |                       | (callee saved)                    |
| $hi, $lo                           | —                     | (caller saved)                    |

The first four input arguments are passed in registers $a0 to $a3, if more arguments are required then fifth and
following arguments are stored on the stack.

Supported syscalls:

#### void [exit](http://man7.org/linux/man-pages/man2/exit.2.html)(int status) __NR_exit (4001)

Stop/end execution of the program. The argument is exit status code, zero means OK, other values informs about error.

#### ssize_t [read](http://man7.org/linux/man-pages/man2/read.2.html)(int fd, void *buf, size_t count) __NR_read (4003)

Read `count` bytes from open file descriptor `fd`. The emulator maps file descriptors 0, 1 and 2 to the internal
terminal/console emulator. They can be used without `open` call. If there are no more characters to read from the
console, newline is appended. At most the count bytes read are stored to the memory location specified by `buf`
argument. Actual number of read bytes is returned.

#### ssize_t [write](http://man7.org/linux/man-pages/man2/write.2.html)(int fd, const void *buf, size_t count) __NR_write (4004)

Write `count` bytes from memory location `buf` to the open file descriptor
`fd`. The same about console for file handles 0, 1 and 2 is valid as for `read`.

#### int [close](http://man7.org/linux/man-pages/man2/close.2.html)(int fd) __NR_close (4006)

Close file associated to descriptor `fd` and release descriptor.

#### int [open](http://man7.org/linux/man-pages/man2/open.2.html)(const char *pathname, int flags, mode_t mode) __NR_open (4005)

Open file and associate it with the first unused file descriptor number and return that number. If the
option `OS Emulation`->`Filesystem root`
is not empty then the file path `pathname` received from emulated environment is appended to the path specified
by `Filesystem root`. The host filesystem is protected against attempt to traverse to random directory by use of `..`
path elements. If the root is not specified then all open files are targetted to the emulated terminal.

#### void * [brk](http://man7.org/linux/man-pages/man2/brk.2.html)(void *addr) __NR_brk (4045)

Set end of the area used by standard heap after end of the program data/bss. The syscall is emulated by dummy
implementation. Whole address space up to 0xffff0000 is backuped by automatically attached RAM.

#### int [ftruncate](http://man7.org/linux/man-pages/man2/ftruncate.2.html)(int fd, off_t length) __NR_truncate (4092)

Set length of the open file specified by `fd` to the new `length`. The `length`
argument is 64-bit even on 32-bit system and for big-endian MIPS it is apssed as higher part and the lower part in the
second and third argument.

#### ssize_t [readv](http://man7.org/linux/man-pages/man2/readv.2.html)(int fd, const struct iovec *iov, int iovcnt) __NR_Linux (4145)

The variant of `read` system call where data to read are would be stored to locations specified by `iovcnt` pairs of
base address, length pairs stored in memory at address pass in `iov`.

#### ssize_t [writev](http://man7.org/linux/man-pages/man2/writev.2.html)(int fd, const struct iovec *iov, int iovcnt) __NR_Linux (4146)

The variant of `write` system call where data to write are defined by `iovcnt`
pairs of base address, length pairs stored in memory at address pass in `iov`.

#### int [set_thread_area](http://man7.org/linux/man-pages/man2/set_thread_area.2.html)(unsigned long addr) __NR_set_thread_area (4283)

Set TLS base into `C0` `user_local` register accessible by `rdhwr` instruction..

## Special instructions support

(TODO Max)

## Limitations of the Implementation

This is initial implementation of the RISC-V edition. Everything is limited.

- Coprocessor0 has to be ported to RISC-V status registers.

### QtMips original limitations

* Only very minimal support for privileged instruction is implemented for now. Only RDHWR, SYNCI, CACHE and some
  coprocessor 0 registers implemented. TLB and virtual memory and complete exception model are not implemented.
* Coprocessors (so no floating point unit and only limited coprocessor 0)
* Memory access stall (stalling execution because of cache miss would be pretty annoying for users so difference between
  cache and memory is just in collected statistics)
* Only limited support for interrupts and exceptions. When `syscall` or `break`
  instruction is recognized, emulation stops. Single step proceed after instruction.

## List of Actually Supported Instructions

- **RV32G**:
  - **LOAD**: `lw, lh, lb`
  - **STORE**: `sw, sh, sb`
  - **OP**: `add, sub, sll, slt, sltu, xor, srl, sra, or, and`
  - **OP-IMM**: `addi, sll, slti, sltiu, xori, srli, srai, ori, andi`
  - **BRANCH**: `beq, bne, btl, bge, bltu, bgtu`
  - **JUMP**: `jal, jalr`
- **Pseudoinstructions**
  - **BASIC**: `nop`
  - **LOAD**: `la, li`,
  - **OP**: `mv, not, neg, negw, sext.b, sext.h, sext.w, zext.b, zext.h, zext.w, seqz, snez, sltz, slgz`
  - **BRANCH**: `beqz, bnez, blez, bgez, bltz, bgtz, bgt, ble, bgtu, bleu`
  - **JUMP**: `j, jal`

For details about RISC-V, refer to the ISA specification:
[https://riscv.org/technical/specifications/](https://riscv.org/technical/specifications/).

## Links to Resources and Similar Projects

- QtRVSim - MIPS predecessor of this simulator [https://github.com/cvut/QtRVSim](https://github.com/cvut/QtRVSim)
- Jakub Dupak: [Graphical RISC-V Architecture Simulator - Memory Model and Project Management](https://dspace.cvut.
  cz/bitstream/handle/10467/94446/F3-BP-2021-Dupak-Jakub-thesis.pdf) documents QtMips and QtRvSim development

## Copyright

- Copyright (c) 2017-2019 Karel Koci <cynerd@email.cz>
- Copyright (c) 2019-2021 Pavel Pisa <pisa@cmp.felk.cvut.cz>
- Copyright (c) 2020-2021 Jakub Dupak <dev@jakubdupak.com>
- Copyright (c) 2020-2021 Max Hollmann <hollmmax@fel.cvut.cz>

## License

This project is licensed under `GPL-3.0-or-later`. The full text of the license is in the [LICENSE](LICENSE) file. 
The license applies to all files except for directories named `external` and files in them. Files in external 
directories have a separate license compatible with the projects license.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public 
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later 
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. 
If not, see [https://www.gnu.org/licenses/](https://www.gnu.org/licenses/).
