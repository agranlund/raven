/*-------------------------------------------------------------------------------
 * rvsnd
 * (c)2025 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/
#include "sys.h"
#include <stdbool.h>
#include <stdint.h>

int32_t rvsnd_testcommand(int16_t a1, int32_t a2) {
    dprintf("rvsnd00 : %04x %08lx\n", a1, a2);
    return 888;
}


/*------------------------------------------------------------------------------*/
/* $8F : xbios rvsnd dispatch
/*------------------------------------------------------------------------------*/
volatile int blah = 1;
int32_t xb_rvsnd(uint16_t* args) {
    uint16_t op = *args; args += 3;
    dprintf("op = %04x\n", op);
    switch(op){
        case 1: return rvsnd_testcommand(
            *((int16_t*)(args+0)),
            *((int32_t*)(args+1)));
        default: return 777;
    }
}
