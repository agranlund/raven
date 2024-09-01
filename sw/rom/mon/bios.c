#include "sys.h"
#include "raven.h"
#include "hw/uart.h"
#include "hw/ikbd.h"
#include "hw/midi.h"
#include "hw/i2c.h"
#include "hw/rtc.h"


void dbg_GPO(int num, int val)
{
    switch (num)
    {
        case 0:     // uart1:powerled
        case 1:     // uart1:TP301
            ikbd_GPO(num, val);
            break;
    }
}


raven_t ravenBios =
{
    C_RAVN,     // magic
    VERSION,    // rom version

    dbg_GPO,

    i2c_Aquire,
    i2c_Release,
    i2c_Start,
    i2c_Stop,
    i2c_Read,
    i2c_Write,

    rtc_SetDateTime,
    rtc_GetDateTime,
    rtc_GetRam,
    rtc_SetRam

};

