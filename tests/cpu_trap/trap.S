.text
.globl _start		  

_start:
	addi $1, $0, 1
loop:
	add $1, $1, $1
	j loop
