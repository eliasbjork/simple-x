OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0xee000000;
  .text.nmi :
  {
    KEEP(*(.text.nmi*))
  }

  /* program code */
  . = 0x80000000;
  .text.init . :
  {
    *(.text.init)
  }

  .text . :
  {
    *(.text)
  }
  _end = .;

  /* memory mapped host (verilator) communication */
  . = 0xd0580000;
  .data.io . :
  {
    *(.data.io)
  }

  . = 0xf0040000 ;
  .data : ALIGN(0x800)
  {
    *(.*data)
    *(.rodata*)
  }

  .bss :
  {
    _bss_start = .;
    *(.*bss)
    _bss_end = .;
  }

  _heap_start = ALIGN(4);
  _heap_end = ALIGN(4) + 0x1000;
  _stack_start = ALIGN(16) + 0x1000;

  /* The following constants tell the testbench to do ICCM preload */
  . = 0xfffffff0;
  .iccm.ctl :
  {
    LONG(ADDR(.text.nmi));
    LONG(ADDR(.text.nmi) + SIZEOF(.text.nmi))
  }

  . = 0xfffffff8; .data.ctl :
  {
    LONG(0xf0040000);
    LONG(_stack_start)
  }
}
