#include "isa_rw.h"

//-----------------------------------------------------------------------------------
// helpers
//-----------------------------------------------------------------------------------
#define leas8(x)    ((x)^3)
#define leas16(x)   ((x)^2)


//-----------------------------------------------------------------------------------
//
// ENDIAN_BE : Big endian
//
//-----------------------------------------------------------------------------------
uint8 inp_be(uint16 port) {
    return *((volatile uint8*)(isa.bus.iobase + port));
}

void outp_be(uint16 port, uint8 data) {
    *((volatile uint8*)(isa.bus.iobase + port)) = data;
}

uint16 inpw_be(uint16 port) {
    return *((volatile uint16*)(isa.bus.iobase + port));
}

void outpw_be(uint16 port, uint16 data) {
    *((volatile uint16*)(isa.bus.iobase + port)) = data;
}

void inp_be_buf(uint16 port, uint8* data, int count) {
    volatile uint8* src = (volatile uint8*)(isa.bus.iobase + port);
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

void outp_be_buf(uint16 port, uint8* data, int count) {
    volatile uint8* dst = (volatile uint8*)(isa.bus.iobase + port);
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

void inpw_be_buf(uint16 port, uint16* data, int count) {
    volatile uint16* src = (volatile uint16*)(isa.bus.iobase + port);
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

void outpw_be_buf(uint16 port, uint16* data, int count) {
    volatile uint16* dst = (volatile uint16*)(isa.bus.iobase + port);
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



//-----------------------------------------------------------------------------------
//
// ENDIAN_LELS : Little endian lane swapped
//
// When lane-swapped Intel (little endian) byte ordering is used, the address needs no modifications.
// 8-bit accesses work normal, 16 and 32 bit accesses the read or written data needs to be swapped
// (ror.w #8,d0 for 16 bit, ror.w d0:swap d0:ror.w d0 for 32 bit).
//
//-----------------------------------------------------------------------------------
uint16 inpw_lels(uint16 port) {
    return swap16(*((volatile uint16*)(isa.bus.iobase + port)));
}

void outpw_lels(uint16 port, uint16 data) {
    *((volatile uint16*)(isa.bus.iobase + port)) = swap16(data);
}

void inpw_lels_buf(uint16 port, uint16* data, int count) {
    volatile uint16* src = (volatile uint16*)(isa.bus.iobase + port);
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

void outpw_lels_buf(uint16 port, uint16* data, int count) {
    volatile uint16* dst = (volatile uint16*)(isa.bus.iobase + port);
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


//-----------------------------------------------------------------------------------
//
// ENDIAN_LEAS : Little endian address swapped
//
// When address-swapped Intel (little endian) byte ordering is used,
// 32 bit accesses work without modifications.
// 16 bit accesses, the address needs to be XOR'd with a value of 2
// 8-bit accesses the address is XOR'd with a value of 3.
// The data read or written is in correct format.
//
//-----------------------------------------------------------------------------------
uint8 inp_leas(uint16 port) {
    return *((volatile uint8*)(leas8(isa.bus.iobase + port)));
}

void outp_leas(uint16 port, uint8 data) {
    *((volatile uint8*)(leas8(isa.bus.iobase + port))) = data;
}

uint16 inpw_leas(uint16 port) {
    return *((volatile uint16*)(leas16(isa.bus.iobase + port)));
}

void outpw_leas(uint16 port, uint16 data) {
    *((volatile uint16*)(leas16(isa.bus.iobase + port))) = data;
}

void inp_leas_buf(uint16 port, uint8* data, int count) {
    volatile uint8* src = (volatile uint8*)(leas8(isa.bus.iobase + port));
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

void outp_leas_buf(uint16 port, uint8* data, int count) {
    volatile uint8* dst = (volatile uint8*)(leas8(isa.bus.iobase + port));
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

void inpw_leas_buf(uint16 port, uint16* data, int count) {
    volatile uint16* src = (volatile uint16*)(leas16(isa.bus.iobase + port));
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

void outpw_leas_buf(uint16 port, uint16* data, int count) {
    volatile uint16* dst = (volatile uint16*)(leas16(isa.bus.iobase + port));
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
