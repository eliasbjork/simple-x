# rounding modes
# defined here, as .insn emitted instructions don't understand them by default
.set rtz, 1
.set rne, 0
.set rdn, 2
.set rup, 3
.set rmm, 4
.set dyn, 7

# R4 type: .insn r4 opcode7, rm3, fmt2, rd, rs1, rs2, rs3
#     +-----+------+-----+-----+-----+----+---------+
#     | rs3 | fmt2 | rs2 | rs1 | rm3 | rd | opcode7 |
#     +-----+------+-----+-----+-----+----+---------+
#     31    27     25    20    15    12   7         0

.macro prepiv rs1, rs2, rs3
    .insn r4 0x0b, 0x0, 0x0, zero, \rs1, \rs2, \rs3
.endm

.macro piv.s rs1, rs2, rs3, rm=dyn
    .insn r4 0x2b, \rm, 0x0, zero, \rs1, \rs2, \rs3
.endm
