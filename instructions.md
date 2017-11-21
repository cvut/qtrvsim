Instructions
============
This is list of all MIPS1 instructions and their implementation status in QtMips.

Explanation of checkboxes:
* [ ] Not tested
* [?] Somewhat tested but not sure about correctness of implementation
* [-] Tested non-pipelined core
* [x] Tested on non-pipelined and pipelined core

CPU Arithmetic Instruction
--------------------------
* [-] ADD
* [ ] ADDI
* [ ] ADDIU
* [-] ADDU
* [ ] CLO
* [ ] CLZ
* [ ] DIV
* [ ] DIVU
* [ ] MADD
* [ ] MADDU
* [ ] MSUB
* [ ] MSUBU
* [ ] MUL
* [ ] MULT
* [ ] MULTU
* [-] SLT
* [ ] SLTI
* [ ] SLTIU
* [?] SLTU
* [-] SUB
* [-] SUBU

CPU Branch and Jump Instructions
--------------------------------
* [ ] B
* [ ] BAL
* [ ] BEQ
* [ ] BGEZ
* [ ] BGEZAL
* [ ] BGTZ
* [ ] BLEZ
* [ ] BLTZ
* [ ] BLTZAL
* [ ] BNE
* [ ] J
* [ ] JAL
* [ ] JALR
* [ ] JR

CPU No Instructions
-------------------
* [ ] NOP
* [ ] SSNOP

CPU Load, Store and Memory Control Instructions
-----------------------------------------------
* [ ] LB
* [ ] LBU
* [ ] LH
* [ ] LHU
* [ ] LL
* [ ] LW
* [ ] LWL
* [ ] LWR
* [ ] PREF
* [ ] SB
* [ ] SC
* [ ] SD
* [ ] SH
* [ ] SW
* [ ] SWL
* [ ] SWR
* [ ] SYNC

CPU Logical Instructions
------------------------
* [ ] AND
* [ ] ANDI
* [ ] LUI
* [ ] NOR
* [ ] OR
* [ ] ORI
* [ ] XOR
* [ ] XORI

CPU Move Instruction
--------------------
* [ ] MFHI
* [ ] MFLO
* [ ] MTHI
* [ ] MTHO
* [ ] MOVN
* [ ] MOVZ
* MOVF, MOVT won't be implemented as floating coprocessor won't be implemented

CPU Shift Instructions
----------------------
* [-] SLL
* [-] SLLV
* [?] SRA
* [?] SRAV
* [-] SRL
* [-] SRLV

CPU Trap Instructions
---------------------
Following instruction will be implemented but only as dummy ones as we are not
implementing privileged instructions. They will all result to exception and core
execution stop.

* [ ] BREAK
* [ ] TEQ
* [ ] TEQI
* [ ] TGE
* [ ] TGEI
* [ ] TGEIU
* [ ] TGEU
* [ ] TLT
* [ ] TLTI
* [ ] TLTIU
* [ ] TLTU
* [ ] TNE
* [ ] TNEI
* SYSCALL won't be implemented at all

Obsolete CPU branch instructions
--------------------------------
None of these instructions will be implemented as they are marked as obsolete and
they shouldn't be used by compiler and the same way by programmers.

FPU Instructions
----------------
No FPU instruction will be implemented as no FPU is planned to be implemented.

Coprocessor Instructions
------------------------
None of these will be implemented as coprocessor 2 won't be implemented.

Privileged instructions
-----------------------
None of these will be implemented as privileged state won't be implemented.

EJTAG Instructions
------------------
No instruction will be implemented as no ejtag support.
