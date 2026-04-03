#include <stdbool.h>
#include <stdint.h>
#include <mint/cookie.h>
#include "isa_bios.h"

bool isabios_setup_hades(void) {
#if ISABIOS_STANDALONE    
    uint32_t cookie;
    if (Getcookie(C_hade, (long*)&cookie) != C_FOUND) {
        return false;
    }
#endif
    isa.bus.endian  = ISA_ENDIAN_LELS;
    isa.bus.iobase  = 0xFFF30000UL;
    isa.bus.membase = 0xFF000000UL;
    return true;
}
