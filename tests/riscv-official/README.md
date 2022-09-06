# qtrvsim-testing

Adaptation of official RISC-V tests for QtRvSim.

## Dependencies

- qtrvsim-cli
- riscv64-unknown-elf-gcc OR clang
- riscv64-unknown-elf-binutils OR llvm
- python (>= 3.4)

Note: with gcc make sure it supports all the used march options.

## Usage

```shell
python qtrvsim_tester.py /path/to/qtrvsim-cli
```

- For more information use: `python qtrvsim_tester.py -h`

## Clang

To use clang instead of gcc set those environment variables:

```shell
export RISCV_COMPILER=clang
export USE_CLANG_OPTS=true
export RISCV_OBJDUMP_CMD=llvm-objdump
```
