#include "piv.s"

# acc conf
.set W, 2

# tableau
.set x_s, 13 # x13 == a3
.set M, 4
.set N, 4

# this pivot
.set p, 2
.set q, 1
.set a_pq, 0x41000000 # 8.0 in fp32

#                          |
#                          v
#     a[0][0]=-3.0; a[0][1]= 5.0; a[0][2]=-7.0; a[0][3]= 5.0;
#     a[1][0]=-2.0; a[1][1]= 7.0; a[1][2]= 7.0; a[1][3]=-6.0;
# --> a[2][0]=-6.0; a[2][1]= 8.0; a[2][2]=-1.0; a[2][3]= 5.0;
#     a[3][0]= 3.0; a[3][1]= 9.0; a[3][2]= 3.0; a[3][3]= 4.0;

# R=3, W=2, x_s=x13: piv.s will use the registers x13-x17==a3-a7

.global main
main:
    # set W
    li t0, W
    nop # TODO: instr to set W in the accelerator

    # load prepiv constants
    li a0, x_s
    li a1, M
    li t1, N

    prepiv a0, a1, t1

    # load piv constants
    li a0, p
    li a1, q
    li a2, a_pq

    # calc reciprocal pivot element
    li t0, 0x3f800000 # 1.0 in fp32
    fdiv.s a2, t0, a2

    jal accpiv_M4_N4_W2

    # done
    li a0, 0
    ret


accpiv_M4_N4_W2:
    la t0, tableau

    # calc addr of pivot row (row 2)
    mul t2, a0, t1
    add t2, t2, t0

    piv.s a0, a1, a2

    ### band 0

    # W regs
    lw a3, 0(t2) # a3==x_s
    lw a4, 4(t2)

    # R regs
    lw x0,  4(t0) # a[0][1] change row
    lw a5,  0(t0) # a[0][0]
    lw x0, 20(t0) # a[1][1] change row
    lw a6, 16(t0) # a[1][0]
    lw x0, 52(t0) # a[3][1] change row
    lw a7, 48(t0) # a[3][0]

    ### band 1

    # W regs
    sw a3,  0(t2)
    lw a3,  8(t2)
    sw a4, 4(t2)
    lw a4, 12(t2)

    # R regs
    lw x0,  4(t0) # a[0][1] change row
    sw a5,  0(t0) # a[0][0] store prev
    lw a5,  8(t0) # a[0][2]
    sw a6, 16(t0) # a[1][0] store prev
    lw a6, 12(t0) # a[0][3]
    lw x0, 20(t0) # a[1][1] change row
    sw a7, 48(t0) # a[3][0] store prev
    lw a7, 24(t0) # a[1][2]
    sw a5,  8(t0) # a[0][2] store prev
    lw a5, 28(t0) # a[1][3]
    lw x0, 52(t0) # a[3][1] change row
    sw a6, 12(t0) # a[0][3] store prev
    lw a6, 56(t0) # a[3][2]
    sw a7, 24(t0) # a[1][2] store prev
    lw a7, 60(t0) # a[3][3]

    # remaining stores
    sw a3,  8(t2)
    sw a4, 12(t2)
    sw a5, 28(t0) # a[1][3]
    sw a6, 56(t0) # a[3][2]
    sw a7, 60(t0) # a[3][3]

    # TODO: band 2 w/ extra column

    # pivot col
    sw x0,  4(t0) # a[0][1]
    sw x0, 20(t0) # a[1][1]
    sw x0, 52(t0) # a[3][1]
    # a[2][1] already stored

    ret

.section .data
tableau:
    # row 0
    .word 0xc0400000 # a[0][0]=-3.0
    .word 0x40a00000 # a[0][1]= 5.0
    .word 0xc0e00000 # a[0][2]=-7.0
    .word 0x40a00000 # a[0][3]= 5.0

    # row 1
    .word 0xc0000000 # a[1][0]=-2.0
    .word 0x40e00000 # a[1][1]= 7.0
    .word 0x40e00000 # a[1][2]= 7.0
    .word 0xc0c00000 # a[1][3]=-6.0

    # row 2
    .word 0xc0c00000 # a[2][0]=-6.0
    .word 0x41000000 # a[2][1]= 8.0
    .word 0xbf800000 # a[2][2]=-1.0
    .word 0x40a00000 # a[2][3]= 5.0

    # row 3
    .word 0x40400000 # a[3][0]= 3.0
    .word 0x41100000 # a[3][1]= 9.0
    .word 0x40400000 # a[3][2]= 3.0
    .word 0x40800000 # a[3][3]= 4.0
