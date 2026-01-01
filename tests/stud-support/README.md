# stud-support Integration Tests

This directory contains integration tests that use the [stud-support](https://gitlab.fel.cvut.cz/b35apo/stud-support) repository examples to verify QtRvSim functionality.

## Overview

The tests compile RISC-V programs from the stud-support teaching materials and run them through `qtrvsim_cli` to verify:

1. **Functional correctness**: Programs execute correctly and produce expected results
2. **Debug info support**: Source-level tracing works correctly for C and assembly files

## Prerequisites

- **RISC-V toolchain**: Either `riscv64-unknown-elf-gcc` or `clang` with RISC-V target support
- **Python 3.6+**: For running the test scripts
- **stud-support repository**: Clone from https://gitlab.fel.cvut.cz/b35apo/stud-support

## Quick Start

```bash
# 1. Clone stud-support (if not already done)
git clone https://gitlab.fel.cvut.cz/b35apo/stud-support.git /path/to/stud-support

# 2. Build test ELFs
python3 build_tests.py --stud-support-path /path/to/stud-support

# 3. Run functional tests
python3 run_tests.py \
    --qtrvsim-cli ../../build/target/qtrvsim_cli \
    --build-dir ./elfs \
    --stud-support-path /path/to/stud-support

# 4. Run debug info tests
python3 test_debuginfo.py \
    --qtrvsim-cli ../../build/target/qtrvsim_cli \
    --build-dir ./elfs
```

## Test Scripts

### `build_tests.py`

Compiles selected stud-support examples into RISC-V ELF files.

```bash
python3 build_tests.py --stud-support-path /path/to/stud-support [OPTIONS]

Options:
  --output-dir DIR      Output directory for ELFs (default: ./elfs)
  --use-gcc             Force GNU toolchain
  --use-clang           Force Clang/LLVM toolchain
  -v, --verbose         Show detailed build output
```

The script automatically detects available toolchains, preferring GCC when available.

### `run_tests.py`

Runs functional integration tests on the compiled ELFs.

```bash
python3 run_tests.py \
    --qtrvsim-cli /path/to/qtrvsim_cli \
    --build-dir ./elfs \
    --stud-support-path /path/to/stud-support \
    [OPTIONS]

Options:
  -v, --verbose         Show detailed output on failures
  -f, --filter PATTERN  Run only tests matching PATTERN
  --pipeline            Test pipelined execution mode
  --cache               Test with cache enabled
```

### `test_debuginfo.py`

Tests debug info (source-level tracing) functionality using **property-based testing**.

Instead of checking exact output (which is brittle due to compiler variations), this script verifies semantic properties:

- **File presence**: Expected source files appear in traces
- **Line validity**: All line numbers are positive integers
- **Coverage**: Traced instructions have debug info annotations
- **Format validity**: Trace output is correctly formatted

```bash
python3 test_debuginfo.py \
    --qtrvsim-cli /path/to/qtrvsim_cli \
    --build-dir ./elfs \
    [OPTIONS]

Options:
  -v, --verbose         Show detailed check results
  -f, --filter PATTERN  Run only tests matching PATTERN
  --list                List available tests and exit
```

## Test Coverage

### Functional Tests (`run_tests.py`)

| Test | Description | Checks |
|------|-------------|--------|
| `selection_sort` | Selection sort algorithm | Register values, cycle count |
| `vect_add` | Vector addition | Register + memory values |
| `vect_add2` | Vector addition variant | Register values |
| `vect_inc` | Vector increment | Register values |
| `branchpred_1` | Branch prediction test | Register values |
| `ffs_as_log2` | Find first set | Register values |
| `uart_echo_irq` | UART with interrupts | Cycle count (early BREAK) |
| `call_clobber` | Register clobbering | Register values |
| `call_save` | Register saving | Register values |
| `fact_ok` | Factorial | Register values |
| `call_10args` | 10-argument function call | Register values |
| `linus_hello` | Linux-style syscalls | Cycle count (early ECALL) |

### Debug Info Tests (`test_debuginfo.py`)

| Test | Description | Property Checks |
|------|-------------|-----------------|
| `selection_sort_debuginfo` | C program with loops | `.c` file presence, 50% coverage |
| `vect_add_debuginfo` | Mixed C/assembly | `.c` or `.S` presence, 30% coverage |
| `branchpred_debuginfo` | C branch code | `.c` file presence, 50% coverage |
| `ffs_debuginfo` | C bit operations | `.c` file presence, 50% coverage |
| `fact_ok_debuginfo` | Pure assembly | `.S` file presence, 80% coverage |
| `call_10args_debuginfo` | Assembly function | `.S` file presence, 80% coverage |

## Why Property-Based Debug Info Testing?

Traditional exact-match testing is problematic for debug info because:

1. **Compiler variability**: GCC and Clang produce different assembly and line mappings
2. **Version differences**: Compiler updates change generated code
3. **Optimization effects**: Different `-O` levels produce vastly different mappings
4. **Line number drift**: Source file edits shift all line numbers

Property-based testing verifies that debug info *works correctly* without requiring specific line numbers, making tests robust across toolchains and compiler versions.

## Adding New Tests

### Adding a Functional Test

1. Add the source to `TESTS_TO_BUILD` in `build_tests.py`:
   ```python
   ("my_test", "seminaries/qtrvsim/my-test"),
   ```

2. Add expected results to `TESTS` in `run_tests.py`:
   ```python
   {
       "name": "my_test",
       "elf_path": "elfs/my_test.elf",
       "description": "My new test",
       "expected_registers": {"a0": 42},
       "min_cycles": 10, "max_cycles": 100,
   },
   ```

### Adding a Debug Info Test

Add to `DEBUGINFO_TESTS` in `test_debuginfo.py`:
```python
{
    "name": "my_test_debuginfo",
    "elf": "my_test.elf",
    "description": "Verify debug info for my test",
    "expected_files": [".c"],  # or [".S"] for assembly
    "require_debuginfo_coverage": 0.5,  # 50% minimum coverage
},
```

## Troubleshooting

### "Tool not found: riscv64-unknown-elf-gcc"

Install a RISC-V toolchain:
- **Arch Linux**: `pacman -S riscv64-elf-gcc`
- **Ubuntu**: `apt install gcc-riscv64-unknown-elf`
- **macOS**: `brew install riscv64-elf-gcc`

Or use Clang with `--use-clang` flag.

### "ELF not found"

Run `build_tests.py` first to compile the test programs.

### Debug info coverage too low

This can happen if:
- The ELF was compiled without `-g` flag
- The compiler stripped debug info
- The source uses macros/inline assembly without debug info

Check the build output and ensure `-g` is in the compiler flags.