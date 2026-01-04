#include "isa_rw.h"

/*-----------------------------------------------------------------------------------
 * helpers
 *---------------------------------------------------------------------------------*/
#define leas8(x)    ((x)^3)
#define leas16(x)   ((x)^2)


/*-----------------------------------------------------------------------------------
 *
 * ENDIAN_BE : Big endian
 *
 *---------------------------------------------------------------------------------*/
uint8_t _ISA_API inp_be(uint16l_t port) {
    return *((volatile uint8_t*)(isa.bus.iobase + port));
}

void _ISA_API outp_be(uint16l_t port, uint8l_t data) {
    *((volatile uint8_t*)(isa.bus.iobase + port)) = data;
}

uint16_t _ISA_API inpw_be(uint16l_t port) {
    return *((volatile uint16_t*)(isa.bus.iobase + port));
}

void _ISA_API outpw_be(uint16l_t port, uint16l_t data) {
    *((volatile uint16_t*)(isa.bus.iobase + port)) = data;
}


/*-----------------------------------------------------------------------------------
 *
 * ENDIAN_LELS : Little endian lane swapped
 *
 * When lane-swapped Intel (little endian) byte ordering is used, the address needs no modifications.
 * 8-bit accesses work normal, 16 and 32 bit accesses the read or written data needs to be swapped
 * (ror.w #8,d0 for 16 bit, ror.w d0:swap d0:ror.w d0 for 32 bit).
 *
 *---------------------------------------------------------------------------------*/
uint16_t _ISA_API inpw_lels(uint16l_t port) {
    return isabios_swap16(*((volatile uint16_t*)(isa.bus.iobase + port)));
}

void _ISA_API outpw_lels(uint16l_t port, uint16l_t data) {
    *((volatile uint16_t*)(isa.bus.iobase + port)) = isabios_swap16(data);
}


/*-----------------------------------------------------------------------------------
 *
 * ENDIAN_LEAS : Little endian address swapped
 *
 * When address-swapped Intel (little endian) byte ordering is used,
 * 32 bit accesses work without modifications.
 * 16 bit accesses, the address needs to be XOR'd with a value of 2
 * 8-bit accesses the address is XOR'd with a value of 3.
 * The data read or written is in correct format.
 *
 *---------------------------------------------------------------------------------*/
uint8_t _ISA_API inp_leas(uint16l_t port) {
    return *((volatile uint8_t*)(leas8(isa.bus.iobase + port)));
}

void _ISA_API outp_leas(uint16l_t port, uint8l_t data) {
    *((volatile uint8_t*)(leas8(isa.bus.iobase + port))) = data;
}

uint16_t _ISA_API inpw_leas(uint16l_t port) {
    return *((volatile uint16_t*)(leas16(isa.bus.iobase + port)));
}

void _ISA_API outpw_leas(uint16l_t port, uint16l_t data) {
    *((volatile uint16_t*)(leas16(isa.bus.iobase + port))) = data;
}
