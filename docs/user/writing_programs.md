# Writing Programs

This chapter covers how to write and run RISC-V assembly programs in QtRvSim.

## Using the Built-in Assembler

QtRvSim includes a built-in assembler that can compile RISC-V assembly code directly within the application:

1. Write your assembly code in the editor
2. Click **Compile Source** (<kbd>Ctrl+E</kbd>) to assemble the code
3. The program is loaded into simulated memory and ready to execute

This is the recommended workflow for learning and experimenting with RISC-V assembly.

## RISC-V Register Reference

RISC-V has 32 general-purpose registers. The simulator supports both numeric names (`x0`–`x31`) and ABI mnemonic names:

| Register | ABI Name | Description |
|----------|----------|-------------|
| x0 | zero | Hard-wired zero |
| x1 | ra | Return address |
| x2 | sp | Stack pointer |
| x3 | gp | Global pointer |
| x4 | tp | Thread pointer |
| x5–x7 | t0–t2 | Temporary registers |
| x8 | s0/fp | Saved register / Frame pointer |
| x9 | s1 | Saved register |
| x10–x11 | a0–a1 | Function arguments / Return values |
| x12–x17 | a2–a7 | Function arguments |
| x18–x27 | s2–s11 | Saved registers |
| x28–x31 | t3–t6 | Temporary registers |

Toggle between numeric and mnemonic names via **Machine → Mnemonic Registers**.

## Simple Assembly Program Template

Here is a minimal RISC-V assembly template for use with the built-in assembler:

```asm
.globl _start
.text

_start:
    # Your code here
    
    # Example: Load immediate values
    li a0, 42          # Load 42 into a0
    li a1, 10          # Load 10 into a1
    add a2, a0, a1     # a2 = a0 + a1

end:
    ebreak             # Stop execution
    j end              # Loop forever if continued
```

The `ebreak` instruction triggers a breakpoint exception, which causes the simulator to pause execution.

## Breakpoints and Debugging

### Software Breakpoints

The `ebreak` instruction causes the simulator to pause execution. This is useful for stopping at specific points in your program:

```asm
    # ... some code ...
    ebreak           # Pause here
    # ... more code ...
```

When `ebreak` is encountered, the simulator stops. You can then inspect registers and memory, and continue execution with **Step** or **Run**.

### Hardware Breakpoints

You can also set breakpoints directly in the QtRvSim interface:
1. In the **Program** panel, click on the address or instruction where you want to break
2. The simulator will pause when that instruction is fetched

## Loading Programs

There are several ways to load programs into QtRvSim:

1. **Built-in editor**: Write assembly in the editor and compile with <kbd>Ctrl+E</kbd>
2. **Open source file**: Use **File → Open Source** to load a `.s` file, then compile
3. **Load ELF**: In the launch dialog, use **Browse** to select a precompiled `.elf` file
4. **Build Executable**: Use **Machine → Build Executable** (<kbd>Ctrl+B</kbd>) to compile using an external toolchain

## Next Steps

Once you're comfortable writing basic assembly programs, you may want to explore:
- Using an [external RISC-V toolchain](external_toolchains.md) for compiling C programs or more complex projects