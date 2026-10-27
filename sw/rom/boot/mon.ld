/*
 * Raven ROM monitor.
 */

OUTPUT_ARCH(m68k)
OUTPUT_FORMAT(elf32-m68k)
ENTRY(start)

MEMORY
{
    ROM(RX) : ORIGIN = 0x40000000, LENGTH = 512k
    RAM(RW) : ORIGIN = 0x00501000, LENGTH = 0x000ff000
}

__export_size   = __export_end - __export_start;
__stack_size    = 1024 * 16;
__pram_size     = 1024 * 16;

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
        . = 0x1000;                         /* size can be adjusted, we don't need all */
        __export_end = .;

        . = ALIGN(4);
        hw/motorola_sp.o(.text* .rodata*)   /* fills up the space between fixed-location areas */

        . = ALIGN(__pram_size);
        __pram_start = .;
        KEEP(*(.pram))                      /* pram stored in flash */
        FILL(0xFF);
        . = __pram_start + __pram_size;
        __pram_end = .;

        __code_start = .;
        *(.text* .rodata* .eh_frame*)       /* text and text-like things */
        . = ALIGN(16);
        __text_end = .;
    } >ROM

    .bss.sysvar :
    {
        __sysvar_start = .;                 /* sysvar bss section at bottom of ram */
        *(.bss.sysvar);
        . = ALIGN(16);
        __sysvar_end = .;
    } >RAM

    .data (READONLY):
    {
        . = ALIGN(16);
        __data_start = .;
        *(.data* .ramtext*)                 /* non-constant initialised data */
        . = ALIGN(4);
        __data_end = .;
    } >RAM AT>ROM

    __data_load = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        __bss_start = .;
        *(.bss);                            /* non-constant, non-initialised data */
        *(.bss.*);

        . = ALIGN(4);
        __bss_end = .;

        . = ALIGN(4096);                    /* BIOS heap */
        __heap_start = .;

        . = ORIGIN(RAM) + LENGTH(RAM) - __stack_size - 256;
        __heap_end = .;
        __stack_base = .;                   /* BIOS stack */
        . += __stack_size;
        __stack_top = .;

    } >RAM

    .end :
    {
        FILL(0xFFFFFFFF);
        BYTE(0xFF);
        . = ALIGN(4096);
        __mon_end = .;
    } >ROM

}
