/*
 * Raven monitor upload.
 */

OUTPUT_ARCH(m68k)
OUTPUT_FORMAT(elf32-m68k)
ENTRY(start)

MEMORY
{
	RAM(RW) : ORIGIN = 0x00000400, LENGTH = 0x001ffc00
}

SECTIONS
{
	.text :
	{
		*(.text* .rodata*)			/* text and text-like things */
	} >RAM

	.data :
	{
		__data_start = .;
		*(.data*)					/* non-constant initialised data */
		. = ALIGN(4);
		__data_end = .;
	} >RAM

	.bss :
	{
		__bss_start = .;
		*(.bss*);					/* non-constant, non-initialised data */
		. = ALIGN(4);
		__bss_end = .;

		__stack_base = .;			/* stack */
		. = ORIGIN(RAM) + LENGTH(RAM) - 4;
		. = ALIGN(4);
		__stack_top = .;
	} > RAM
}
