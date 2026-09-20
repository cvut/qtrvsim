//  Simple example of working of RISC-V MMU function
//  QtRVSim simulator https://github.com/cvut/qtrvsim/
//
//  page-to-page-copy.S  - MMU use example file
//
//  (C) 2026 by Petr Stepan
//      e-mail:   stepan@fel.cvut.cz
//      https://mrs.fel.cvut.cz/people/petr-stepan
//      license:  public domain

// The example maps two data pages into the virtual
// address space, then switches the CPU to S-mode
// and copies 1024 words (the whole 4 kB page)
// from the "source_page" (physical address 0x1000,
// PFN 1, virtual address 0x6000, VPFN 7)
// to the "target_page" (physical address 0x2000 PFN 2,
// virtual address 0x7000, VPFN 7).
// The code needs to stay accessible after the CPU switch
// to Supervisor mode. This is achieved by identity mapping
// of code page 0 to the same virtual address (code starting
// at 0x200, which allows its size to grow up
// to 0x1000-0x200=0xE00=3584 bytes for single page mapping).

// To run the example, enable "Virtual memory" in
// the "New simulation" dialog. The "target_page" page can
// be visualized at its physical address 0x2000 when "Direct"
// is chosen in memory view or at address 0x7000
// when "As CPU (VMA)" is chosen.

.text
.globl _start

.equ PAGE_SIZE  4096
.equ ATTR  0x0CF
.equ PTE_V 1
# PTE_V = 0x001
# PTE_R = 0x002
# PTE_W = 0x004
# PTE_X = 0x008
# PTE_A = 0x040
# PTE_D = 0x080

# -------------------------------------------------
# start v M-mode
# -------------------------------------------------

_start:

# setup handlers for pagefaults, exceptions and interrupts

    li t0, m_fault     # set M-mode exception vector
    csrw mtvec, t0

    li t0, s_fault     # set S-mode exception vector
    csrw stvec, t0

# setup page table
    la a0, _start      # do identity mapping of the code
    addi a1, a0, 0
    call map_page

    li a0, 0x6000      # map source_page to virtual
    la a1, source_page # page address 0x6000
    call map_page

    li a0, 0x7000      # map target_page to virtual
    la a1, target_page # page address 0x7000
    call map_page

# Setup 32-bit RISC-V page translation mode (Sv32)

    la t0, l1_table
    srli t0, t0, 12

    li t1, 0x80000000
    or t0, t0, t1

    csrw satp, t0
    fence

# Switch CPU to S-mode and start S-mode code

    la t0, s_mode_start
    csrw mepc, t0

    csrr t1, mstatus

    li t2, 0xffffe700  # ~(3 << 11)     # clear MPP
    and t1, t1, t2

    li t2, 0x800       # (1 << 11)      # MPP = S
    or t1, t1, t2

    li t2, 0x40000     # (1 << 18)      # SUM = 1
    or  t1, t1, t2     # enable CPU running in S-mode
                       # to access pages mapped as unprivileged
    csrw mstatus, t1

    mret

# -------------------------------------------------
# map_page(a0 = virtual address, a1 = physical address)
# -------------------------------------------------

map_page:

    mv t0, a0

# VPN1

    srli t1, t0, 22
    andi t1, t1, 0x3ff

    slli t1, t1, 2
    la t2, l1_table
    add t3, t2, t1

# when L1 is epty → setup mapping at L0

    lw t4, 0(t3)
    bnez t4, l1_ok

    la t5, l0_table
    srli t5, t5, 12
    slli t5, t5, 10
    ori t5, t5, 0X11

    sw t5, 0(t3)

l1_ok:

# VPN0

    srli t1, t0, 12
    andi t1, t1, 0x3ff

    slli t1, t1, 2
    la t2, l0_table
    add t3, t2, t1

# setup page table entry (PTE)

    srli t4, a1, 12
    slli t4, t4, 10

    li t5, 0x1f        # setup page privileges in PTE 
    or t4, t4, t5      # D = 0, A = 0, G = 0, U = 1
                       # X = 1, W = 1, R = 1, V = 1
    sw t4, 0(t3)

    ret

# -------------------------------------------------
# service routines for pagefaults, exceptions and interrupts
# -------------------------------------------------

m_fault:
    ebreak
    j m_fault

s_fault:
    ebreak
    j s_fault

# -------------------------------------------------
# S-mode "kernel" code running with page translation
# -------------------------------------------------

s_mode_start:

    li t0, 0x6000
    li t1, 0x7000
    li t2, 1024

copy_loop:

    lw t3, 0(t0)       # read,  page 6 set A to 1 
    lw t4, 0(t1)       # dummy, page 7 set A to 1
    sw t3, 0(t1)       # write, page 7 set D to 1

    addi t0, t0, 4
    addi t1, t1, 4

    addi t2, t2, -1
    bnez t2, copy_loop

hang:
    ebreak
    j hang



#-------------------------------------------------
# Data pages
#-------------------------------------------------

.data
.org 0x1000    # to be mapped to 0x6000 virtual address
source_page:
    .word 0x12345678, 1,2,3,4,5,6,7, 8, 9, 10, 11, 12 ,13, 14, 15
    .word 0,1,2,3,4,5,6,7, 8, 9, 10, 11, 12 ,13, 14, 15
    .word 0,1,2,3,4,5,6,7, 8, 9, 10, 11, 12 ,13, 14, 15
    .word 0,1,2,3,4,5,6,7, 8, 9, 10, 11, 12 ,13, 14, 15
    .word 0,1,2,3,4,5,6,7, 8, 9, 10, 11, 12 ,13, 14, 15
    .word 0,1,2,3,4,5,6,7, 8, 9, 10, 11, 12 ,13, 14, 15
    .word 0,1,2,3,4,5,6,7, 8, 9, 10, 11, 12 ,13, 14, 15
    .word 0,1,2,3,4,5,6,7, 8, 9, 10, 11, 12 ,13, 14, 15


.org 0x2000    # to be mapped to 0x7000 virtual address
target_page:
    .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    .word 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    .word 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    .word 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    .word 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1

#-------------------------------------------------
# Page table
#-------------------------------------------------

.org 0x3000    # the page table directory start to be set into satp
l1_table:
    .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

.org 0x4000    # the last level page table with PTE entries 
l0_table:
    .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
