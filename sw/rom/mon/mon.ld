/*
 * Raven ROM monitor.
 */

OUTPUT_ARCH(m68k)
OUTPUT_FORMAT(elf32-m68k)
ENTRY(start)

MEMORY
{
	ROM(RX) : ORIGIN = 0x40000000, LENGTH = 256k
	RAM(RW) : ORIGIN = 0x00501000, LENGTH = 0x000ff000
}

__rom_end = ORIGIN(ROM) + LENGTH(ROM);
__etos_signature = __rom_end + 0x2c;

SECTIONS
{
	.text :
	{
		KEEP(*(.vectors)) 			/* early ROM vector table */
		*(.text* .rodata*)			/* text and text-like things */
		. = ALIGN(4);
		__text_end = .;
	} >ROM

	.data :
	{
		__data_start = .;
		*(.data* .ramtext*)				/* non-constant initialised data */
		. = ALIGN(4);
		__data_end = .;
	} >RAM AT>ROM

	__data_load = LOADADDR(.data);

	.bss :
	{
		__bss_start = .;
		*(.bss);					/* non-constant, non-initialised data */
		*(.bss.*);
		. = ALIGN(4);
		__bss_end = .;

		__stack_base = .;			/* BIOS stack */
		. = ORIGIN(RAM) + LENGTH(RAM) - 4;
		. = ALIGN(4);
		__stack_top = .;
	} > RAM
}
