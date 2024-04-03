lw x0, 0(x0)
lw x0, fail(x0)
lw x0, %lo(label)(x0)
addi x0, x0, %lo(0xDEA)

li x1, label
lui x2, %hi(label)
addi x2, x2, %lo(label)
bne x1, x2, fail
ebreak
nop
nop

fail:
.data
.word 0


.org 0xABCDE123
label: