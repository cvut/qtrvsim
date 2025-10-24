// I-TLB test — map code pages at MAP_VA and execute tiny functions placed on eight
// pages to print A–H, exercising instruction fetch from different pages.

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

    // Compute root page table entry location for MAP_VA and write a leaf PTE.
    srli t3, t2, 22
    slli t3, t3, 2
    add t4, t0, t3

    // Build leaf PTE: PPN from MAP_VA + full flags and store into root PT.
    srli t5, t2, 12
    slli t5, t5, 10
    li t6, PTE_FLAGS_FULL
    or t5, t5, t6
    sw t5, 0(t4)
    fence

    // Enable SATP: construct satp value from ROOT_PT_PHYS (ppn) and MODE bit(s),
    // then write to satp and fence. This form shifts root physical >> 12 and ORs
    // with the mode bit mask (implementation-specific).
    li t0, ROOT_PT_PHYS
    srli t0, t0, 12
    li t1, 0x80000000
    or t0, t0, t1
    csrw satp, t0
    fence

    // Prepare mstatus.MPP so mret will return to Supervisor.
    li t0, MSTATUS_MPP_CLEAR
    csrrc zero, mstatus, t0
    li t0, MSTATUS_MPP_SET
    csrrs zero, mstatus, t0

    // Set mepc to vm_entry and enter Supervisor with mret.
    la t0, vm_entry
    csrw mepc, t0
    mret


.org 0xC4007000
.text
vm_entry:
    li t0, SERIAL_PORT_BASE
    li t4, 65 // 'A'

    // print_self: print current char, then step through executing code on other mapped pages.
print_self:
    lw t6, SERP_TX_ST_REG_o(t0)
    andi t6, t6, SERP_TX_ST_REG_READY_m
    beq t6, zero, print_self
    sw t4, SERP_TX_DATA_REG_o(t0)
    addi t4, t4, 1

    // Execute code placed in separate mapped pages. Each page contains a tiny
    // function that prints the current character then returns using the address
    // stored in t3. The sequence tests I-TLB / instruction fetch of different pages.
    la t1, page_func_0
    la t3, resume_0
    jalr zero, 0(t1)
resume_0:
    addi t4, t4, 1

    la t1, page_func_1
    la t3, resume_1
    jalr zero, 0(t1)
resume_1:
    addi t4, t4, 1

    la t1, page_func_2
    la t3, resume_2
    jalr zero, 0(t1)
resume_2:
    addi t4, t4, 1

    la t1, page_func_3
    la t3, resume_3
    jalr zero, 0(t1)
resume_3:
    addi t4, t4, 1

    la t1, page_func_4
    la t3, resume_4
    jalr zero, 0(t1)
resume_4:
    addi t4, t4, 1

    la t1, page_func_5
    la t3, resume_5
    jalr zero, 0(t1)
resume_5:
    addi t4, t4, 1

    la t1, page_func_6
    la t3, resume_6
    jalr zero, 0(t1)
resume_6:
    addi t4, t4, 1

    ebreak


.org 0xC4000000
page_func_0:
    // Each page_func_* polls transmitter, writes current char (t4) then jumps
    // back to the resume address stored in t3. These functions live on
    // separate mapped pages to exercise instruction fetch from different pages.
    lw t6, SERP_TX_ST_REG_o(t0)
    andi t6, t6, SERP_TX_ST_REG_READY_m
    beq  t6, zero, page_func_0
    sw t4, SERP_TX_DATA_REG_o(t0)
    jalr zero, 0(t3)

.org 0xC4001000
page_func_1:
    lw t6, SERP_TX_ST_REG_o(t0)
    andi t6, t6, SERP_TX_ST_REG_READY_m
    beq t6, zero, page_func_1
    sw t4, SERP_TX_DATA_REG_o(t0)
    jalr zero, 0(t3)

.org 0xC4002000
page_func_2:
    lw t6, SERP_TX_ST_REG_o(t0)
    andi t6, t6, SERP_TX_ST_REG_READY_m
    beq t6, zero, page_func_2
    sw t4, SERP_TX_DATA_REG_o(t0)
    jalr zero, 0(t3)

.org 0xC4003000
page_func_3:
    lw t6, SERP_TX_ST_REG_o(t0)
    andi t6, t6, SERP_TX_ST_REG_READY_m
    beq t6, zero, page_func_3
    sw t4, SERP_TX_DATA_REG_o(t0)
    jalr zero, 0(t3)

.org 0xC4004000
page_func_4:
    lw t6, SERP_TX_ST_REG_o(t0)
    andi t6, t6, SERP_TX_ST_REG_READY_m
    beq t6, zero, page_func_4
    sw t4, SERP_TX_DATA_REG_o(t0)
    jalr zero, 0(t3)

.org 0xC4005000
page_func_5:
    lw t6, SERP_TX_ST_REG_o(t0)
    andi t6, t6, SERP_TX_ST_REG_READY_m
    beq t6, zero, page_func_5
    sw t4, SERP_TX_DATA_REG_o(t0)
    jalr zero, 0(t3)

.org 0xC4006000
page_func_6:
    lw t6, SERP_TX_ST_REG_o(t0)
    andi t6, t6, SERP_TX_ST_REG_READY_m
    beq t6, zero, page_func_6
    sw t4, SERP_TX_DATA_REG_o(t0)
    jalr zero, 0(t3)