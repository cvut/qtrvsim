QtMips
======

[![OpenHub](https://www.openhub.net/p/QtMips/widgets/project_thin_badge?format=gif)](https://www.openhub.net/p/QtMips) [![build status](https://dev.azure.com/qtmips/qtmips/_apis/build/status/cvut.QtMips?branchName=master)](https://dev.azure.com/qtmips/QtMips/_build/latest?definitionId=1&branchName=master)

MIPS CPU simulator for education purposes.

Documentation
-------------
The project has started as diploma theses work of Karel Kočí.
The complete text of the thesis [Graphical CPU Simulator with
Cache Visualization](https://dspace.cvut.cz/bitstream/handle/10467/76764/F3-DP-2018-Koci-Karel-diploma.pdf)
is available from the online archive of
the [Czech Technical University in Prague](https://www.cvut.cz/).
The document provides analysis of available alternative simulators,
overview of the project architecture and basic usage
information.

The used [MIPS CPU](https://en.wikipedia.org/wiki/MIPS_architecture) building block diagram,
and a pipeline model matches lecture slides prepared by Michal Štepanovský for the subject
[Computer Architectures](https://cw.fel.cvut.cz/wiki/courses/b35apo/start).
The course is based on the book  [Computer Organization and Design, The HW/SW Interface](https://www.elsevier.com/books/computer-organization-and-design-mips-edition/patterson/978-0-12-407726-3) written by
professors Paterson and Henessy.

Additional documentation can be found in subdirectory [`docs`](docs)
of the project.

Build Dependencies
------------------
* Qt 5
* elfutils (libelf works too but there can be some problems)

General Compilation
-------------------
To compile whole project just run these commands:
```
qmake /path/to/qtmips
make
```
Where `/path/to/qtmips` is path to this project root.

(Be sure to use qt5 qmake.)

Compilation for Local Execution
-------------------------------
Because simulator it self and operating system stub are implemented as libraries you
need to have that libraries in path where loader can found them. Binary looks for library
at system library paths (on Windows in actual directory as well) and on compiled in
RPATH which is `../lib` (i.e., install into `bin` and `lib` directory is assumed):

```
qmake /path/to/qtmips "QMAKE_RPATHDIR += ../qtmips_machine ../qtmips_osemu"
make
```

Or compile the application with static libraries

```
mkdir QtMips-build
cd QtMips-build
qmake "CONFIG+=static" "CONFIG+=staticlib" -recursive ../QtMips/qtmips.pro
```

Alternatively, you can setup

```
LD_LIBRARY_PATH=/path_to_QtMips/qtmips_machine /path_to_QtMips/qtmips_osemu
```

Building from source on macOS
-------------------------------

Install the latest version of **Xcode** from the App Store.
Then open a terminal and execute xcode-select --install to install Command Line Tools.
Then open Xcode, accept the license agreement and wait for it to install
any additional components. After you finally see the "Welcome to Xcode" screen,
from the top bar choose Xcode -> Preferences -> Locations -> Command Line Tools
and select an SDK version.

Install [Homebrew](https://brew.sh/) and the following dependencies:
```bash
brew install qt libelf
```

Add Qt to the PATH. If you use Bash, you can do it like this:
```bash
echo 'export PATH="/usr/local/opt/qt/bin:$PATH"' >> ~/.bash_profile
```

Verify Qt is available in the PATH by running:
```bash
qmake --version
```

Finally, you can build the QtMips:
```bash
./build-macos.sh
```

After successful completion, you should see **build/qtmips-macos.zip**
which contains two files: qtmips_qui.app and qtmips_cli.

You can unzip it and then copy qtmips_qui.app wherever you want. Double-click to start it.


Download Binary Packages
------------------------

* [https://github.com/cvut/QtMips/releases](https://github.com/cvut/QtMips/releases) - archives with Windows and generic GNU/Linux binaries
* [https://launchpad.net/~ppisa/+archive/ubuntu/qtmips](https://launchpad.net/~ppisa/+archive/ubuntu/qtmips) - Ubuntu packages for Disco, Cosmic, Bionic and Xenial releases.
* [https://build.opensuse.org/repositories/home:ppisa/qtmips](https://build.opensuse.org/repositories/home:ppisa/qtmips) - Open Build Service build for Fedora_29, Fedora_Rawhide, Raspbian_9.0, SLE_15, openSUSE_Leap_15.0_Ports, openSUSE_Leap_15.0, openSUSE_Leap_15.1, openSUSE_Leap_42.3, openSUSE_Leap_42.3_Ports, openSUSE_Tumbleweed and Debian
* [https://software.opensuse.org//download.html?project=home%3Appisa&package=qtmips](https://software.opensuse.org//download.html?project=home%3Appisa&package=qtmips) - Open Build Service binary packages

Accepted Binary Formats
------------------------
The simulator accepts ELF statically linked executables
compiled for 32-bit big-endian MISP target.

Optimal is use of plain mips-elf GCC toolchain.

For more refer to the [supported executable formats](docs/exec-formats-and-tools.md)
documentation in the [`docs`](docs) projects subdirectory.

Integrated Assembler
-------------------
Basic integrated assembler is included in the simulator. It recognizes basic
MIPS instructions and `la` and `li` pseudo instructions. Small subset of
[GNU assembler](https://sourceware.org/binutils/docs/as/) directives is recognized as well.
Next directives are recognized: `.word`, `.orig`, `.set`/`.equ`, `.ascii` and `.asciz`.
Some other directives are simply ignored: `.data`, `.text`, `.globl`, `.end` and `.ent`.
This allows to write code which can be compiled by both - integrated and full-featured
assembler. Addresses are assigned to labels/symbols which are stored in symbol table.
Addition, substraction, multiplication, divide and bitwise and and or are recognized.

Support to call external make utility
-------------------------------------
The action "Build executable by external make" call "make" program. If the
action is invoked and some of source editors selected in main windows
tabs then the "make" is started in the corresponding directory. Else
directory of last selected editor is chosen. If no editor is open then
directory of last loaded ELF executable are used as "make" start path.
If even that is not an option then default directory when the emulator
has been started is used.

Tests
-----
There are two types of tests in QtMips. One type are unit tests for simulator it
self and second one are integration tests with command line client and real
compiled elf binaries. All these tests can be executed using script
`tests/run-all.sh` or one by one by running respective `test.sh` scripts.

Source files for unit tests can be found in path `qtmips_machine/tests` and
integration tests are located in `tests` directory.

Peripherals
-----------

The simulator implements emulation of two peripherals for now.
Base addresses are selected such way that they are accessible
by 16 immediate offset which uses register 0 (`zero`) as base.

The first is simple serial port (UART). It support transmission
(Tx) and receiption (Rx). Receiver status register (`SERP_RX_ST_REG`)
implements two bits. Read-only bit 0 (`SERP_RX_ST_REG_READY`)
is set to one if there is unread character available in
the receiver data register (`SERP_RX_DATA_REG`). The bit 1
(`SERP_RX_ST_REG_IE`) can be written to 1 to enable interrupt
request when unread character is available.
The transmitter status register (`SERP_TX_ST_REG`) bit 0
(SERP_TX_ST_REG_READY) signals by value 1 that UART is ready
and can accept next character to be sent. The bit 1
(`SERP_TX_ST_REG_IE`) enables generation of interrupt.
The register `SERP_TX_DATA_REG` is actual Tx buffer.
The LSB byte of written word is transmitted to the terminal
window. Definition of peripheral base address and registers
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
The UART registers region is mirrored on the address 0xffff0000
to enable use of programs initially writtent for [SPIM](http://spimsimulator.sourceforge.net/)
or [MARS](http://courses.missouristate.edu/KenVollmar/MARS/) emulators.

The another peripheral allows to set three byte values
concatenated to single word (read-only KNOBS_8BIT register)
from user panel set by knobs and display one word in hexadecimal,
decimal and binary format (`LED_LINE` register). There are two
other words writable which control color of RGB LED 1 and 2
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

The simple 16-bit per pixel (RGB565) framebuffer and LCD display
are implemented. The framebuffer is mapped into range starting
at `LCD_FB_START` address. The display size is 480 x 320 pixel.
Pixel format RGB565 expect red component in bits 11 .. 15, green
component in bits 5 .. 10 and blue component in bits 0 .. 4.
```
#define LCD_FB_START       0xffe00000
#define LCD_FB_END         0xffe4afff
```
Limitation: actual concept of memory view updates and access
doesnot allows to reliably read peripheral registers and I/O
memory content. It is possible to write into framebuffer memory
when cached (from CPU perspective) access to memory is selected.

Interrupts and Coprocessor 0 Support
------------------------------------

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

Hardware/special registers implemented:

| Number | Name       | Description |
|-------:|:-----------|:------------|
|  0     | CPUNum     | CPU number, fixed to 0  |
|  1     | SYNCI_Step | Increment required for instruction cache synchronization |
|  2     | CC         | Cycle counter |
|  3     | CCRes      | Cycle counter resolution, fixed on 1 |
| 29     | UserLocal  | Read only value of Coprocessor 0 $4,2 register |

Sequence to enable serial port receive interrupt:

Decide location of interrupt service routine the first. The default address
is 0x80000180. The base can be changed (`EBase` register) and then PC is set
to address EBase + 0x180. This is in accordance with MIPS release 1 and 2
manuals.

Enable bit 10 (interrupt mask) in the Status register. Ensure that bit
1 (`EXL`) is zero and bit 0 (`IE`) is set to one.

Enable interrupt in the receiver status register (bit 1 of `SERP_RX_ST_REG`).

Write character to the terminal. It should be immediately consumed by
the serial port receiver if interrupt is enabled in `SERP_RX_ST_REG`.
CPU should report interrupt exception and when it propagates to
the execution phase `PC` is set to the interrupt routine start address.

Some hints how to direct linker to place interrupt handler routine
at appropriate address. Implement interrupt routine in new section

```
.section .irq_handler, "ax"
```

Use next linker option to place section start at right address

```
 -Wl,--section-start=.irq_handler=0x80000180
```

System Calls Support
--------------------

The emulator includes support for a few Linux kernel systemcalls.
The MIPS O32 ABI is used.

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

The first four input arguments are passed in registers $a0 to $a3, if more arguments are required
then fifth and following arguments are stored on the stack.

Supported syscalls:

#### void [exit](http://man7.org/linux/man-pages/man2/exit.2.html)(int status) __NR_exit (4001)
Stop/end execution of the program. The argument is exit status code,
zero means OK, other values informs about error.

#### ssize_t [read](http://man7.org/linux/man-pages/man2/read.2.html)(int fd, void *buf, size_t count) __NR_read (4003)
Read `count` bytes from open file descriptor `fd`. The emulator maps
file descriptors 0, 1 and 2 to the internal terminal/console emulator.
They can be used without `open` call. If there are no more characters to read
from the console, newline is appended. At most the count bytes read
are stored to the memory location specified by `buf` argument.
Actual number of read bytes is returned.

#### ssize_t [write](http://man7.org/linux/man-pages/man2/write.2.html)(int fd, const void *buf, size_t count) __NR_write (4004)
Write `count` bytes from memory location `buf` to the open file descriptor
`fd`. The same about console for file handles 0, 1 and 2 is valid as for `read`.

#### int [close](http://man7.org/linux/man-pages/man2/close.2.html)(int fd) __NR_close (4006)
Close file associated to descriptor `fd` and release descriptor.

#### int [open](http://man7.org/linux/man-pages/man2/open.2.html)(const char *pathname, int flags, mode_t mode) __NR_open (4005)
Open file and associate it with the first unused file descriptor number
and return that number. If the option `OS Emulation`->`Filesystem root`
is not empty then the file path `pathname` received from emulated
environment is appended to the path specified by `Filesystem root`.
The host filesystem is protected against attempt to traverse to
random directory by use of `..` path elements. If the root is not specified
then all open files are targetted to the emulated terminal.

#### void * [brk](http://man7.org/linux/man-pages/man2/brk.2.html)(void *addr) __NR_brk (4045)
Set end of the area used by standard heap after end of the program data/bss.
The syscall is emulated by dummy implementation. Whole address space
up to 0xffff0000 is backuped by automatically attached RAM.

#### int [ftruncate](http://man7.org/linux/man-pages/man2/ftruncate.2.html)(int fd, off_t length) __NR_truncate (4092)
Set length of the open file specified by `fd` to the new `length`.
The `length` argument is 64-bit even on 32-bit system and for
big-endian MIPS it is apssed as higher part and the lower part
in the second and third argument.

#### ssize_t [readv](http://man7.org/linux/man-pages/man2/readv.2.html)(int fd, const struct iovec *iov, int iovcnt) __NR_Linux (4145)
The variant of `read` system call where data to read are would be stored
to locations specified by `iovcnt` pairs of base address, length pairs stored
in memory at address pass in `iov`.

#### ssize_t [writev](http://man7.org/linux/man-pages/man2/writev.2.html)(int fd, const struct iovec *iov, int iovcnt) __NR_Linux (4146)
The variant of `write` system call where data to write are defined
by `iovcnt` pairs of base address, length pairs stored in memory
at address pass in `iov`.

#### int [set_thread_area](http://man7.org/linux/man-pages/man2/set_thread_area.2.html)(unsigned long addr) __NR_set_thread_area (4283)
Set TLS base into `C0` `user_local` register accessible by `rdhwr` instruction..

Special instructions support
---------------------------------

#### RDHWR - read hardware registers
Supported registers described in Interrupts and Coprocessor 0 Support section

#### SYNC - memory barrier between preceding and following reads/writes
It is implemented as NOP because memory access is processed in order and only in the memory stage.

#### SYNCI - synchronize/propagate modification to the instruction cache memory and pipeline
The function codes for different modes nor address/cache line which should be synchronized
is recognized. Instruction is implemented as full instruction and data cache flush.

#### CACHE - cache maintenance operations
Function is not decoded, full flush of data and instruction caches is performed.

Limitations of the Implementation
---------------------------------
* Only very minimal support for privileged instruction is implemented for now.
  Only RDHWR, SYNCI, CACHE and some coprocessor 0 registers implemented. TLB and virtual
  memory and complete exception model are not implemented.
* Coprocessors (so no floating point unit and only limited coprocessor 0)
* Memory access stall (stalling execution because of cache miss would be pretty
  annoying for users so difference between cache and memory is just in collected
  statistics)
* Only limited support for interrupts and exceptions. When `syscall` or `break`
  instruction is recognized, emulation stops. Single step proceed after instruction.

List of Actually Supported Instructions
---------------------------------------
ADD ADDI ADDIU ADDU AND ANDI BEQ BEQL BGEZ BGEZAL BGEZALL BGEZL BGTZ BGTZL BLEZ BLEZL BLTZ BLTZAL BLTZALL BLTZL BNE BNEL BREAK CACHE CLO CLZ DIV DIVU ERET EXT INS J JAL JALR JR LB LBU LH LHU LL LUI LW LWC1 LWD1 LWL LWR MADD MADDU MFC0 MFHI MFLO MFMC0 MOVN MOVZ MSUB MSUBU MTC0 MTHI MTLO MUL MULT MULTU NOR OR ORI PREF RDHWR ROTR ROTRV SB SC SDC1 SEB SEH SH SLL SLLV SLT SLTI SLTIU SLTU SRA SRAV SRL SRLV SUB SUBU SW SWC1 SWL SWR SYNC SYNCI SYSCALL TEQ TEQI TGE TGEI TGEIU TGEU TLT TLTI TLTIU TLTU TNE TNEI WSBH XOR XORI 

Links to Resources and Similar Projects
---------------------------------------
* SPIM/QtSPIM: A MIPS32 Simulator [http://spimsimulator.sourceforge.net/](http://spimsimulator.sourceforge.net/)
* MARS: IDE with detailed help and hints [http://courses.missouristate.edu/KenVollmar/MARS/index.htm](http://courses.missouristate.edu/KenVollmar/MARS/index.htm)
* EduMIPS64: 1x fixed and 3x FP pipelines [https://www.edumips.org/](https://www.edumips.org/)
