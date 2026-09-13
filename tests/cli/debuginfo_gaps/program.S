.text
_start:
    li x1, 0x12345678
    jal x0, untracked
.org 0x220
untracked:
    .word 0x00000013
    ebreak
