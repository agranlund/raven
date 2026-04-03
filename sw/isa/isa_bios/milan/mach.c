#include <stdbool.h>
#include <stdint.h>
#include <mint/cookie.h>
#include "isa_bios.h"

bool isabios_setup_milan(void) {
#if ISABIOS_STANDALONE
    uint32_t cookie;
    if (Getcookie(C__MIL, (long*)&cookie) != C_FOUND) {
        return false;
    }
#endif

    #if 1
    isa.bus.endian  = ISA_ENDIAN_LELS;
    isa.bus.iobase  = 0xC0000000UL;
    #else        
    isa.bus.endian  = ISA_ENDIAN_LEAS;
    isa.bus.iobase  = 0x80000000;
    #endif
    return true;
}
