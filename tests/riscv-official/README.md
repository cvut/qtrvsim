# qtrvsim-testing
Addaptation of official RISC-V tests for qtrvsim.

## Requirements
- [qtrvsim-cli](https://github.com/cvut/qtrvsim)
- Python3
- Cmake
- Git

## Instructions
1. Setup qtrvsim
2. Run

        git clone https://github.com/veznitom/qtrvsim-testing
        cd qtrvsim-testing
        git submodule update --init --recursive
        python py-test.py /path/to/qtrvsim-cli