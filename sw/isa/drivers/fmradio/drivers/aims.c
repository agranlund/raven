/*-------------------------------------------------------------------------------
 * Aimslab FMRADIO driver
  * 
 * Based on Linux AimsLab RadioTrack (aka RadioVeveal) driver
 * Copyright 1997 M. Kirkwood
  * Converted to V4L2 API by Mauro Carvalho Chehab <mchehab@infradead.org>
 * Converted to new API by Alan Cox <alan@lxorguk.ukuu.org.uk>
 * Various bugfixes and enhancements by Russell Kroll <rkroll@exploits.org
 * 
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
#include <mint/osbind.h>
#include "fmradio.h"
#include "isa.h"

#define AIMS_BIT_TUN_CE		(1 << 0)
#define AIMS_BIT_TUN_CLK	(1 << 1)
#define AIMS_BIT_TUN_DATA	(1 << 2)
#define AIMS_BIT_VOL_CE		(1 << 3)
#define AIMS_BIT_TUN_STRQ	(1 << 4)
#define AIMS_BIT_VOL_UP		(1 << 6)	/* active low */
#define AIMS_BIT_VOL_DN		(1 << 7)	/* active low */

static uint16_t port = 0x30f;
static uint8_t cur_vol = 0x3f;
static uint8_t cur_mute = false;

static void send(uint8_t data)
{
    uint8_t b = AIMS_BIT_VOL_DN|AIMS_BIT_VOL_UP|AIMS_BIT_TUN_STRQ|AIMS_BIT_TUN_CE;
    if (data) { b |= AIMS_BIT_TUN_DATA; }
    if ((cur_vol > 0) && !cur_mute) { b |= AIMS_BIT_VOL_CE; }
    outp(port, b);
    isa_delay(1000);
    outp(port, b | AIMS_BIT_TUN_CLK);
    isa_delay(1000);
}


static int setfreq(uint32_t freq)
{
	int i;

	/* adapted from radio-aztech.c */
	/* now uses VIDEO_TUNER_LOW for fine tuning */

	freq += 171200;			    /* Add 10.7 MHz IF 		*/
	freq /= 800;			    /* Convert to 50 kHz units	*/
    send(0);
	send(0);		            /*  0: LSB of frequency		*/
	for (i = 0; i < 13; i++) {	/*   : frequency bits (1-13)	*/
		if (freq & (1 << i)) {
            send(1);
        } else {
            send(0);
        }
    }
	send(0);	/* 14: test bit - always 0    */
	send(0);	/* 15: test bit - always 0    */
	send(0);	/* 16: band data 0 - always 0 */
	send(0);	/* 17: band data 1 - always 0 */
	send(0);	/* 18: band data 2 - always 0 */
	send(0);	/* 19: time base - always 0   */
	send(0);	/* 20: spacing (0 = 25 kHz)   */
	send(1);	/* 21: spacing (1 = 25 kHz)   */
	send(0);	/* 22: spacing (0 = 25 kHz)   */
	send(1);	/* 23: AM/FM (FM = 1, always) */

	if ((cur_vol == 0) || cur_mute) {
		outp(port, 0xd0);	/* volume steady + sigstr */
    } else {
		outp(port, 0xd8);	/* volume steady + sigstr + on */
    }
}

static bool drv_detect(void) {
    return true;
}

static void drv_vol(int8_t vol) {
    if (vol > 0) {
        outp(port, 0x98);   /* volume up */
        isa_delay(10000UL * vol);
        outp(port, 0xd8);
    } else if (vol < 0) {
        outp(port, 0x58);
        isa_delay(10000UL * -vol);
        outp(port, 0xd8);
    }
}

static void drv_freq(uint32_t freq) {
    if (freq == 0) {
        outp(port, 0xd0);
    } else {
        setfreq((freq * 125) >> 1);
    }
}

fmdriver_t drv_aims = {
    "aims",
    drv_detect,
    drv_freq,
    drv_vol,
};
