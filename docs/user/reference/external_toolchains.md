# External Toolchains

For more complex programs or C code, you can use an external RISC-V cross-compiler toolchain instead of the built-in assembler.

## Installing the Toolchain

**Ubuntu/Debian:**
```bash
sudo apt install gcc-riscv64-unknown-elf
```

**Fedora:**
```bash
sudo dnf install riscv64-elf-gcc
```

**Arch Linux:**
```bash
sudo pacman -S riscv64-elf-gcc
```

**macOS (Homebrew):**
```bash
brew install riscv64-elf-gcc
```

## Compiling Assembly Programs

To compile a simple assembly program:

```bash
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -nostartfiles -o program.elf program.s
```

Or for RV64:
```bash
riscv64-unknown-elf-gcc -march=rv64i -mabi=lp64 -nostdlib -nostartfiles -o program.elf program.s
```

## Compiling C Programs

For C programs, you need a minimal startup file and appropriate compiler flags.

**Startup code (`crt0.s`):**
```asm
.globl _start
.text

_start:
    la sp, _stack_top    # Initialize stack pointer
    call main            # Call main function

_exit:
    ebreak               # Stop execution
    j _exit

.section .bss
.align 4
_stack_bottom:
    .space 4096          # 4KB stack
_stack_top:
```

**Example C program (`hello.c`):**
```c
#define SERIAL_PORT_BASE   0xffffc000
#define SERP_TX_ST_REG     (*(volatile unsigned *)(SERIAL_PORT_BASE + 0x00))
#define SERP_TX_DATA_REG   (*(volatile unsigned *)(SERIAL_PORT_BASE + 0x04))

void print_char(char c) {
    while (SERP_TX_ST_REG & 0x1);  // Wait while busy
    SERP_TX_DATA_REG = c;
}

void print_string(const char *s) {
    while (*s) {
        print_char(*s++);
    }
}

int main(void) {
    print_string("Hello from QtRvSim!\n");
    return 0;
}
```

**Compilation:**
```bash
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -c crt0.s -o crt0.o
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -O2 -c hello.c -o hello.o
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -static -T linker.ld crt0.o hello.o -o hello.elf
```

## Linker Script

A simple linker script (`linker.ld`) for QtRvSim:

```ld
ENTRY(_start)

MEMORY {
    RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 64K
}

SECTIONS {
    .text : {
        *(.text)
        *(.text.*)
    } > RAM

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    } > RAM

    .data : {
        *(.data)
        *(.data.*)
    } > RAM

    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } > RAM
}
```

## Using Make for Build Automation

For larger projects, a `Makefile` automates the build process:

```makefile
CROSS = riscv64-unknown-elf-
CC = $(CROSS)gcc
AS = $(CROSS)as
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump

ARCH_FLAGS = -march=rv32i -mabi=ilp32
CFLAGS = $(ARCH_FLAGS) -O2 -Wall -nostdlib -ffreestanding
LDFLAGS = $(ARCH_FLAGS) -nostdlib -static -T linker.ld

SOURCES = crt0.s main.c
TARGET = program.elf

OBJECTS = $(SOURCES:.s=.o)
OBJECTS := $(OBJECTS:.c=.o)

all: $(TARGET)

%.o: %.s
	$(CC) $(ARCH_FLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

disasm: $(TARGET)
	$(OBJDUMP) -d $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean disasm
```

## Loading ELF Files

To load a compiled ELF file into QtRvSim:

1. In the launch dialog, use **Browse** to select the `.elf` file, then click **Load machine**
2. Or use **Machine → Build Executable** (<kbd>Ctrl+B</kbd>) to compile and load using an external toolchain directly from the editor