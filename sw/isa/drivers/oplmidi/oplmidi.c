/*-------------------------------------------------------------------------------
 * OPL midi driver for Atari bios/xbios
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
extern uint32_t new_bconout3;
extern uint32_t new_bcostat3;

/* ------------------------------------------------------------------- */
extern bool OPLWIN_MIDI_init(bool opl3mode);
extern void OPLWIN_MIDI_write(uint32_t data);
extern void OPLWIN_MIDI_reset();


/* ------------------------------------------------------------------- */
#define MIDI_MODE_OFF           0
#define MIDI_MODE_IN            (1 << 0)
#define MIDI_MODE_OUT           (1 << 1)
#define MIDI_MODE_INOUT         (MIDI_MODE_IN | MIDI_MODE_OUT)

#define MIDI_DEFAULT_MODE       MIDI_MODE_OUT
#define OPL_DEFAULT_PORT        0x388

static uint16_t opl_port = 0;
static uint16_t opl_chip = 0;
static uint16_t opl_delay_idx = 8;      /* opl2 index write delay */
static uint16_t opl_delay_val = 32;     /* opl2 value write delay */

/* ------------------------------------------------------------------- */

static void adlib_delay( unsigned long usec ) {
    isa_delay(usec);
}

void adlib_write(uint16_t idx, uint8_t val) {
    if (opl_chip != 0) {
        if (idx >= 0x100) {
            /* opl3 second register set */
            outp(opl_port + 2, (idx - 0x100));
            adlib_delay(opl_delay_idx);
            outp(opl_port + 3, val);
            adlib_delay(opl_delay_val);
        } else {
            /* opl2 or opl3 first register set */
            outp(opl_port + 0, idx);
            adlib_delay(opl_delay_idx);
            outp(opl_port + 1, val);
            adlib_delay(opl_delay_val);

        }
    }
}


/* ------------------------------------------------------------------- */
static bool init_midi(uint16_t port) {
    uint8_t result1, result2;

    opl_port = port;
    if (opl_port == 0) {
        opl_port = OPL_DEFAULT_PORT;
    }

    /* calibrate delays */
    isa_delay(1);

    /* detect opl version */
    opl_chip = 2;               /* assume opl2 for now */
    adlib_write(0x04, 0x60);    /* reset timer1+2 and irq */
    adlib_write(0x04, 0x80);
    result1 = inp(opl_port);
    adlib_write(0x02, 0xff);    /* set timer1 */
    adlib_write(0x04, 0x21);    /* start timer1 */
    adlib_delay(2000);
    result2 = inp(opl_port);
    adlib_write(0x04, 0x60);    /* reset timer1+2 and irq */
    adlib_write(0x04, 0x80);

    /* now work out if we actually have an opl chip */
    opl_chip = 0;
    if (((result1 & 0xe0) == 0x00) && ((result2 & 0xe0) == 0xc0)) {
        opl_chip = 2;
        if ((result2 & 0x06) == 0x00) {
            opl_chip = 3;
            opl_delay_idx = 4;      /* less delay required on opl3 */
            opl_delay_val = 4;
        }
    }
    if (opl_chip == 0) {
        return false;
    }

    return OPLWIN_MIDI_init((opl_chip == 3) ? true : false);
}


/* ------------------------------------------------------------------- */
int32_t midi_bcostat(void) {
    /* return -1 if characters can be written, 0 if not */
    return -1;
}

#define MSG_NONE        0
#define MSG_1PARAM      1
#define MSG_2PARAM      2
#define MSG_RUN         3
#define MSG_SYSEX       4
#define MSG_META        5

