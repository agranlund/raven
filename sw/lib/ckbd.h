/*-------------------------------------------------------------------------------
 * Eiffel / Ckbd defines and helpers
 * (c)2026 Anders Granlund
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
#ifndef _CKBD_EIFFEL_H_
#define _CKBD_EIFFEL_H_

#include <stdint.h>

/*--------------------------------------------------------------------------------*/
#ifndef C_Eiff
#define C_Eiff 0x45696666UL
#endif
#ifndef C_Temp
#define C_Temp 0x54656D70UL
#endif


/*--------------------------------------------------------------------------------*/
typedef struct {
	/* IKBD_EIFFEL_STATUS_TEMP */
	uint8_t temp0_deg;
	uint8_t temp0_adc;
	uint8_t temp0_fan;
	uint8_t temp0_fan_hi;
	uint8_t temp0_fan_lo;
	uint8_t temp0_res;
	/* IKBD_CKBD_STATUS_TEMP */
	uint8_t temp1_adc;
	uint8_t temp1_deg;
	uint8_t temp1_fan;
	uint8_t temp1_fan_hi;
	uint8_t temp1_fan_lo;
	uint8_t temp1_res;
	/* IKBD_CKBD_STATUS_VERSION */	
	uint8_t board_rev;
	uint8_t board_flags;
	uint32_t ckbd_version;
	/* IKBD_CKBD_STATUS_SETTING */
	uint8_t cfg_msgid;
	uint8_t cfg_index;
	uint8_t cfg_data[4];
} ckbd_cookie_temp_t;

typedef struct {
	uint16_t header;
	ckbd_cookie_temp_t temp;
} ckbd_cookie_eiff_t;


/*--------------------------------------------------------------------------------*/
#define CKBD_CMD_EIFFEL_GET_TEMP    0x03
#define CKBD_CMD_EIFFEL_PROG_TEMP   0x04    /* <index> <code> */
#define CKBD_CMD_EIFFEL_PROG_KEY    0x05    /* <index> <code> */
#define CKBD_CMD_EIFFEL_PROG_MOUSE  0x06    /* <index> <code> */
#define CKBD_CMD_EIFFEL_LCD         0x23    /* <len> <data...> */

#define CKBD_CMD_CKBD_READ_SETTING  0x2A
#define CKBD_CMD_CKBD_PROG_SETTING  0x2B    /* <settings...> */
#define CKBD_CMD_CKBD_PROG_FIRMWARE 0x2C
#define CKBD_CMD_CKBD_ARGB			0x2D	/* <idx> <num> <siz> <data> */
#define CKBD_CMD_CKBD_RESET         0x2E
#define CKBD_CMD_CKBD_POWER         0x2F


/*--------------------------------------------------------------------------------*/
#define CKBD_CFG_MAGIC				0x00
#define CKBD_CFG_VERSION			0x04
#define CKBD_CFG_CHANGED			0x08
#define CKBD_CFG_PS2MOUSE_SCALE		0x10
#define CKBD_CFG_PS2WHEEL_SCALE		0x12
#define CKBD_CFG_USBMOUSE_SCALE		0x14
#define CKBD_CFG_USBWHEEL_SCALE		0x16
#define CKBD_CFG_OLDMOUSE_SCALE		0x14
#define CKBD_CFG_OLDMOUSE_AMIGA		0x22

#define CKBD_CFG_TEMP_SHUTDOWN		0x23
#define CKBD_CFG_FAN0_MODE			0x24
#define CKBD_CFG_FAN1_MODE			0x25
#define CKBD_CFG_FAN0_MIN_SPEED		0x28
#define CKBD_CFG_FAN0_MAX_SPEED		0x29
#define CKBD_CFG_FAN1_MIN_SPEED		0x2A
#define CKBD_CFG_FAN1_MAX_SPEED		0x2B
#define CKBD_CFG_RGB_I				0x2C
#define CKBD_CFG_RGB_R				0x2D
#define CKBD_CFG_RGB_G				0x2E
#define CKBD_CFG_RGB_B				0x2F

#define CKBD_CFG_EIFFEL_MOUSE_CFG	0x30
#define CKBD_CFG_EIFFEL_TEMP0_HI	0x38
#define CKBD_CFG_EIFFEL_TEMP0_LO	0x39
#define CKBD_CFG_EIFFEL_TEMP1_HI	0x52
#define CKBD_CFG_EIFFEL_TEMP1_LO	0x53
#define CKBD_CFG_EIFFEL_KEYMAP		0x6C



/*--------------------------------------------------------------------------------*/
static long ckbd_get200hz(void) {
	return *((volatile long*)0x4ba);
}

static void ckbd_send(long val) {
	while (Bcostat(3) == 0) { }
	Bconout(4, val);
}

static int16_t ckbd_get(ckbd_cookie_eiff_t* eiffel, uint8_t idx, uint8_t* d0, uint8_t* d1, uint8_t* d2, uint8_t* d3) {
	int16_t i;
	eiffel->temp.cfg_index = 0;
	ckbd_send(CKBD_CMD_CKBD_READ_SETTING);
	ckbd_send(idx);
	for (i=0; i<100; i++) {
		uint32_t tm;
		if (eiffel->temp.cfg_index == idx) {
			if (d0) { *d0 = eiffel->temp.cfg_data[0]; }
			if (d1) { *d1 = eiffel->temp.cfg_data[1]; }
			if (d2) { *d2 = eiffel->temp.cfg_data[2]; }
			if (d3) { *d3 = eiffel->temp.cfg_data[3]; }
			return 1;
		}

		tm = Supexec(ckbd_get200hz);
		while (tm == Supexec(ckbd_get200hz));
	}
	return 0;
}

static void ckbd_set(uint8_t idx, uint8_t val) {
	ckbd_send(CKBD_CMD_CKBD_PROG_SETTING);
	ckbd_send(idx);
	ckbd_send(val);
}

static void ckbd_save(void) {
	ckbd_set(0xff, 0x00);
}

#endif /* _CKBD_EIFFEL_H_ */
