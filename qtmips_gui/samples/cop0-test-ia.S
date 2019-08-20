.set SERIAL_PORT_BASE,     0xffffc000

.set SERP_RX_ST_REG_o,           0x00
.set SERP_RX_ST_REG_READY_m,      0x1
.set SERP_RX_ST_REG_IE_m,         0x2

.set SERP_RX_DATA_REG_o,         0x04

.set SERP_TX_ST_REG_o,           0x08
.set SERP_TX_ST_REG_READY_m,      0x1
.set SERP_TX_ST_REG_IE_m,         0x2

.set SERP_TX_DATA_REG_o,         0x0c


.globl _start
.set noat
.set noreorder
.ent _start

.text

_start:
	addi	$1, $0, 0x101
	addi	$2, $0, 0x102
	synci	0($zero)
	addi	$3, $0, 0x103
	addi	$4, $0, 0x104
	la	$20, skip
	mtc0	$20, $14, 0  // EPC
	eret
	addi	$5, $0, 0x105
	addi	$6, $0, 0x106
skip:
	addi	$7, $0, 0x107
	addi	$8, $0, 0x108
	mfc0	$9, $14, 0   // EPC

	break
	mfc0	$10, $14, 0  // EPC

	la	$20, irq_entry - 0x180
	mtc0	$20, $15, 1  // EBase

	li	$21, SERIAL_PORT_BASE
	li	$20, SERP_RX_ST_REG_IE_m
	sw	$20, SERP_RX_ST_REG_o($21)
	li	$20, 0x00000801
	mtc0	$20, $12, 0  // Status

loop:	mfc0	$10, $14, 0  // EPC
	beq	$zero, $zero, loop
	nop


irq_entry:
	mfc0	$26, $13, 0  // Cause
	andi	$27, $26, 0x00000800
	bne	$27, $0, rx_irq
	nop
	mfc0	$27, $12, 0  // Status
	nor	$26, $26, $0
	andi	$26, $26, 0x0000ff00
	and	$27, $27, $26
	mtc0	$27, $12, 0  // Status
	eret

rx_irq:
	li	$26, SERIAL_PORT_BASE
	lw	$27, SERP_RX_DATA_REG_o($26)
	sw	$27, SERP_TX_DATA_REG_o($26)
	eret

.end _start
