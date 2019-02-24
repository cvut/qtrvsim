QtMips
======
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

Additional documentation can be found in subdirectory ['docs'](docs)
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
at system libary paths (on Windows in actual directory as well) and on compiled in
RPATH which is `../lib` (i.e., install into 'bin' and 'lib' directory is assumed):

```
qmake /path/to/qtmips "QMAKE_RPATHDIR += ../qtmips_machine ../qtmips_osemu"
make
```

Or compile the application with static libraries

```
mkdir QtMips-build
cd QtMips-build
qmake CONFIG+=static" "CONFIG+=staticlib" -recursive ../QtMips/qtmips.pro
```

Alternativelly, you can setup

```
LD_LIBRARY_PATH=/path_to_QtMips/qtmips_machine /path_to_QtMips/qtmips_osemu
```

Accepted binary formats
------------------------
The simulator accepts ELF statically linked executables
compiled for 32-bit big-endian MISP target.

Optimal is use of plain mips-elf GCC toolchain.

For more reffer to the [supported executable formats](docs/exec-formats-and-tools.md)
documentation in the ['docs'](docs) projects subdirectory.

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

The firs is simple serial port (UART) which transmission
(Tx) support only for now. Actual version implements only
two registers, the 'SERP_TX_ST_REG' is trasmit status register.
Bit 0 signals by value 1 that UART is ready and can
accept next character to be sent.
The second register 'SERP_TX_DATA_REG' is actual Tx buffer.
The LSB byte of writtent word is transmitted to terminal
window. Definition of peripheral base address and registers
offsets follows including reserve for the future Rx port
implementation.
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

The another peripheral allows to set three byte values
concatenated to single word (read-only KNOBS_8BIT register)
from user panel set by knobs and display one word in hexadecimal,
decimal and binary format ('LED_LINE' register). There are two
other words writtable which control color of RGB LED 1 and 2
(registers 'LED_RGB1' and 'LED_RGB2').

```
#define SPILED_REG_BASE    0xffffc100

#define SPILED_REG_LED_LINE_o           0x004
#define SPILED_REG_LED_RGB1_o           0x010
#define SPILED_REG_LED_RGB2_o           0x014
#define SPILED_REG_LED_KBDWR_DIRECT_o   0x018

#define SPILED_REG_KBDRD_KNOBS_DIRECT_o 0x020
#define SPILED_REG_KNOBS_8BIT_o         0x024
```

Limitations of the Implementation
---------------------------------
* Only 'rdhwr' privileged instruction is implemented for now. All other privileged
  instructions and features dependent on them are not implemented.
* Coprocessors (so no floating point unit nor any other type)
* Memory access stall (stalling execution because of cache miss would be pretty
  annoying for users so difference between cache and memory is just in collected
  statistics)
* Only limited support for interrupts and exceptions. When 'syscall' or 'break'
  instruction is recognized, emulation stops. Single step proceed after instruction.
* Complete binary instruction check (we check only minimal set of bites to decode
  instruction, we don't check if zero sections are really zero unless we need it),
  but instruction decoder can be easily extended to distinguish instructions
  according additional subfiled.
  
List of Actually Supported Instructions
---------------------------------------
ADD ADDI ADDIU ADDU AND ANDI BEQ BEQL BGEZ BGEZAL BGEZALL BGEZL BGTZ BGTZL BLEZ BLEZL BLTZ BLTZAL BLTZALL BLTZL BNE BNEL BREAK CACHE CLO CLZ DIV DIVU EXT INS J JAL JALR JR LB LBU LH LHU LL LUI LW LWC1 LWD1 LWL LWR MADD MADDU MFHI MFLO MOVN MOVZ MSUB MSUBU MTHI MTLO MUL MULT MULTU NOR OR ORI PREF RDHWR ROTR ROTRV SB SC SDC1 SEB SEH SH SLL SLLV SLT SLTI SLTIU SLTU SRA SRAV SRL SRLV SUB SUBU SW SWC1 SWL SWR SYNC SYNCI SYSCALL TEQ TEQI TGE TGEI TGEIU TGEU TLT TLTI TLTIU TLTU TNE TNEI WSBH XOR XORI.
