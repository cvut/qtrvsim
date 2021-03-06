// Directives to make interresting windows visible
#pragma qtmips show registers
#pragma qtmips show memory

.globl _start
.set noat
.set noreorder

.text

_start:
loop:
	// load the word from absolute address
	lw     $2, 0x2000($0)
	// store the word to absolute address
	sw     $2, 0x2004($0)

	// stop execution wait for debugger/user
	//break
	// ensure that continuation does not
	// interpret random data
	beq    $0, $0, loop
	nop

.data
.org	0x2000

src_val:
	.word  0x12345678
dst_val:
	.word  0

// Specify location to show in memory window
#pragma qtmips focus memory src_val
