#include <stdbool.h>
#include <stdint.h>
#include <mint/cookie.h>
#include "isa_bios.h"

bool isabios_setup_panther(void) {
#if ISABIOS_STANDALONE
    uint32_t cookie;
    if (Getcookie(C__P2I, (long*)&cookie) == C_FOUND) {
        uint32_t* cardpth2 = (uint32_t*) (cookie+6);
        isa.bus.endian  = ISA_ENDIAN_LELS;
        isa.bus.iobase  = *cardpth2;
        cardpth2 += 2;
        isa.bus.membase = *cardpth2;
        return true;
    }
#endif
    return false;
}
