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
at system library paths (on Windows in actual directory as well) and on compiled in
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

Alternatively, you can setup

```
LD_LIBRARY_PATH=/path_to_QtMips/qtmips_machine /path_to_QtMips/qtmips_osemu
```

Accepted binary formats
------------------------
The simulator accepts ELF statically linked executables
compiled for 32-bit big-endian MISP target.

Optimal is use of plain mips-elf GCC toolchain.

For more refer to the [supported executable formats](docs/exec-formats-and-tools.md)
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
The LSB byte of written word is transmitted to terminal
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
other words writable which control color of RGB LED 1 and 2
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
is 0x80000180. The base can be changed (EBase register) and then PC is set
to address EBase + 0x180. This is in accordance with MIPS release 1 and 2
manuals.

Enable bit 10 (interrupt mask) in the Status register. Ensure that bit
1 (EXL) is zero and bit 0 (IE) is set to one.

Enable interrupt in the receiver status register (bit 1 of SERP_RX_ST_REG).

Write character to the terminal It should be immediately consumed by
the serial port receiver if interrupt is enabled in SERP_RX_ST_REG.
CPU should report interrupt exception and when it propagates to
the execution phase PC is set to the interrupt routine start address.

Some hints how to direct linker to place interrupt handler routine
at appropriate address. Implement interrupt routine in new section

```
.section .irq_handler, "ax"
```

Use next linker option to place section start at right address

```
 -Wl,--section-start=.irq_handler=0x80000180
```


Limitations of the Implementation
---------------------------------
* Only very minimal support for privileged instruction is implemented for now.
  Only RDHWR and some coprocessor 0 registers implemented. TLB and virtual
  memory and complete exception model not implemented.
* Coprocessors (so no floating point unit and only limited coprocessor 0)
* Memory access stall (stalling execution because of cache miss would be pretty
  annoying for users so difference between cache and memory is just in collected
  statistics)
* Only limited support for interrupts and exceptions. When 'syscall' or 'break'
  instruction is recognized, emulation stops. Single step proceed after instruction.
* Complete binary instruction check (we check only minimal set of bites to decode
  instruction, we don't check if zero sections are really zero unless we need it),
  but instruction decoder can be easily extended to distinguish instructions
  according additional subfield.
  
List of Actually Supported Instructions
---------------------------------------
ADD ADDI ADDIU ADDU AND ANDI BEQ BEQL BGEZ BGEZAL BGEZALL BGEZL BGTZ BGTZL BLEZ BLEZL BLTZ BLTZAL BLTZALL BLTZL BNE BNEL BREAK CACHE CLO CLZ DIV DIVU ERET EXT INS J JAL JALR JR LB LBU LH LHU LL LUI LW LWC1 LWD1 LWL LWR MADD MADDU MFC0 MFHI MFLO MFMC0 MOVN MOVZ MSUB MSUBU MTC0 MTHI MTLO MUL MULT MULTU NOR OR ORI PREF RDHWR ROTR ROTRV SB SC SDC1 SEB SEH SH SLL SLLV SLT SLTI SLTIU SLTU SRA SRAV SRL SRLV SUB SUBU SW SWC1 SWL SWR SYNC SYNCI SYSCALL TEQ TEQI TGE TGEI TGEIU TGEU TLT TLTI TLTIU TLTU TNE TNEI WSBH XOR XORI 
