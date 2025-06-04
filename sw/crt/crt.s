.section .text.init
.global _start
_start:
        la sp, _stack
        call main
