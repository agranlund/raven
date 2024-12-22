#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

static int16_t cursrate = 20;
static uint8_t* rambase = NULL;

uint32_t xbios_dispatch(uint16_t opcode, uint32_t pd)
{
	uint32_t retval = opcode;
    uint32_t sp = m68k_get_reg(NULL, M68K_REG_SP);

    //    printf("xbios %02x\n", opcode);

    switch(opcode)
    {
        case 0x0010:  /* Keytabl() */
        {
       //     printf("ignored XBIOS call (%04x)\n", opcode);
        }
            break;

        case 0x0004: /* Getrez() */
        {
            retval = 2; /* always fake STHIGH */
        }
            break;

        case 0x0015: /* int16_t Cursconf( int16_t func, int16_t rate ); */
        {
            int16_t func = READ_LONG(rambase, sp + 2);
            int16_t rate = READ_LONG(rambase, sp + 4);

            switch(func)
            {
                case 4:
                {
                    cursrate = rate;
                }
                    break;

                case 5:
                {
                    retval = cursrate;
                }
                    break;

                default:
                {

                }
                    break;
            }
        }
            break;

        case 0x0026: /* Supexec() */
        {
            uint32_t call = READ_LONG(rambase, sp + 2);

            sp -= 4;
            WRITE_LONG(rambase, sp, m68k_get_reg(NULL, M68K_REG_PC));
            m68k_set_reg(M68K_REG_SP, sp);

            m68k_set_reg(M68K_REG_PC, call);

           // printf("Supexec(%08x)\r\n", call);
            return true;
        }
            break;

        default:
        {
            printf("unknown XBIOS call (%04x)\n", opcode);
            exit(0);
        }
            break;
    }

    return retval;
}

void xbios_init(uint8_t* ram, uint32_t ramsize)
{
	rambase = ram;
}
