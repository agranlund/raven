#ifndef _BIOS_H_
#define _BIOS_H_
#include "sys.h"

#define BIOS_MAGIC  0x5241564E

typedef struct
{
    uint32          magic;
    uint32          version;

    void(*dbgOut)(int num, int val);

    // ikbd
    // midi
    // uart
    // rtc
    // i2c

} biosHeader_t;

extern biosHeader_t biosHeader;

#endif // _BIOS_H_


