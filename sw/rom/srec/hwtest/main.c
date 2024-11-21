/*
 * Raven example srec program.
 */

#include <lib.h>
#include "sys.h"

void delay_long()
{
    puts("start");
    /*for (uint32_t d1 = 0x01000000UL; d1 !=0; d1--)*/ {
        for (uint32_t d2 = 0x01000000; d2 !=0; d2--) {
            nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
        }
    }
    puts("done");
}

void main(void)
{
    puts("test");
    delay_long();
}
