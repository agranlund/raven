/*-------------------------------------------------------------------------------
 * Raven support software
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
#include <tos.h>
#include <linea.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <mint/cookie.h>
#include <mint/osbind.h>

/*
#define _RVBIOS_INTERNAL_
#include "raven.h"
*/
#include "rvbios.h"

#include "setup/setup.h"
#include "setup/form_vt.h"


/*----------------------------------------
	Constants
----------------------------------------*/
#define SETUP_ONLY		0
#define ENABLE_SETUP	1

#define COL_FG			COL_BLACK
#define COL_BG			COL_WHITE

/*----------------------------------------
	Globals
----------------------------------------*/
extern uint8_t LogoPic;

/*----------------------------------------
	Create or change cookie
----------------------------------------*/
void Setcookie(uint32_t c, uint32_t d)
{
	int32_t cookies_size = 0;
	int32_t cookies_used = 0;
	int32_t cookies_avail = 0;
	uint32_t* jar = (uint32_t*) *((uint32_t*)0x5A0);
	uint32_t* ptr = jar;
	
	/* replace value or find number of free slots */
	while (1){
		cookies_used++;
		if (ptr[0] == c){
			ptr[1] = d;
			return;
		}
		else if (ptr[0] == 0){
			cookies_size = ptr[1];
			break;
		}
		ptr+=2;
	}

	/* grow jar when necessary */
	cookies_avail = cookies_size - cookies_used;
	if (cookies_avail <= 0) {
		int32_t oldsize = (2*4*(cookies_size + 0));
		int32_t newsize = (2*4*(cookies_size + 8));
		uint32_t* newjar = (uint32_t*)Malloc(newsize);
		memcpy(newjar, jar, oldsize);
		*((uint32_t*)0x5A0) = (uint32_t)newjar;
		jar = newjar;
		cookies_size = cookies_size + 8;
	}
	
	/* install cookie */
	jar[(cookies_used<<1)-2] = c;
	jar[(cookies_used<<1)-1] = d;
	jar[(cookies_used<<1)+0] = 0;
	jar[(cookies_used<<1)+1] = cookies_size;
}


/*----------------------------------------
	Locale
----------------------------------------*/

void InitTime(void)
{
#if XBTIME
	/* Init TOS time and date from RTC before installing
	   our own xbios override.
	   This is a workaround for EmuTOS's internal timekeep
	   variable. Atari TOS does not have this problem.
	   We can skip this if TOS has nvram support built-in
	*/
	if (Getcookie(C__IDT,(long*)&cookie) != C_FOUND) {
		uint32_t dt   = xbc_gettime();
		uint16_t date = (uint16_t) ((dt>>16)&0xffff);
		uint16_t time = (uint16_t) ((dt>> 0)&0xffff);
		Tsetdate(date);
		Tsettime(time);
	}
#endif
}


void InstallCookies(void)
{
#if XBNVM
	uint8_t r[4];
	if (NVMaccess(0, 6, 4, r) == 0)
	{
		uint32_t cookie;
		uint16_t country  = (uint16_t)r[0];
		uint16_t keyboard = (uint16_t)r[1];
		uint16_t idt0     = (uint16_t)r[2];
		uint16_t idt1	  = (uint16_t)r[3];

		if (Getcookie(C__AKP,(long*)&cookie) != C_FOUND) {
			Setcookie(C__AKP, (country << 8) | keyboard);
		}

		if (Getcookie(C__IDT,(long*)&cookie) != C_FOUND) {
			Setcookie(C__IDT, (idt0 << 8) | idt1);
		}	
	}
#endif
	
	Setcookie(C_RAVN, (uint32_t)raven());
}

/*----------------------------------------
	Boot info
----------------------------------------*/
void bootscreen(void)
{
	int i;
	const uint32_t  logo_x = 0;
	const uint32_t  logo_y = 8;
	const uint32_t  logo_w = 3;
	const uint32_t  logo_h = 56+30;

	uint32_t  vram_w = (uint32_t)(Vdiesc->bytes_lin >> 2);
	uint32_t* vram = (uint32_t*)Physbase();
	uint32_t* logo = (uint32_t*)&LogoPic;

	vt_setFgColor(COL_FG);
	vt_setBgColor(COL_BG);
	Cconws(C_OFF);
	Cconws(CLEAR_HOME);
    vt_setCursorPos(0, 7);	

	vram += (logo_x + (logo_y * vram_w));
	for (i=0; i<logo_h; i++) {
#if (COL_BG == COL_BLACK)
		*vram++ = 0xffffffffUL ^ *logo++;
		*vram++ = 0xffffffffUL ^ *logo++;
		*vram++ = 0xffffffffUL ^ *logo++;
#else
		*vram++ = *logo++ ;
		*vram++ = *logo++;
		*vram++ = *logo++;
#endif
		vram += (vram_w - logo_w);
	}
}


/*----------------------------------------
	Boot delay and setup
----------------------------------------*/
int setup(void)
{
#if SETUP_ONLY
	setup_main();
#else

	unsigned long boot_delay = 5;
	
	if (boot_delay > 0) {
		unsigned long dot_tick, cur_tick, start_tick;
		int start_setup = 0;

#if ENABLE_SETUP
/*
		Cconws(" Press DEL to enter setup.");
*/
#else
		Cconws(" ");
#endif
		start_tick = dot_tick = cur_tick = ticks_get();

		while (cur_tick-start_tick < (200*boot_delay)) {
			if (Cconis() != 0) {
				unsigned long key_pressed = Cnecin();
				unsigned char scancode = (key_pressed >> 16) & 0xff;
				if (scancode == SCANCODE_DELETE) {
					start_setup = 1;
					break;
				}
				else if (scancode == SCANCODE_ESCAPE || scancode == SCANCODE_SPACE /*|| scancode == SCANCODE_LEFTSHIFT*/) {
					break;
				}
			}

			if (cur_tick-dot_tick>200) {
				dot_tick = cur_tick;
/*				Cconws(".");
*/
			}
			cur_tick = ticks_get();
		}
		
    	Cconws(DEL_BOL "\r");
   		if (ENABLE_SETUP && start_setup) {
   			setup_main();
			vt_setFgColor(COL_FG);
			vt_setBgColor(COL_BG);
   			return 1;
   		}
	}
#endif
	return 0;
}


/*----------------------------------------
	Main
----------------------------------------*/

long supermain()
{
	uint16_t ipl;

	linea_init();

	/* fetch pointer rom bios */	
	if (raven()->magic != C_RAVN) {
		return -1;
	}

	/* boot screen */
	bootscreen();

	/* install xbios extensions */
	ipl = ipl_set(0x0700);

#if !SETUP_ONLY
	InitTime();

	InstallXbios();

	InstallEiffel();

	Install060sp();

	InstallCookies();
#endif
	cache_clear();

	ipl_set(ipl);

#if ENABLE_SETUP
	if (setup() != 0) {
		vt_setFgColor(COL_FG);
		vt_setBgColor(COL_BG);
		Cconws(CLEAR_HOME "\r\n");
		/* todo: reset */
	}
#endif

	return 1;
}

long main()
{
	if (Supexec(supermain)) {
		Ptermres(_PgmSize, 0);
	}
	return 0;
}

long rom_main()
{
	return Supexec(supermain);
}


