#include "sys.h"
#include "bios.h"
#include "hw/uart.h"
#include "hw/ikbd.h"
#include "hw/midi.h"

void debugOut(int num, int val)
{
    switch (num)
    {
        case 0:     // uart1:powerled
        case 1:     // uart1:TP301
            ikbd_GPO(num, val);
            break;
    }
}


biosHeader_t biosHeader =
{
    BIOS_MAGIC,             // magic
    VERSION,                // rom version

    debugOut
};

