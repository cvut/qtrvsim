    .org 0x00000200
    .text
    .globl _start
_start:
    la   t0, data1
    lw   t1, 0(t0)
    addi t1, t1, 1
    sw   t1, 0(t0)
    ebreak
//
    .org 0x00002000
    .data
data1:
    .word 0x12345678


