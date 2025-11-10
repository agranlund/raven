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
#include "raven.h"
#include "rvbios.h"

#include "setup/setup.h"
#include "setup/form_vt.h"


/*----------------------------------------
	Constants
----------------------------------------*/
#define SETUP_ONLY		0
#define ENABLE_SETUP	1

#define COL_FG_TOS		COL_BLACK
#define COL_BG_TOS		COL_WHITE

#define BOOTSCREEN_HELP 1
#define BOOTSCREEN_NAME 1


#define C__MCH_RAVEN    0x00070000UL
#define MIN_ROM_VERSION 0x00251110UL

static bool RomVersionValid(void) {
    return ((raven()->version & 0x00ffffffUL) >= MIN_ROM_VERSION) ? true : false;
}

/*----------------------------------------
	Globals
----------------------------------------*/

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
    /* init gemdos time from rtc */
    /*if (Getcookie(C__IDT,(long*)&cookie) != C_FOUND)*/ {
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
#if defined(C__MCH_RAVEN)
	Setcookie(C__MCH, C__MCH_RAVEN);
#endif    
}

/*----------------------------------------
	Boot info
----------------------------------------*/
#include "logo.c"

void bootscreen(void)
{
	int p,i,j,k;
    uint16_t* logoHdr = (uint16_t*)logo_bin;
    uint32_t logo_w = (uint32_t)logoHdr[3];
    uint32_t logo_h = (uint32_t)logoHdr[4];
    int16_t logo_p  = logoHdr[5];
	uint8_t* logo = (uint8_t*)(&logo_bin[64]);

    const uint32_t logo_x = (logo_w < (640 >> 3)) ? ((320 - (logo_w << 2)) >> 3) : 0;
    const uint32_t logo_y = (logo_h < 480) ? (240 - (logo_h >> 1)) : 0;

    uint32_t vram_w = (uint32_t)Vdiesc->bytes_lin;

    /* todo: get background color from logo header data */
    if ((logo[1] & (1<<7)) == 0) {
        vt_setFgColor(COL_BLACK);
    	vt_setBgColor(COL_WHITE);
    } else {
        vt_setFgColor(COL_WHITE);
    	vt_setBgColor(COL_BLACK);
    }

    Cconws(C_OFF);
	Cconws(CLEAR_HOME "\r\n");
    vt_setCursorPos(0, 7);

    for (p=0; p<logo_p; p++) {
        uint8_t* vram = (uint8_t*)Physbase();
        vram += (logo_x + (logo_y * vram_w));
        for (i=0; i<logo_h-1; i++) {
            for (j=0; j<logo_w; ) {
                uint8_t ctrl = *logo++;
                uint8_t runl = ctrl & 0x7f;
                j += runl;
                if (ctrl & 0x80) {
                    uint8_t data = *logo++;
                    for (k=0; k<runl; k++) {
                        *vram++ = data;
                    }
                } else {
                    for (k=0; k<runl; k++) {
                        *vram++ = *logo++;
                    }
                }
            }
            vram += (vram_w - logo_w);
        }
    }
}

extern LINEA *Linea;
extern VDIESC *Vdiesc;
extern FONTS *Fonts;

typedef struct
{
    bool changed;
    FONT_HDR* font;
    short v_cel_ht;
    short v_cel_wr;
    short v_cel_mx;
    short v_cel_my;
    short v_fnt_wd;
    short v_fnt_st;
    short v_fnt_nd;
    void* v_fnt_ad;
    short* v_off_ad;
} fnt_data_t;

static fnt_data_t fnt_data;

void font_init(void) {
    fnt_data.changed = false;
    fnt_data.font = Vdiesc->cur_font;
    fnt_data.v_cel_ht = Vdiesc->v_cel_ht;
    fnt_data.v_cel_wr = Vdiesc->v_cel_wr;
    fnt_data.v_cel_mx = Vdiesc->v_cel_mx;
    fnt_data.v_cel_my = Vdiesc->v_cel_my;
    fnt_data.v_fnt_wd = Vdiesc->v_fnt_wd;
    fnt_data.v_fnt_st = Vdiesc->v_fnt_st;
    fnt_data.v_fnt_nd = Vdiesc->v_fnt_nd;
    fnt_data.v_fnt_ad = Vdiesc->v_fnt_ad;
    fnt_data.v_off_ad = Vdiesc->v_off_ad;
}

