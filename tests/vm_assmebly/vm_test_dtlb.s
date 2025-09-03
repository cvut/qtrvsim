// D-TLB test — map data pages at MAP_VA; write 'A'..'H' to the first byte of eight pages and read/print them.

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

    // Compute level-1 table index for MAP_VA and form address of PTE in root page table.
    // srli/slli extracts bits for index and scales by 4 (word-sized PTEs).
    srli t3, t2, 22
    slli t3, t3, 2
    add t4, t0, t3

    // Build leaf PTE: physical page number (vpn->ppn shift) | flags.
    // srli/slli moves MAP_VA to form the physical page number field in the PTE.
    srli t5, t2, 12
    slli t5, t5, 10
    li t6, PTE_FLAGS_FULL
    or t5, t5, t6
    sw t5, 0(t4)
    fence

    // Enable paging: write SATP with implementation-specific value.
    li t0, 0
    csrw satp, t0
    li t0, SATP_ENABLE
    csrw satp, t0
    fence

    // Prepare mstatus.MPP so mret will return to Supervisor (set bits appropriately).
    li t0, MSTATUS_MPP_CLEAR
    csrrc zero, mstatus, t0
    li t0, MSTATUS_MPP_SET
    csrrs zero, mstatus, t0

    // Set mepc to vm_entry and enter Supervisor with mret.
    la t0, vm_entry
    csrw mepc, t0
    mret

.org 0xC4000000
.text
vm_entry:
    li t0, SERIAL_PORT_BASE 
    la t1, MAP_VA // pointer to start of mapped virtual region
    li t2, 0 // page counter
    li t3, 8 // number of pages to write/read (A..H)
    li t4, 65 // ASCII 'A'
    li t5, 0x1000 // page size (4KB)

// write_pages_loop: write one byte (A..H) at the start of each mapped page.
write_pages_loop:
    sb t4, 0(t1)
    add t1, t1, t5
    addi t4, t4, 1
    addi t2, t2, 1
    blt t2, t3, write_pages_loop

    // Reset pointer and counter to read back and print the first byte of each page.
    la t1, MAP_VA
    li t2, 0

read_print_loop:
    lb t6, 0(t1) // load the byte stored at start of current page

    // wait_tx: poll transmitter status until ready, then write byte to TX data reg.
wait_tx:
    lw t4, SERP_TX_ST_REG_o(t0) 
    andi t4, t4, SERP_TX_ST_REG_READY_m
    beq t4, zero, wait_tx
    sw t6, SERP_TX_DATA_REG_o(t0)

    add t1, t1, t5
    addi t2, t2, 1
    blt t2, t3, read_print_loop

    ebreak

1:  auipc t0, 0
    jalr zero, 0(t0)
