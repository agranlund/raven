#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

#define C__MCH 0x5F4D4348L     /* Machine Type */
#define C__CPU 0x5F435055L     /* Central Processor Unit Type */
#define C__FPU 0x5F465055L     /* Floating Point Unit Type */

static uint8_t* rambase = NULL;

uint32_t bios_dispatch(uint16_t opcode, uint32_t pd)
{
	uint32_t retval = opcode;

    //printf("bios %02x\n", opcode);

    switch(opcode)
    {
        case 0x0003: /* VOID Bconout ( int16_t dev, int16_t c ); */
        {
            int16_t dev = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int16_t c   = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);

            switch(dev)
            {

                case 1: /* AUX: */
                case 2: /* CON: */
                {
                    putchar((char)c);
                }
                    break;

                case 0: /* PRN: */
                case 3: /* MIDI */
                case 4: /* KBD  */
                case 5: /* SCRN */
                case 6: /* MODEM-1 */
                case 7: /* MODEM-2 */
                case 8: /* MODEM-3 */
                case 9: /* MODEM-4 */
                default:
                {
                    /* ignore */
                }
                    break;
            }
        }
            break;

        case 0x0005: /* int32_t Setexc ( int16_t number, VOID (*vec)() ); */
        {
            int16_t number = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            uint32_t value = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);

            uint32_t prev  = READ_LONG(rambase, number << 2);

            if(value != 0xffffffff)
            {
                WRITE_LONG(rambase, number << 2, value);
            }

            retval = prev;
        }
            break;

        case 0x000B: /* int32_t Kbshift ( int16_t mode ); */
        {
        	retval = 0;  /* No modifier keys emulated */
        }
            break;

        default:
        {
            printf("unknown BIOS call (%04x)\n", opcode);
            exit(0);
        }
            break;
    }

    return retval;
}

void bios_init(uint8_t* ram, uint32_t ramsize)
{
    uintptr_t cjar = 0x1000UL;
	rambase = ram;

    WRITE_LONG(rambase, 0x5A0UL, cjar);

    WRITE_LONG(rambase, cjar + 0, C__MCH);
    WRITE_LONG(rambase, cjar + 4, 0x20000);
    cjar += 8;

    WRITE_LONG(rambase, cjar + 0, C__CPU);
    WRITE_LONG(rambase, cjar + 4, 30);
    cjar += 8;

    WRITE_LONG(rambase, cjar + 0, C__FPU);
    WRITE_LONG(rambase, cjar + 4, 0x30000);
    cjar += 8;

    WRITE_LONG(rambase, cjar + 0, 0);
    WRITE_LONG(rambase, cjar + 4, 0);
    cjar += 8;
}
