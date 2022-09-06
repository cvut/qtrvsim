# qtrvsim-testing
Addaptation of official RISC-V tests for qtrvsim.

## Requirements
- [qtrvsim-cli](https://github.com/cvut/qtrvsim)
- riscv64-unknown-elf-gcc
- riscv64-unknown-elf-binutils
- Python3 (>= 3.4)
- Cmake
- Git

## Instructions
1. Setup qtrvsim
2. Run

        git clone https://github.com/veznitom/qtrvsim-testing
        cd qtrvsim-testing
        git submodule update --init --recursive
        python qtrvsim_tester.py /path/to/qtrvsim-cli

- For more information use:

        python qtrvsim_tester.py -h
