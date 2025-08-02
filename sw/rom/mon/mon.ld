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

__export_size = __export_end - __export_start;
__config_size = __config_end - __config_start;

SECTIONS
{
    .text :
    {
        . = 0x0000;
        __vectors_start = .;
        KEEP(*(.vectors))                   /* early ROM vector table */
        . = 0x0400;
        __toc_start = .;
        KEEP(*(.toc))                       /* rom toc */
        . = 0x0800;
        __toc_end = .;
        __export_start = .;
        KEEP(*(.export))                    /* function exports */
        . = 0x1000;
        __export_end = .;
        __config_start = .;
        KEEP(*(.config))                    /* configs stored in ROM */
        . = 0x2000;
        __config_end = .;
        __code_start = .;
        *(.text* .rodata* .eh_frame*)       /* text and text-like things */
        . = ALIGN(4);
        __text_end = .;
    } >ROM

    .data (READONLY):
    {
        __data_start = .;
        *(.data* .ramtext*)                 /* non-constant initialised data */
        . = ALIGN(4);
        __data_end = .;
    } >RAM AT>ROM

    __data_load = LOADADDR(.data);

    .end :
    {
        FILL(0xFFFFFFFF);
        BYTE(0xFF);
        . = ALIGN(4096);
        __mon_end = .;
    } >ROM

    .bss :
    {
        __bss_start = .;
        *(.bss);                            /* non-constant, non-initialised data */
        *(.bss.*);
        . = ALIGN(4);
        __bss_end = .;

        __stack_base = .;                   /* BIOS stack */
        . = ORIGIN(RAM) + LENGTH(RAM) - 4;
        . = ALIGN(4);
        __stack_top = .;
    } > RAM
}
