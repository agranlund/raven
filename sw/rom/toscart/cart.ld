/*
 * Raven TOS cart
 */

OUTPUT_ARCH(m68k)
OUTPUT_FORMAT(elf32-m68k)
ENTRY(start)

MEMORY
{
    ROM(RX) : ORIGIN = 0x00fa0000, LENGTH = 128k
/*  RAM(RW) : ORIGIN = 0x005fff00, LENGTH = 0x000000ff */
}

SECTIONS
{
    .text :
    {
        . = 0x0000;
        __cart_start = .;
        __text_start = .;
        *(.text* .rodata* .eh_frame*)
        . = ALIGN(4);
        __text_end = .;
    } >ROM

/*
    .data (READONLY):
    {
        __data_start = .;
        *(.data* )
        . = ALIGN(4);
        __data_end = .;
    } >RAM AT>ROM
    __data_load = LOADADDR(.data);
*/

    .end :
    {
        FILL(0xFFFFFFFF);
        BYTE(0xFF);
        . = ALIGN(4096);
        __cart_end = .;
    } >ROM

/*
    .bss :
    {
        __bss_start = .;
        *(.bss);
        *(.bss.*);
        . = ALIGN(4);
        __bss_end = .;
    } > RAM
*/    
}
