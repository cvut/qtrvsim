.text
.set noat
.set noreorder
.globl _start

_start:
	addi $1, $0, 1
loop:
	j loop
	add $1, $1, $1
