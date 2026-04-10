/*
	CPU settings

	Copyright (C) 2024	Anders Granlund
    Copyright (C) 2009	Patrice Mandin

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>

#include <mint/osbind.h>
#include <mint/falcon.h>
#include <mint/cookie.h>
#include "../rvbios.h"
#include "raven.h"
#include "form_vt.h"
#include "misc.h"
#include "f_cpu.h"
#include "f_exit.h"

#include "raven.h"

/*--- Defines ---*/

typedef enum
{
	FORM_CHIPSET=0,
	FORM_CPU,
	FORM_FPU,
	FORM_RAM,

	FORM_STRAM_SIZE,
	FORM_TTRAM_SIZE,
	FORM_STRAM_CACHE,
	FORM_TTRAM_CACHE,

	FORM_ROM_UPDATE,
	FORM_KBD_UPDATE,
	FORM_MAX
};

typedef enum
{
	FORM_SETTING_STRAM_SIZE=0,
	FORM_SETTING_STRAM_CACHE,
	FORM_SETTING_TTRAM_CACHE,
	FORM_SETTING_ROM_UPDATE,
	FORM_SETTING_KBD_UPDATE,
	FORM_SETTING_MAX
};

#define FORM_X0					2
#define FORM_Y0					2
#define FORM_X1					39
#define FORM_TEXTPOS			21
#define FORM_TEXTPOS_CPUREV		(FORM_TEXTPOS + 6)

#define CHAR_DEG	"\xf8"


static void funcUpdateRom(void);
static void funcUpdateKbd(void);


/*--- Const ---*/
static void updownSTRAM(int direction);
static void updownSTRAMC(int direction);
static void updownTTRAMC(int direction);

enum 								{ FPU_NONE, FPU_SOFT, 	FPU_SFP, 	FPU_6888X, 	FPU_68881, 	FPU_68882, 	FPU_68040, 	FPU_68060 };
static const char* fpu_string[] = 	{ "-----", 	"Emulated", "SFP-004", 	"6888X", 	"68881", 	"68882", 	"68040", 	"68060" };

enum								{ CM_WT,          CM_CB,          CM_PRECISE,     CM_IMPRECISE,  CM_MAX };
static const char* cm_string[] =	{ "Writethrough", "Copyback    ", "Precise     ", "Imprecise   " };

static form_t form_cpu[]={
	{FORM_TEXT, "CHIPSET ............ RAVEN __",		FORM_X0, FORM_Y0+0},
	{FORM_TEXT, "CPU ................ -----       ", 	FORM_X0, FORM_Y0+2},
	{FORM_TEXT, "FPU ................ -----       ", 	FORM_X0, FORM_Y0+3},
	{FORM_TEXT, "RAM ................ -- MB",			FORM_X0, FORM_Y0+4},

    {FORM_TEXT, "ST-RAM Size ........ -- MB", 			FORM_X0, FORM_Y0+6},
	{FORM_TEXT, "TT-RAM Size ........ -- MB", 			FORM_X0, FORM_Y0+7},

	{FORM_TEXT, "ST-RAM Cache ....... ------------", 	FORM_X0, FORM_Y0+9},
	{FORM_TEXT, "TT-RAM Cache ....... ------------", 	FORM_X0, FORM_Y0+10},

    {FORM_TEXT, "ROM xxxxxx ......... UPDATE",          FORM_X0, FORM_Y0+13},

    {FORM_TEXT, "KBD Eiffel ......... UPDATE",		    FORM_X0, FORM_Y0+15},

	{FORM_END, 0,0,0}
};

form_setting_t form_setting_cpu[]={
	{FORM_X0+FORM_TEXTPOS, FORM_Y0+ 6, NULL, SETTING_UPDOWN,  2, updownSTRAM},
	{FORM_X0+FORM_TEXTPOS, FORM_Y0+ 9, NULL, SETTING_UPDOWN, 12, updownSTRAMC},
	{FORM_X0+FORM_TEXTPOS, FORM_Y0+10, NULL, SETTING_UPDOWN, 12, updownTTRAMC},

	{FORM_X0+FORM_TEXTPOS, FORM_Y0+13, NULL, SETTING_FUNC, 1, funcUpdateRom},

    {FORM_X0+FORM_TEXTPOS, FORM_Y0+15, NULL, SETTING_FUNC, 1, funcUpdateKbd},
	{0, 0, NULL, SETTING_END}
};

/*--- Global variables ---*/



/*--- Variables ---*/

static void exitFormCpu(void);
static void initFormCpu(void);
static void confirmFormCpu(int num_setting, conf_setting_u* confSetting);


