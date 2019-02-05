.text
.globl _start		  
.set noat
.set noreorder
.ent _start

_start:
	addi $1, $0, 6
	j _start
