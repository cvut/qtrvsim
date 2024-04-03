label:
nop
nop
la x1, label
auipc x2, %pcrel_hi(label)
addi x2, x2, %pcrel_lo(label)
bne x1, x1, fail
ebreak
nop
nop

fail:
.data
.word 0
