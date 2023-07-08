//  Template file with simple memory example
//  QtRVSim simulator https://github.com/cvut/qtrvsim/
//
//  template-os.S       - example file
//
//  (C) 2021 by Pavel Pisa
//      e-mail:   pisa@cmp.felk.cvut.cz
//      homepage: http://cmp.felk.cvut.cz/~pisa
//      work:     http://www.pikron.com/
//      license:  public domain

// Directives to make interesting windows visible
#pragma qtrvsim show registers
#pragma qtrvsim show memory

.globl _start
.option norelax

.text

_start:
loop:
	// load the word from absolute address
	lw     x2, 0x400(x0)
	// store the word to absolute address
	sw     x2, 0x404(x0)

	// stop execution wait for debugger/user
	// ebreak
	// ensure that continuation does not
	// interpret random data
	beq    x0, x0, loop
	nop
	nop
	ebreak

.data
.org 0x400

src_val:
	.word  0x12345678
dst_val:
	.word  0

// Specify location to show in memory window
#pragma qtrvsim focus memory src_val
