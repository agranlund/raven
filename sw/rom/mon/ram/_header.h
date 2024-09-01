#ifndef _RAMCODE_H_
#define _RAMCODE_H_

// function pointers for code that runs from ram
typedef struct
{
    int (*test_1)(void);
    int (*test_2)(void);
} ramcode_table_t;


// function table accessor
static inline ramcode_table_t* ramcode() {
    extern ramcode_table_t* kramcode;
    return kramcode;
}


// header for the binary
typedef struct
{
    uint32_t text_start;
    uint32_t text_end;
    uint32_t bss_start;
    uint32_t bss_end;
} ramcode_header_t;


#endif // _RAMCODE_H_

