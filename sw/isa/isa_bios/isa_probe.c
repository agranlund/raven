#include "isa_core.h"

//#define DEBUG

#ifdef DEBUG
#define dbg_printf(...)    printf(__VA_ARGS__);
#else
#define dbg_printf(...)    { }
#endif


uint32 isa_probe() {
    return 0;
}
