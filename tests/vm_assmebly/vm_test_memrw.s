// Write ASCII bytes into the mapped virtual page and read them back printing to verify.

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

    // pointer to mapped virtual page
    la a1, MAP_VA

    // write ASCII letters A..H into the first 8 bytes
    li t0, 65
    sb t0, 0(a1)
    li t0, 66
    sb t0, 1(a1)
    li t0, 67
    sb t0, 2(a1)
    li t0, 68
    sb t0, 3(a1)
    li t0, 69
    sb t0, 4(a1)
    li t0, 70
    sb t0, 5(a1)
    li t0, 71
    sb t0, 6(a1)
    li t0, 72
    sb t0, 7(a1)

    // Now read back and print each byte
    li t5, 0
read_print_loop:
    lb t1, 0(a1)
    // print t1
wait_tx2:
    lw t0, SERP_TX_ST_REG_o(a0)
    andi t0, t0, SERP_TX_ST_REG_READY_m
    beq t0, zero, wait_tx2
    sw t1, SERP_TX_DATA_REG_o(a0)

    addi a1, a1, 1
    addi t5, t5, 1
    li t6, 8
    blt t5, t6, read_print_loop

    ebreak