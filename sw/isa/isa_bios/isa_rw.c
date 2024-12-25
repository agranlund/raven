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

void _ISA_API inp_be_buf(uint16l_t port, uint8_t* data, int32_t count) {
    volatile uint8_t* src = (volatile uint8_t*)(isa.bus.iobase + port);
    while (count >= 8) {
        *data++ = *src; *data++ = *src; *data++ = *src; *data++ = *src;
        *data++ = *src; *data++ = *src; *data++ = *src; *data++ = *src;
        count -= 8;
    }
    while (count) {
        *data++ = *src;
        count--;
    }
}

void _ISA_API outp_be_buf(uint16l_t port, uint8_t* data, int32_t count) {
    volatile uint8_t* dst = (volatile uint8_t*)(isa.bus.iobase + port);
    while (count >= 8) {
        *dst = *data++; *dst = *data++; *dst = *data++; *dst = *data++;
        *dst = *data++; *dst = *data++; *dst = *data++; *dst = *data++;
        count -= 8;
    }
    while (count) {
        *dst = *data++;
        count--;
    }
}

void _ISA_API inpw_be_buf(uint16l_t port, uint16_t* data, int32_t count) {
    volatile uint16_t* src = (volatile uint16_t*)(isa.bus.iobase + port);
    while (count) {
        *data++ = *src; *data++ = *src; *data++ = *src; *data++ = *src;
        *data++ = *src; *data++ = *src; *data++ = *src; *data++ = *src;
        count -= 8;
    }
    while (count) {
        *data++ = *src;
        count--;
    }
}

void _ISA_API outpw_be_buf(uint16l_t port, uint16_t* data, int32_t count) {
    volatile uint16_t* dst = (volatile uint16_t*)(isa.bus.iobase + port);
    while (count >= 8) {
        *dst = *data++; *dst = *data++; *dst = *data++; *dst = *data++;
        *dst = *data++; *dst = *data++; *dst = *data++; *dst = *data++;
        count -= 8;
    }
    while (count) {
        *dst = *data++;
        count--;
    }
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
    return swap16(*((volatile uint16_t*)(isa.bus.iobase + port)));
}

void _ISA_API outpw_lels(uint16l_t port, uint16l_t data) {
    *((volatile uint16_t*)(isa.bus.iobase + port)) = swap16(data);
}

void _ISA_API inpw_lels_buf(uint16l_t port, uint16_t* data, int32_t count) {
    volatile uint16_t* src = (volatile uint16_t*)(isa.bus.iobase + port);
    while (count) {
        *data++ = swap16(*src); *data++ = swap16(*src); *data++ = swap16(*src); *data++ = swap16(*src);
        *data++ = swap16(*src); *data++ = swap16(*src); *data++ = swap16(*src); *data++ = swap16(*src);
        count -= 8;
    }
    while (count) {
        *data++ = swap16(*src);
        count--;
    }
}

void _ISA_API outpw_lels_buf(uint16l_t port, uint16_t* data, int32_t count) {
    volatile uint16_t* dst = (volatile uint16_t*)(isa.bus.iobase + port);
    while (count >= 8) {
        *dst = swap16(*data++); *dst = swap16(*data++); *dst = swap16(*data++); *dst = swap16(*data++);
        *dst = swap16(*data++); *dst = swap16(*data++); *dst = swap16(*data++); *dst = swap16(*data++);
        count -= 8;
    }
    while (count) {
        *dst = swap16(*data++);
        count--;
    }
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

void _ISA_API inp_leas_buf(uint16l_t port, uint8_t* data, int32_t count) {
    volatile uint8_t* src = (volatile uint8_t*)(leas8(isa.bus.iobase + port));
    while (count >= 8) {
        *data++ = *src; *data++ = *src; *data++ = *src; *data++ = *src;
        *data++ = *src; *data++ = *src; *data++ = *src; *data++ = *src;
        count -= 8;
    }
    while (count) {
        *data++ = *src;
        count--;
    }
}

void _ISA_API outp_leas_buf(uint16l_t port, uint8_t* data, int32_t count) {
    volatile uint8_t* dst = (volatile uint8_t*)(leas8(isa.bus.iobase + port));
    while (count >= 8) {
        *dst = *data++; *dst = *data++; *dst = *data++; *dst = *data++;
        *dst = *data++; *dst = *data++; *dst = *data++; *dst = *data++;
        count -= 8;
    }
    while (count) {
        *dst = *data++;
        count--;
    }
}

void _ISA_API inpw_leas_buf(uint16l_t port, uint16_t* data, int32_t count) {
    volatile uint16_t* src = (volatile uint16_t*)(leas16(isa.bus.iobase + port));
    while (count) {
        *data++ = *src; *data++ = *src; *data++ = *src; *data++ = *src;
        *data++ = *src; *data++ = *src; *data++ = *src; *data++ = *src;
        count -= 8;
    }
    while (count) {
        *data++ = *src;
        count--;
    }
}

void _ISA_API outpw_leas_buf(uint16l_t port, uint16_t* data, int32_t count) {
    volatile uint16_t* dst = (volatile uint16_t*)(leas16(isa.bus.iobase + port));
    while (count >= 8) {
        *dst = *data++; *dst = *data++; *dst = *data++; *dst = *data++;
        *dst = *data++; *dst = *data++; *dst = *data++; *dst = *data++;
        count -= 8;
    }
    while (count) {
        *dst = *data++;
        count--;
    }
}