const form_menu_t form_menu_cpu={
	displayFormCpu,
	updateFormCpu,
	initFormCpu,
	confirmFormCpu,
	NULL, exitFormCpu
};

static unsigned char ram_total = 0;
static unsigned char stram_size = 0, stram_min = 1, stram_max = 4;
static unsigned char stram_cm = CM_WT;
static unsigned char ttram_cm = CM_WT;
static unsigned char nv_flags = 0;


/*--- Functions ---*/

/*
static void readCT60Freq(void);
static void readCT60Temp(void);
*/

/*--- Functions prototypes ---*/

void detectCPU(unsigned long* cpu, unsigned long* rev, unsigned long* fpu)
{
	unsigned long cookie_fpu;
	unsigned long cookie_cpu;
	unsigned long pcr = 0L;

	if (Getcookie(C__CPU, (long*)&cookie_cpu) != C_FOUND) {
		cookie_cpu = 60L;
	}
	if (Getcookie(C__FPU, (long*)&cookie_fpu) != C_FOUND) {
		cookie_fpu = 0L;
	}
	if (cookie_cpu == 60) {
		pcr = pcr_get();
	}

	*fpu = FPU_NONE;
	if (cookie_fpu & 0xffffL)				{ *fpu = FPU_SOFT;  }
	else if (cookie_fpu & (1L<<20))			{ *fpu = FPU_68060; }
	else if (cookie_fpu & (1L<<19))			{ *fpu = FPU_68040; }
	else if ((cookie_fpu & (3L<<17)) == 1)	{ *fpu = FPU_68882; }
	else if ((cookie_fpu & (3L<<17)) == 2)	{ *fpu = FPU_68881; }
	else if ((cookie_fpu & (3L<<17)) == 3)	{ *fpu = FPU_6888X; }
	else if (cookie_fpu & (1L << 16))		{ *fpu = FPU_SFP;   }

	*cpu = cookie_cpu + 68000L;
	*rev = (pcr >> 8) & 0xffL;
}

void detectRAM(void)
{
	unsigned int  ttram_size = 0;
	unsigned long phystop = *((volatile unsigned long*)0x42e);
	unsigned long ramtop  = *((volatile unsigned long*)0x5a4);
	unsigned long ramval  = *((volatile unsigned long*)0x5a8);

	stram_size = (int) ((phystop + 0x000FFFFFL) >> 20);
	if (stram_size < stram_min) stram_size = stram_min;
	if (stram_size > stram_max) stram_size = stram_max;
	if (ramval == 0x1357BD13L) {
		ttram_size = (int) (((ramtop - 0x01000000L) + 0x000FFFFFL) >> 20);
	}
	ram_total = stram_size + ttram_size;
}

void loadSettings(void)
{
	unsigned long cacr = cacr_get();
	unsigned long pcr  = pcr_get();
    /*
    unsigned long flags;
    */
	nv_flags = 0;
	nv_flags |= (cacr & (1L<<15)) >> 15;
	nv_flags |= (cacr & (1L<<31)) >> 30;
	nv_flags |= (cacr & (1L<<23)) >> 21;
	nv_flags |= (cacr & (1L<<29)) >> 26;
	nv_flags |= (pcr & (1<<0)) << 4;
	nv_flags |= (pcr & (1<<1)) << 4;

    /*
    flags = raven()->cfg_Read("cpuflags");
    */
    if ((raven()->version & 0x00ffffffUL) >= 0x00241225UL) {
        stram_cm = raven()->cfg_Read("st_ram_cache");
        ttram_cm = raven()->cfg_Read("tt_ram_cache");
    }
}

void saveSettings(void)
{
    if ((raven()->version & 0x00ffffffUL) >= 0x00241225UL) {
        raven()->cfg_Write("st_ram_size", stram_size);
        raven()->cfg_Write("st_ram_cache", stram_cm);
        raven()->cfg_Write("tt_ram_cache", ttram_cm);
/*
    { "cpuflags",       0,  0, 0x3A, 6, 0, 0, 0x3F, 0x3F },
    { "boot_enable",    0,  0, 0x3B, 1, 0, 0, 1,    1 },
    { "boot_delay",     0,  0, 0x3B, 4, 4, 0, 15,   0 },
*/
    }
}