void font_small(void) {
    if (!fnt_data.changed) {
        fnt_data.changed = true;
        Vdiesc->cur_font = Fonts->font[1];
        Vdiesc->v_cel_ht = Vdiesc->cur_font->frm_hgt;
        Vdiesc->v_cel_wr = Linea->v_lin_wr * Vdiesc->cur_font->frm_hgt;
        Vdiesc->v_cel_mx = (Vdiesc->v_rez_hz / Vdiesc->cur_font->wcel_wdt) - 1;
        Vdiesc->v_cel_my = (Vdiesc->v_rez_vt / Vdiesc->cur_font->frm_hgt) - 1;
        Vdiesc->v_fnt_wd = Vdiesc->cur_font->frm_wdt;
        Vdiesc->v_fnt_st = Vdiesc->cur_font->ade_lo;
        Vdiesc->v_fnt_nd = Vdiesc->cur_font->ade_hi;
        Vdiesc->v_fnt_ad = Vdiesc->cur_font->fnt_dta;
        Vdiesc->v_off_ad = Vdiesc->cur_font->ch_ofst;
    }
}

void font_default(void) {
    if (fnt_data.changed) {
        fnt_data.changed = false;
        Vdiesc->cur_font = fnt_data.font;
        Vdiesc->v_cel_ht = fnt_data.v_cel_ht;
        Vdiesc->v_cel_wr = fnt_data.v_cel_wr;
        Vdiesc->v_cel_mx = fnt_data.v_cel_mx;
        Vdiesc->v_cel_my = fnt_data.v_cel_my;
        Vdiesc->v_fnt_wd = fnt_data.v_fnt_wd;
        Vdiesc->v_fnt_st = fnt_data.v_fnt_st;
        Vdiesc->v_fnt_nd = fnt_data.v_fnt_nd;
        Vdiesc->v_fnt_ad = fnt_data.v_fnt_ad;
        Vdiesc->v_off_ad = fnt_data.v_off_ad;
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
	
    if (!RomVersionValid()) {
        boot_delay = 10;
    }

	if (boot_delay > 0) {
		unsigned long dot_tick, cur_tick, start_tick;
		int start_setup = 0;

#if ENABLE_SETUP

    #if BOOTSCREEN_NAME
        vt_setCursorPos(79-8, 58);
        printf("RAVEN060");
    #endif        

    #if BOOTSCREEN_HELP
        vt_setCursorPos(1, 58);
		printf("[DEL] Setup");
    #endif
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
            Cconws(CLEAR_HOME "\r\n");
            font_default();
            printf("hello\r\n");
   			setup_main();
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
    long cookie;
    bool tsr_only = false;

    /* fetch pointer rom bios */
	if (raven()->magic != C_RAVN) {
		return -1;
	}

    /* in magic, skip boot and configuration screens
     * assume rvbios.prg has already been run before magxboot.prg
     * so we only install the tsr parts this time around */
    if (Getcookie(C_MagX, &cookie) == C_FOUND) {
        tsr_only = true;        
    }

	/* boot screen */
    if (!tsr_only) {
        linea_init();
        font_init();
        font_small();
        bootscreen();

        if (!RomVersionValid()) {
            printf("*** ROM version is old, please update ***\n");
        }
    }

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
    if (!tsr_only) {
        setup();
        vt_setFgColor(COL_FG_TOS);
        vt_setBgColor(COL_BG_TOS);
        Cconws(CLEAR_HOME "\r\n");
        vt_setCursorPos(0, 0);
        font_default();
    }
#endif

	return 1;
}

unsigned long _StkSize = 4096;
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


