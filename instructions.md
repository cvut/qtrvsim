Instructions
============
This is list of all MIPS1 instructions and their implementation status in QtMips.

Explanation of checkboxes:
* [ ] Not tested
* [?] Somewhat tested but not sure about correctness of implementation
* [x] Tested

CPU Arithmetic Instruction
--------------------------
* [x] ADD
* [x] ADDI
* [x] ADDIU
* [x] ADDU
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
* [x] SLT
* [x] SLTI
* [x] SLTIU
* [x] SLTU
* [x] SUB
* [x] SUBU

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
* [x] AND
* [x] ANDI
* [ ] LUI
* [x] NOR
* [x] OR
* [x] ORI
* [x] XOR
* [x] XORI

CPU Move Instruction
--------------------
* [x] MFHI
* [x] MFLO
* [x] MTHI
* [x] MTHO
* [ ] MOVN
* [ ] MOVZ
* MOVF, MOVT won't be implemented as floating coprocessor won't be implemented

CPU Shift Instructions
----------------------
* [x] SLL
* [x] SLLV
* [?] SRA
* [?] SRAV
* [x] SRL
* [x] SRLV

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