void refreshFormCpu(void)
{
	unsigned long ramt = (ram_total + 3) & 0xfffffffcL;
	format_number(&form_cpu[FORM_RAM].text[FORM_TEXTPOS],   ramt, 2, ' ');	
	format_number(&form_cpu[FORM_STRAM_SIZE].text[FORM_TEXTPOS], stram_size, 2, ' ');
	format_number(&form_cpu[FORM_TTRAM_SIZE].text[FORM_TEXTPOS], ram_total - stram_size, 2, ' ');
	strCopy(cm_string[stram_cm], &form_cpu[FORM_STRAM_CACHE].text[FORM_TEXTPOS]);
	strCopy(cm_string[ttram_cm], &form_cpu[FORM_TTRAM_CACHE].text[FORM_TEXTPOS]);
}

void exitFormCpu(void)
{
    saveSettings();
}

void initFormCpu(void)
{
	/* todo: load settings */

	unsigned long fpu_type, cpu_type, cpu_rev;
	unsigned long rom;
    unsigned long kbd;
	unsigned short rev;

	loadSettings();		
	detectRAM();

	/* cpu, fpu */
	detectCPU(&cpu_type, &cpu_rev, &fpu_type);
	format_number(&form_cpu[FORM_CPU].text[FORM_TEXTPOS], cpu_type, 5, ' ');
	if (cpu_rev > 0) {
		strCopy("Rev.", &form_cpu[FORM_CPU].text[FORM_TEXTPOS_CPUREV]);
		format_number(&form_cpu[FORM_CPU].text[FORM_TEXTPOS_CPUREV+4], cpu_rev, 1, ' ');
	}
	strCopy(fpu_string[fpu_type], &form_cpu[FORM_FPU].text[FORM_TEXTPOS]);

	/* info */
	rom = *((unsigned long*)0x40000000L);
	if ((rom & 0xffff0000L) == 0x40000000L) {
		rom = *((unsigned long*)(rom+4));
		rev = (unsigned short) (rom >> 24);
		rom &= 0x00ffffffL;
	} else {
		rev = 0;
		rom = 0;
	}
    kbd = 0;

    if (Getcookie(0x54656D70UL, (long*)&kbd) == C_FOUND) {
        kbd = *((uint32_t*)(kbd+14)) & 0x00ffffffUL;
    }

	/* chipset */
	rev = raven()->chipset ? (raven()->chipset() & 0xFF) : 0xA1;
	format_number_hex(&form_cpu[FORM_CHIPSET].text[FORM_TEXTPOS+6], rev, 2, 0);

	/* ram */
	form_setting_cpu[FORM_SETTING_STRAM_SIZE].text = &form_cpu[FORM_STRAM_SIZE].text[FORM_TEXTPOS];
	form_setting_cpu[FORM_SETTING_STRAM_CACHE].text = &form_cpu[FORM_STRAM_CACHE].text[FORM_TEXTPOS];
	form_setting_cpu[FORM_SETTING_TTRAM_CACHE].text = &form_cpu[FORM_TTRAM_CACHE].text[FORM_TEXTPOS];

	/* rom version */
    format_number_hex(&form_cpu[FORM_ROM_UPDATE].text[4], rom, 6, 0);
	form_setting_cpu[FORM_SETTING_ROM_UPDATE].text = &form_cpu[FORM_ROM_UPDATE].text[FORM_TEXTPOS];

    /* kbd version */
    if (kbd > 0) {
        format_number_hex(&form_cpu[FORM_KBD_UPDATE].text[4], kbd, 6, 0);
    } else {
        form_cpu[FORM_KBD_UPDATE].text[FORM_TEXTPOS] = 0;
        form_setting_cpu[FORM_SETTING_KBD_UPDATE].input = SETTING_HIDDEN;
    }
	form_setting_cpu[FORM_SETTING_KBD_UPDATE].text = &form_cpu[FORM_KBD_UPDATE].text[FORM_TEXTPOS];

	refreshFormCpu();
}

void displayFormCpu(void)
{
	vt_displayForm(form_cpu);
}

void updateFormCpu(void)
{
}

