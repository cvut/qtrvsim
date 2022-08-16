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

    <code>
    git clone https://github.com/veznitom/qtrvsim-testing <br>
    cd qtrvsim-testing <br>
    git submodule update --init --recursive <br> 
    python py-test.py /path/to/qtrvsim-cli
    </code>