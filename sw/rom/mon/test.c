
#include "sys.h"

int test_ym_write(int size)
{
    static const unsigned char data[14*2] =
    {
        0, 0x34,
        1, 0,
        2, 0,
        3, 0,
        4, 0,
        5, 0,
        6, 0,
        7, 0xFE,
        8, 0x10,
        9, 0,
        10, 0,
        11, 0,
        12, 16,
        13, 9,
    };

    if (size == 1)
    {
        volatile unsigned char* ym2149 = (volatile unsigned char*)PADDR_YM;
        for (int i=0; i<14*2; i+=2)
        {
            ym2149[0] = data[i+0];
            ym2149[2] = data[i+1];
        }
    }
    else if (size == 2)
    {

    }
    else if (size == 4)
    {
        volatile unsigned int* ym2149 = (volatile unsigned int*)PADDR_YM;
        for (int i=0; i<14; i++)
        {
            unsigned int d0 = 0xFF & data[(i<<1)+0];
            unsigned int d1 = 0xFF & data[(i<<1)+1];
            unsigned int d = (d0 << 24) | (d1 << 8);
            *ym2149 = d;
        }
    }
    
    return 0;
}
