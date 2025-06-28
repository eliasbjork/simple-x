/*
 * Everything is linked in RAM, and we rely on a bootloader
 * (../../fusesoc_libraries/veerwolf/sw/bootloader.vh) to actually put it there
 * by copying it from flash. The bootloader copies from address 0x0 in flash to
 * address 0x0 in ram, which allows us to link everything to be offset from 0x0
 * here.
 */

OUTPUT_ARCH( "riscv" )
ENTRY(_start)

MEMORY
{
  ram (rwx) : ORIGIN = 0x00000000, LENGTH = 128M
}

/* stack begins at end of ram */
_stack_start = ORIGIN(ram) + LENGTH(ram);

SECTIONS
{
  /* program code */
  .text : ALIGN(4)
  {
    KEEP(*(.init))
    *(.text*)
  } > ram

  /* read-only data */
  .rodata : ALIGN(4)
  {
    *(.rodata*)
    *(.srodata*)
  } > ram

  /* initialized global and static variables */
  .data : ALIGN(4)
  {
    *(.data*)
    *(.sdata*)
  } > ram

  /* zero initialized data */
  .bss : ALIGN(4)
  {
    _bss_start = .;
    *(.bss)
    *(.sbss*)
    *(COMMON)       /* unitialized data */
    . = ALIGN(4);
    _bss_end = .;
  } > ram

  . = ALIGN(4);
  _heap_start = .;
}