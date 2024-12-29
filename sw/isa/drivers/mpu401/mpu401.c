/*-------------------------------------------------------------------------------
 * MPU401 uart mode driver for Atari bios/xbios
 * (c)2024 Anders Granlund
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
unsigned long _StkSize = 4096;
extern unsigned long _PgmSize;

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mint/cookie.h"
#include "mint/osbind.h"
#include "isa.h"

extern uint32_t xbios_old;
extern uint32_t xbios_new;
extern uint32_t new_bconin3;
extern uint32_t new_bconout3;
extern uint32_t new_bconstat3;
extern uint32_t new_bcostat3;


/* ------------------------------------------------------------------- */
#define MPU401_MODE_OFF         0
#define MPU401_MODE_IN          (1 << 0)
#define MPU401_MODE_OUT         (1 << 1)
#define MPU401_MODE_INOUT       (MPU401_MODE_IN | MPU401_MODE_OUT)

#define MPU401_REG_DATA         0
#define MPU401_REG_COMMAND      1
#define MPU401_REG_STATUS       1

#define MPU401_STAT_READ        0x80
#define MPU401_STAT_WRITE       0x40

#define MPU401_DEFAULT_PORT     0x330
#define MPU401_DEFAULT_MODE     MPU401_MODE_OUT

static uint16_t mpu401_port = 0;


/* ------------------------------------------------------------------- */

static void nop(void) 0x4E71;
#define get200hz() *((volatile uint32_t*)0x4ba)

static void mpu401_Delay(uint32_t micros)
{
    /* calibration */
    static uint32_t loops_count = 0;
    if (loops_count == 0) {
        uint32_t tick_start = get200hz();
        uint32_t tick_end = tick_start;
        do {
            nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
            nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
            tick_end = get200hz();
            loops_count++;
            if (loops_count > 1000000UL) {
                loops_count = 0xffffffffUL;
                break;
            }
        } while ((tick_end - tick_start) <= 25UL);
    }
    if ((micros < 1000) && (loops_count != 0xffffffffUL)) {
        /* microseconds delay using calibration data */
        uint32_t i; uint32_t loops = 1 + ((2 * 4 * loops_count * micros) / (1000 * 1000UL));
        for (i=0; i<=loops; i++) {
            nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
            nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
        }
        return;
    } else {
        /* millisecond delay using 200hz counter */
        uint32_t ticks = micros / 5000;
        uint32_t start  = get200hz();
        ticks = (ticks < 1) ? 1 : ticks;
        while (1) {
            volatile uint32_t now = get200hz();
            if (now < start) {
                start = now;
            } else if ((now - start) >= ticks) {
                break;
            }
        }
    }
}

/* ------------------------------------------------------------------- */

static bool mpu401_CheckStatus(uint8_t stat) {
    return ((inp(mpu401_port + MPU401_REG_STATUS) & stat) == 0) ? true : false;
}

static bool mpu401_WaitStatus(uint8_t stat) {
    int32_t timeout = 100000L;
    while(timeout) {
        if (mpu401_CheckStatus(stat)) {
            return true;
        }
        mpu401_Delay(5);
        timeout -= 1L;
    }
    return false;
}

static bool mpu401_WaitAck(void) {
    int32_t timeout = 100L;
    while(timeout) {
        if (!mpu401_WaitStatus(MPU401_STAT_READ)) {
            return false;
        }
        if (inp(mpu401_port + MPU401_REG_DATA) == 0xFE) {
            return true;
        }
        timeout -= 1L;
    }
    return false;
}

static bool mpu401_WriteCommand(uint8_t data) {
    if (mpu401_port && mpu401_WaitStatus(MPU401_STAT_WRITE)) {
        outp(mpu401_port + MPU401_REG_COMMAND, data);
        return true;
    }
    return false;
}

static void mpu401_WriteData(uint8_t data) {
    if (mpu401_port && mpu401_WaitStatus(MPU401_STAT_WRITE)) {
        outp(mpu401_port + MPU401_REG_DATA, data);
    }
}

static uint8_t mpu401_ReadData(void) {
    if (mpu401_port && mpu401_WaitStatus(MPU401_STAT_READ)) {
        return inp(mpu401_port + MPU401_REG_DATA);
    }
    return 0x00;
}