static void confirmFormCpu(int num_setting, conf_setting_u* confSetting)
{
	(void)confSetting;

	switch(num_setting) {
		case FORM_SETTING_STRAM_SIZE:
		case FORM_SETTING_STRAM_CACHE:
		case FORM_SETTING_TTRAM_CACHE:
            exit_flag |= EXIT_FLAG_COLD_RESET;
			break;
/*            
		case FORM_SETTING_CACR_EIC: nv_flags = (nv_flags & ~(1<<0)) | (~nv_flags & (1<<0)); break;
		case FORM_SETTING_CACR_EDC: nv_flags = (nv_flags & ~(1<<1)) | (~nv_flags & (1<<1)); break;
		case FORM_SETTING_CACR_EBC: nv_flags = (nv_flags & ~(1<<2)) | (~nv_flags & (1<<2)); break;
		case FORM_SETTING_CACR_ESB: nv_flags = (nv_flags & ~(1<<3)) | (~nv_flags & (1<<3)); break;
		case FORM_SETTING_PCR_SS: 	nv_flags = (nv_flags & ~(1<<4)) | (~nv_flags & (1<<4)); break;
            exit_flag |= EXIT_FLAG_COLD_RESET;
            break;
*/        
		default:
			break;
	}

	refreshFormCpu();
	vt_displayForm_idx(form_cpu, 0, FORM_MAX);
}

static void updownSTRAM(int direction)
{
	switch(direction) {
		case SETTING_DIR_UP:
			stram_size += (stram_size < stram_max) ? 1 : 0;
			break;
		case SETTING_DIR_DOWN:
			stram_size -= (stram_size > stram_min) ? 1 : 0; 
			break;
		case SETTING_DIR_PRINT:
			{
				char s[6] = "__ MB";
				format_number(&s[0], stram_size, 2, ' ');
				Cconws(s);
			}
			break;
	}
}

static void updownSTRAMC(int direction)
{
	switch(direction) {
		case SETTING_DIR_UP:    stram_cm += (stram_cm < (CM_MAX-1)) ? 1 : 0; break;
		case SETTING_DIR_DOWN:  stram_cm -= (stram_cm > 0) ? 1 : 0; break;
		case SETTING_DIR_PRINT: Cconws(cm_string[stram_cm]); break;
	}
}

static void updownTTRAMC(int direction)
{
	switch(direction) {
		case SETTING_DIR_UP:    ttram_cm += (ttram_cm < (CM_MAX-1)) ? 1 : 0; break;
		case SETTING_DIR_DOWN:  ttram_cm -= (ttram_cm > 0) ? 1 : 0; break;
		case SETTING_DIR_PRINT: Cconws(cm_string[ttram_cm]); break;
	}
}


static void cdecl (*mon_putc_old)(int32_t);
static int32_t cdecl (*mon_getc_old)(void);

static void cdecl mon_putc(int32_t c) {
    if (c == '\n') { Cconout('\r'); }
    Cconout((int16_t)c);
}
static int32_t cdecl mon_getc(void) {
    return (int32_t)Cconin();
}

static void delay(uint16_t ms) {
    uint32_t ta = *((volatile uint32_t*)0x4ba);
    uint32_t tb = ta + ((ms / 5) + 1);
    while ( *((volatile uint32_t*)0x4ba) < tb);
}

extern void display_restore(void);

static void funcUpdateRom(void) {
    int16_t key;

    Cconws(CLEAR_HOME "\r\n");
    Cconws("Raven ROM update\r\n\r\n");
    Cconws("** WARNING **\r\n");
    Cconws("This will flash a new system ROM over serial port COM1.\r\n");
    Cconws("Press Y if you want to proceed. ");

    key = mon_getc();
    if ((key == 'Y') || (key == 'y')) {
        Cconws("\r\n\r\n");
        *(raven()->mon_fgetchar) = mon_getc;
        *(raven()->mon_fputchar) = mon_putc;
        raven()->mon_Exec("flash");
        *(raven()->mon_fgetchar) = mon_getc_old;
        *(raven()->mon_fputchar) = mon_putc_old;
        delay(2000);
        Cconws(CLEAR_HOME "\r\n");
        raven()->sys_reset(2);
        delay(500);
    }
    display_restore();
    displayFormCpu();
}

static void funcUpdateKbd(void) {
    int16_t key;
	Cconws(CLEAR_HOME "\r\n");
    Cconws("Raven KBD update\r\n\r\n");
    Cconws("** WARNING **\r\n");
    Cconws("This will flash a new CKBD firmware over serial port COM1.\r\n");
    Cconws("Press Y if you want to proceed. ");
    key = mon_getc();
    if ((key == 'Y') || (key == 'y')) {
        Cconws("\r\n\r\n");
        *(raven()->mon_fgetchar) = mon_getc;
        *(raven()->mon_fputchar) = mon_putc;
        raven()->mon_Exec("kbd flash");
        *(raven()->mon_fgetchar) = mon_getc_old;
        *(raven()->mon_fputchar) = mon_putc_old;
        delay(2000);
        Cconws(CLEAR_HOME "\r\n");
        raven()->sys_reset(2);
        delay(500);
    }
    display_restore();
    displayFormCpu();
}
