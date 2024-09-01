// -----------------------------------------------------------------
// this is the entry point for code that needs to run from ram
// -----------------------------------------------------------------
    .global _etext
    .global _edata
    .global _end

    .macro fptr func
        .global _\func
        dc.l _\func
    .endm

    .bss
    .balign 4
start_bss:

    .data
    .balign 4
start_data:


    // -----------------------------------------------------------------
    // ramcode_header_t
    // -----------------------------------------------------------------
    .text
start_text:
    dc.l start_text         // code+data start
    dc.l _edata             // code+data end
    dc.l start_bss          // bss start
    dc.l _end               // bss end

    // -----------------------------------------------------------------
    // ramcode_table_t
    // -----------------------------------------------------------------
    fptr test_1
    fptr test_2
    dc.l 0
    dc.l 0