/* ------------------------------------------------------------------- */
static bool init_midi(uint16_t port) {

    mpu401_port = port;
    if (mpu401_port == 0) {
        /* todo: ask pnp for "PNPB006" */
        mpu401_port = MPU401_DEFAULT_PORT;
    }

    /* calibrate delays */
    mpu401_Delay(1);

    /* reset */
    if (!mpu401_WriteCommand(0xff)) {
        return false;
    }

    /* wait for ack */
    if (!mpu401_WaitAck()) {
        return false;
    }

    /* set uart mode */
    if (!mpu401_WriteCommand(0x3f)) {
        return false;
    }

    return true;
}


/* ------------------------------------------------------------------- */
int32_t midi_bcostat(void) {
    /* return -1 if characters can be written, 0 if not */
    return mpu401_CheckStatus(MPU401_STAT_WRITE) ? -1L : 0L;
}

int32_t midi_bconstat(void) {
    /* return -1 when there are characters avaialble, 0 if not */
    return mpu401_CheckStatus(MPU401_STAT_READ) ? -1L : 0L;
}

void midi_bconout(uint8_t data) {
    mpu401_WriteData(data);
}

int32_t midi_bconin(void) {
    /* top 3 bytes are supposed to be a timestamp */
    uint32_t data = mpu401_ReadData();
    return data;
}



/* ------------------------------------------------------------------- */
void install_bios(uint16_t mode) {
    volatile uint32_t* ptr_xbios      = (volatile uint32_t*)(0xb8UL);
    volatile uint32_t* ptr_bconin3    = (volatile uint32_t*)(0x53eUL + (3 * 4));
    volatile uint32_t* ptr_bconout3   = (volatile uint32_t*)(0x57eUL + (3 * 4));
    volatile uint32_t* ptr_bconstat3  = (volatile uint32_t*)(0x51eUL + (3 * 4));
    volatile uint32_t* ptr_bcostat3   = (volatile uint32_t*)(0x55eUL + (4 * 4));  /* midi and keyb are swapped in bcostat */

    /* replace midi related bios vectors */
    if (mode & MPU401_MODE_IN) {
        *ptr_bconin3   = (uint32_t)&new_bconin3;
        *ptr_bconstat3 = (uint32_t)&new_bconstat3;
    }
    if (mode & MPU401_MODE_OUT) {
        *ptr_bconout3 = (uint32_t)&new_bconout3;
        *ptr_bcostat3 = (uint32_t)&new_bcostat3;
    }

    /* replace xbios */
    if (mode & MPU401_MODE_OUT) {
        xbios_old = *ptr_xbios;
        *ptr_xbios = (uint32_t)&xbios_new;
    }
}


/* ------------------------------------------------------------------- */
long super_main(int args, char** argv) {
    uint16_t i;
    uint16_t port = 0;
    uint16_t mode = MPU401_DEFAULT_MODE;
    const char* modestr[4] = { "off", "in", "out", "in/out" };

    for (i=1; i<args; i++) {
        if (strcmp(argv[i], "out") == 0) {
            mode = MPU401_MODE_OUT;
        } else if (strcmp(argv[i], "in") == 0) {
            mode = MPU401_MODE_IN;
        } else if (strcmp(argv[i], "inout") == 0) {
            mode = MPU401_MODE_INOUT;
        } else if (*argv[i] >= '0' && *argv[i] <= '9') {
            long l = strtol(argv[i], NULL, 16);
            if ((l > 0x100L) && (l < 0xffffL)) {
                port = (uint16_t)l;
            }
        }
    }

    /* initialize isa interface*/
    if (!isa_init()) {
        printf("MPU401 isa interface not found\n\n");
        return -1L;
    }

    /* initialize midi interface */
    if (!init_midi(port)) {
        printf("MPU401 not found on port %03x\n\n", mpu401_port);
        return -2L;
    }

    /* install bios + xbios overrides */
    install_bios(mode);

    printf("\nMPU401 midi %s on port %03x\n\n", modestr[mode], mpu401_port);
    return 0;
}


/* ------------------------------------------------------------------- */
static int _super_args;
static char** _super_argv;
long super_trampoline() {
    return super_main(_super_args, _super_argv);
}
int main(int _user_args, char** _user_argv) {
    _super_args = _user_args; _super_argv = _user_argv;
    if (Supexec(super_trampoline) == 0) {
        Ptermres(_PgmSize, 0);
    }
    return 0;
}

