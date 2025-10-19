// Place a tiny function in the mapped virtual page and jump to it (tests X bit).

.globl _start
.option norelax

// Serial port/terminal registers
.equ SERIAL_PORT_BASE,      0xffffc000 // base address of serial port region
.equ SERP_RX_ST_REG_o,      0x0000 // Offset of RX_ST_REG
.equ SERP_RX_DATA_REG_o,    0x0004 // Offset of RX_DATA_REG
.equ SERP_TX_ST_REG_o,      0x0008 // Offset of TX_ST_REG
.equ SERP_TX_ST_REG_READY_m,0x1 // Transmitter can accept next byte
.equ SERP_TX_DATA_REG_o,    0x000c // Offset of TX_DATA_REG

.equ ROOT_PT_PHYS, 0x1000
.equ MAP_VA, 0xC4000000

// PTE flags: 0xCF = 11001111
// bit0 V = 1 (Valid)
// bit1 R = 1 (Readable)
// bit2 W = 1  Writable)
// bit3 X = 1 (Executable)
// bit4 U = 0 (NOT user-accessible)
// bit5 G = 0 (NOT global)
// bit6 A = 1 (Accessed)
// bit7 D = 1 (Dirty)
.equ PTE_FLAGS_FULL, 0xCF

// mstatus MPP manipulation masks (for preparing mret to change privilege)
.equ MSTATUS_MPP_CLEAR, 0x1000 // mask to clear MPP[12] (set bit 12 -> will be cleared via csrrc)
.equ MSTATUS_MPP_SET, 0x800 // mask to set MPP[11]  (set bit 11 -> will be set via csrrs)

.equ SATP_ENABLE, 0x80000001 // satp value to enable paging (implementation-specific)

.org 0x00000200
.text
_start:
    // t0 = physical address of root page table
    li t0, ROOT_PT_PHYS

    // t4 = virtual address we want to map (MAP_VA)
    li t4, MAP_VA

    // Build leaf PTE
    srli t1, t4, 12
    slli t1, t1, 10
    li t6, PTE_FLAGS_FULL
    or t1, t1, t6

    srli t5, t4, 22
    slli t2, t5, 2
    add t3, t0, t2
    sw t1, 0(t3)
    fence

    // Enable SATP
    li t0, 0
    csrw satp, t0
    li t0, SATP_ENABLE
    csrw satp, t0
    fence

    // Prepare mstatus.MPP to return to Supervisor
    li t0, MSTATUS_MPP_CLEAR
    csrrc zero, mstatus, t0
    li t0, MSTATUS_MPP_SET
    csrrs zero, mstatus, t0

    la t0, vm_entry
    csrw mepc, t0
    mret

.org 0xC4000000
.text
vm_entry:
    li a0, SERIAL_PORT_BASE

    // Call the function placed in the same mapped page
    la t0, mapped_function
    jalr zero, 0(t0)

    ebreak

// small function placed in the mapped page
.org 0xC4000100
mapped_function:
    li a0, SERIAL_PORT_BASE
    li t1, 88
wait_tx:
    lw t0, SERP_TX_ST_REG_o(a0)
    andi t0, t0, SERP_TX_ST_REG_READY_m
    beq t0, zero, wait_tx
    sw t1, SERP_TX_DATA_REG_o(a0)
    ebreak
