.section .init
.global _start
_start:
    # setup stack pointer
    la sp, _stack_start

    # zero-initialize .bss section
    jal _init_bss

    call main

_loop:
    # to catch us if main returns
    j _loop

_init_bss:
    lw t0, _bss_start
    lw t1, _bss_end
    j _bss_bounds_check
_clear_bss:
    sw a0, 0(t0)
    addi t0, t0, 4
_bss_bounds_check:
    bltu t0, t1, _clear_bss
    ret
