.text

_start:
loop:
	addi x1, x0, 0x11
	addi x2, x0, 0x22
	addi x3, x0, 0x33
	lw   x4, 0x44(x0)
	addi x5, x4, 0x55
	beq  x0, x0, tgt1
	addi x11, x0, 0x11
	addi x12, x0, 0x22
	addi x13, x0, 0x33
	addi x14, x0, 0x44
	addi x15, x0, 0x55
tgt1:
	addi x21, x0, 0x11
	addi x22, x0, 0x22
	addi x23, x0, 0x33
	addi x24, x0, 0x44
	addi x25, x0, 0x55

	ebreak