void midi_bconout(uint8_t data) {
    static uint8_t msg_type = MSG_NONE;
    static uint8_t msg_prev_type = MSG_NONE;
    static uint8_t msg_pos = 0;
    static uint8_t msg_data[4];

    if (msg_type == MSG_NONE)
    {
        msg_pos = 0;
        if ((data >= 0x80 && data <= 0xBF) || (data >= 0xE0 && data <= 0xEF) || (data == 0xF2)) {
            msg_type = MSG_2PARAM;
            msg_data[msg_pos++] = data;
        }
        else if ((data >= 0xC0 && data <= 0xDF) || (data == 0xF1) || (data == 0xF3) || (data == 0xF9)) {
            msg_type = MSG_1PARAM;
            msg_data[msg_pos++] = data;
        }
        else if (data <= 0x7F)
        {
            msg_type = msg_prev_type;
            msg_data[1] = data;
            msg_pos = 2;
        }
        else if (data == 0xF0)
        {
            msg_type = MSG_SYSEX;
        }
        else if (data == 0xFF)
        {
            msg_type = MSG_META;
        }
    }
    else if (msg_type == MSG_1PARAM)
    {
        msg_data[msg_pos++] = data;
        if (msg_pos > 1) {
            uint32_t d = ((uint32_t)msg_data[0]) | (((uint32_t)msg_data[1]) << 8);
            OPLWIN_MIDI_write(d);
            msg_prev_type = msg_type;
            msg_type = MSG_NONE;
        }
    }
    else if (msg_type == MSG_2PARAM)
    {
        msg_data[msg_pos++] = data;
        if (msg_pos > 2) {
            uint32_t d = ((uint32_t)msg_data[0]) | (((uint32_t)msg_data[1]) << 8) | (((uint32_t)msg_data[2]) << 16);
            OPLWIN_MIDI_write(d);
            msg_prev_type = msg_type;
            msg_type = MSG_NONE;
        }
    }
    else if (msg_type == MSG_SYSEX)
    {
        msg_prev_type = MSG_NONE;
        if (data == 0xF7) {
            msg_type = MSG_NONE;
        }
    }
    else if (msg_type == MSG_META)
    {
        static int32_t varlen = 0;
        if (msg_pos == 0) {
            msg_data[msg_pos++] = data;
            varlen = 0;
        } else if (msg_pos == 1) {
            varlen = (varlen << 7) + (data & 0x7f);
            if ((data & 0x80) == 0) {
                msg_pos = 2;
            }
        } else {
            varlen--;
            if (varlen <= 0) {
                msg_type = MSG_NONE;
            }
        }
    }
}


/* ------------------------------------------------------------------- */
void install_bios(void) {
    volatile uint32_t* ptr_xbios      = (volatile uint32_t*)(0xb8UL);
    volatile uint32_t* ptr_bconout3   = (volatile uint32_t*)(0x57eUL + (3 * 4));
    volatile uint32_t* ptr_bcostat3   = (volatile uint32_t*)(0x55eUL + (4 * 4));  /* midi and keyb are swapped in bcostat */

    /* replace midi related bios vectors */
    *ptr_bconout3 = (uint32_t)&new_bconout3;
    *ptr_bcostat3 = (uint32_t)&new_bcostat3;
    xbios_old = *ptr_xbios;
    *ptr_xbios = (uint32_t)&xbios_new;
}


/* ------------------------------------------------------------------- */
long super_main(int args, char** argv) {
    uint16_t i;
    uint16_t port = 0;

    for (i=1; i<args; i++) {
         if (*argv[i] >= '0' && *argv[i] <= '9') {
            long l = strtol(argv[i], NULL, 16);
            if ((l > 0x100L) && (l < 0xffffL)) {
                port = (uint16_t)l;
            }
        }
    }

    /* initialize isa interface*/
    if (!isa_init()) {
        printf("ISA bus not found\n\n");
        return -1L;
    }

    /* initialize midi interface */
    if (!init_midi(port)) {
        printf("OPL not found on port %03x\n\n", opl_port);
        return -2L;
    }

    /* install bios + xbios overrides */
    install_bios();

    printf("\nOPL%d midi out on port %03x\n\n", opl_chip, opl_port);
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

