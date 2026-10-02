/*
	NVRAM + IKBD fan settings

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
#include <mint/sysvars.h>
#include <mint/cookie.h>
#include "ckbd.h"

#include "form_vt.h"
#include "misc.h"
#include "nvram.h"
#include "f_nvram.h"
#include "f_exit.h"
#include "../rvbios.h"


/*--- Defines ---*/

#define FORM_DATE 		0
#define FORM_TIME 		1
#define FORM_LANG 		2
#define FORM_DELAY 		4
#define FORM_FAN_MODE	5
#define FORM_FAN_TLO	6
#define FORM_FAN_THI	7
#define FORM_FAN_SLO	8
#define FORM_FAN_SHI	9
#define FORM_RGB_I		10
#define FORM_RGB_R		11
#define FORM_RGB_G		12
#define FORM_RGB_B		13

#define FORM_SETTING_DATE 		0
#define FORM_SETTING_TIME 		(FORM_SETTING_DATE+3)
#define FORM_SETTING_LANG 		(FORM_SETTING_TIME+2)
#define FORM_SETTING_DELAY 		(FORM_SETTING_LANG+2)
#define FORM_SETTING_FAN_MODE	(FORM_SETTING_DELAY+1)
#define FORM_SETTING_FAN_TLO	(FORM_SETTING_FAN_MODE+1)
#define FORM_SETTING_FAN_THI	(FORM_SETTING_FAN_TLO+1)
#define FORM_SETTING_FAN_SLO	(FORM_SETTING_FAN_THI+1)
#define FORM_SETTING_FAN_SHI	(FORM_SETTING_FAN_SLO+1)
#define FORM_SETTING_RGB_I		(FORM_SETTING_FAN_SHI+1)
#define FORM_SETTING_RGB_R		(FORM_SETTING_RGB_I+1)
#define FORM_SETTING_RGB_G		(FORM_SETTING_RGB_R+1)
#define FORM_SETTING_RGB_B		(FORM_SETTING_RGB_G+1)

#define MASK_DATE_DAY 		((1<<5)-1)
#define SHIFT_DATE_DAY		0
#define MASK_DATE_MONTH 	((1<<4)-1)
#define SHIFT_DATE_MONTH	5
#define MASK_DATE_YEAR 		((1<<7)-1)
#define SHIFT_DATE_YEAR		9

#define MASK_TIME_SECOND 	((1<<5)-1)
#define SHIFT_TIME_SECOND	0
#define MASK_TIME_MINUTE 	((1<<6)-1)
#define SHIFT_TIME_MINUTE	5
#define MASK_TIME_HOUR 		((1<<5)-1)
#define SHIFT_TIME_HOUR		11

#define NUM_LANG_TOS 6
#define NUM_LANG_KBD 18

#define FORM_X0			2
#define FORM_Y0			2
#define FORM_TEXTPOS	21

static const char* lang_tos[NUM_LANG_TOS+1] = { "American", "German  ", "French  ", "British ", "Spanish ", "Italian ",	NULL };
static const char* lang_kbd[NUM_LANG_KBD+1] = { "US", "DE", "FR", "UK", "ES", "IT", "SE", "SF", "SG", "TR", "FI", "NO", "DK", "SA", "NL", "CZ", "HU", "PL",	NULL };
static const char* fanmode_list[4] = { "AUTO", "ON  ", "OFF ", NULL };
static const uint8_t fanmode_to_ikbd[4] = { 3, 1, 0, 3 };
static const uint8_t fanmode_from_ikbd[4] = { 2, 1, 0, 0 };

static void exitFormNvram();
static void saveToNvram(void);
static void confirmFormNvram(int num_setting, conf_setting_u* confSetting);
static void updateClock(void);
static void updateValues(void);

static form_t form_nvram[]={
	{FORM_TEXT, "Date ............... --/--/----", 		FORM_X0, FORM_Y0+0},
	{FORM_TEXT, "Time ............... --:--:--", 		FORM_X0, FORM_Y0+1},

	{FORM_TEXT, "Language ...........                 ",FORM_X0, FORM_Y0+3},
	{FORM_TEXT, "Keyboard ........... --", 				FORM_X0, FORM_Y0+4},

	{FORM_TEXT, "Boot Delay ......... - sec",			FORM_X0, FORM_Y0+6},

	{FORM_TEXT, "Fan mode ...........      ",			FORM_X0, FORM_Y0+8},
	{FORM_TEXT, "Fan temp lo ........ --- C",			FORM_X0, FORM_Y0+10},
	{FORM_TEXT, "Fan temp hi ........ --- C",			FORM_X0, FORM_Y0+11},
	{FORM_TEXT, "Fan speed lo ....... --- \%",			FORM_X0, FORM_Y0+12},
	{FORM_TEXT, "Fan speed hi ....... --- \%",			FORM_X0, FORM_Y0+13},

	{FORM_TEXT, "RGB Brightness ..... --- ",			FORM_X0, FORM_Y0+15},
	{FORM_TEXT, "RGB Red ............ --- ",			FORM_X0, FORM_Y0+16},
	{FORM_TEXT, "RGB Green .......... --- ",			FORM_X0, FORM_Y0+17},
	{FORM_TEXT, "RGB Blue ........... --- ",			FORM_X0, FORM_Y0+18},

	{FORM_END, 0,0,0}
};

form_setting_t form_setting_nvram[]={
	/* Date */
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+0, NULL, SETTING_INPUT, 2},
	{FORM_X0+FORM_TEXTPOS+3, FORM_Y0+0, NULL, SETTING_INPUT, 2},
	{FORM_X0+FORM_TEXTPOS+6, FORM_Y0+0, NULL, SETTING_INPUT, 4},

	/* Time */
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+1, NULL, SETTING_INPUT, 2},
	{FORM_X0+FORM_TEXTPOS+3, FORM_Y0+1, NULL, SETTING_INPUT, 2},
/*	{FORM_X0+FORM_TEXTPOS+6, FORM_Y0+1, NULL, SETTING_INPUT, 2},*/

	/* Language + Keyboard */
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+3, NULL, SETTING_LIST, 8, lang_tos},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+4, NULL, SETTING_LIST, 2, lang_kbd},

	/* boot delay */
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+6, NULL, SETTING_INPUT, 1},

	/* fan */
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+ 8, NULL, SETTING_LIST,  4, fanmode_list},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+10, NULL, SETTING_INPUT, 3},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+11, NULL, SETTING_INPUT, 3},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+12, NULL, SETTING_INPUT, 3},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+13, NULL, SETTING_INPUT, 3},

	/* rgb */
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+15, NULL, SETTING_INPUT, 3},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+16, NULL, SETTING_INPUT, 3},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+17, NULL, SETTING_INPUT, 3},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+18, NULL, SETTING_INPUT, 3},

	{0, 0, NULL, SETTING_END}
};


const form_menu_t form_menu_nvram={
	displayFormNvram,
	updateFormNvram,
	initFormNvram,
	confirmFormNvram,
	NULL, exitFormNvram
};

static unsigned long start_tick;
static unsigned long cur_tick;
static unsigned char nvram[17];

static unsigned char fanmode;
static unsigned char fantemp[2];
static unsigned char fanspeed[2];
static unsigned char argb[4];

ckbd_cookie_eiff_t* eiffel;


void initFormNvram(void) {
	form_setting_nvram[FORM_SETTING_DATE].text = &form_nvram[FORM_DATE].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_DATE+1].text = &form_nvram[FORM_DATE].text[FORM_TEXTPOS+3];
	form_setting_nvram[FORM_SETTING_DATE+2].text = &form_nvram[FORM_DATE].text[FORM_TEXTPOS+6];

	form_setting_nvram[FORM_SETTING_TIME].text = &form_nvram[FORM_TIME].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_TIME+1].text = &form_nvram[FORM_TIME].text[FORM_TEXTPOS+3];
/*	form_setting_nvram[FORM_SETTING_TIME+2].text = &form_nvram[FORM_TIME].text[FORM_TEXTPOS+6]; */

	form_setting_nvram[FORM_SETTING_LANG].text = &form_nvram[FORM_LANG].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_LANG+1].text = &form_nvram[FORM_LANG+1].text[FORM_TEXTPOS];

	form_setting_nvram[FORM_SETTING_DELAY].text = &form_nvram[FORM_DELAY].text[FORM_TEXTPOS];

	form_setting_nvram[FORM_SETTING_FAN_MODE].text = &form_nvram[FORM_FAN_MODE].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_FAN_TLO].text = &form_nvram[FORM_FAN_TLO].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_FAN_THI].text = &form_nvram[FORM_FAN_THI].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_FAN_SLO].text = &form_nvram[FORM_FAN_SLO].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_FAN_SHI].text = &form_nvram[FORM_FAN_SHI].text[FORM_TEXTPOS];
#if 0
	form_nvram[FORM_FAN_TLO].text[FORM_TEXTPOS+3] = 0xf8;	/* degrees */
	form_nvram[FORM_FAN_THI].text[FORM_TEXTPOS+3] = 0xf8;	/* degrees */
#endif
	form_setting_nvram[FORM_SETTING_RGB_I].text = &form_nvram[FORM_RGB_I].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_RGB_R].text = &form_nvram[FORM_RGB_R].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_RGB_G].text = &form_nvram[FORM_RGB_G].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_RGB_B].text = &form_nvram[FORM_RGB_B].text[FORM_TEXTPOS];

	fanmode = 0;
	fantemp[0] = 0;
	fantemp[1] = 0;
	fanspeed[0] = 0;
	fanspeed[1] = 100;
	argb[0] = 0;
	argb[1] = 0;
	argb[2] = 0;
	argb[3] = 0;

	if (Getcookie(C_Eiff, (long*)&eiffel) == C_FOUND) {
		ckbd_send(CKBD_CMD_EIFFEL_GET_TEMP);
		delay(100);
		fantemp[0] = eiffel->temp.temp0_fan_lo;
		fantemp[1] = eiffel->temp.temp0_fan_hi;
	} else {
		eiffel = 0;
	}

	if (raven()->chipset() >= 0xA2)
	{
		int16_t i;

		ckbd_get(eiffel, CKBD_CFG_FAN0_MODE, &fanmode, 0, 0, 0);
		fanmode = fanmode_from_ikbd[fanmode & 3];

		ckbd_get(eiffel, CKBD_CFG_FAN0_MIN_SPEED, &fanspeed[0], &fanspeed[1], 0, 0);
		for (i=0; i<2; i++) {
			if (fanspeed[i] >= 255) { fanspeed[i] = 100; }
			else if(fanspeed[i] > 0) { fanspeed[i] = ((uint16_t)100 * fanspeed[i]) / 255; }
		}

		ckbd_get(eiffel, CKBD_CFG_RGB_I, &argb[0], &argb[1], &argb[2], &argb[3]);
	}
	else
	{
		/* hide options not available on older revs */
		form_nvram[FORM_FAN_SLO].flag = FORM_END;
		form_setting_nvram[FORM_SETTING_FAN_SLO].input = SETTING_END;
	}

	NVMaccess(NVM_READ, 0, 17, nvram);
}

static void exitFormNvram(void) {
	saveToNvram();
	ckbd_save();
}

void displayFormNvram(void) {
	start_tick = ticks_get();
	updateClock();
	updateValues();
	vt_displayForm(form_nvram);
}

void updateFormNvram(void) {
	/* Update time/date after 1 second */
	cur_tick = ticks_get();
	if (cur_tick-start_tick<200) {
		return;
	}
	start_tick = cur_tick;

	updateClock();
	vt_displayForm_idx(form_nvram, FORM_DATE, 2);
}

static void saveToNvram(void) {
	NVMaccess(NVM_WRITE, 0, 17, nvram);
}

static void confirmFormNvram(int num_setting, conf_setting_u* confSetting) {
	int i;
	unsigned short sys_date, sys_time;
	int save_date = 0;
	int save_time = 0;
	int refresh_nvram = 1;

	sys_date = Tgetdate();
	sys_time = Tgettime();

	switch(num_setting) {
		case FORM_SETTING_DATE:
		{
			i = strToInt(confSetting->input);
			if ((i>=1) && (i<=31)) {
				sys_date &= ~(MASK_DATE_DAY<<SHIFT_DATE_DAY);
				sys_date |= i<<SHIFT_DATE_DAY;
				save_date = 1;
			}
		}
		break;

		case FORM_SETTING_DATE+1:
		{
			i = strToInt(confSetting->input);
			if ((i>=1) && (i<=12)) {
				sys_date &= ~(MASK_DATE_MONTH<<SHIFT_DATE_MONTH);
				sys_date |= i<<SHIFT_DATE_MONTH;
				save_date = 1;
			}
		}
		break;

		case FORM_SETTING_DATE+2:
		{
			i = strToInt(confSetting->input);
			if ((i>=1980) && (i<=1980+127)) {
				sys_date &= ~(MASK_DATE_YEAR<<SHIFT_DATE_YEAR);
				sys_date |= (i-1980)<<SHIFT_DATE_YEAR;
				save_date = 1;
			}
		}
		break;

		case FORM_SETTING_TIME:
		{
			i = strToInt(confSetting->input);
			if ((i>=0) && (i<=23)) {
				sys_time &= ~(MASK_TIME_HOUR<<SHIFT_TIME_HOUR);
				sys_time |= i<<SHIFT_TIME_HOUR;
				save_time = 1;
			}
		}
		break;

		case FORM_SETTING_TIME+1:
		{
			i = strToInt(confSetting->input);
			if ((i>=0) && (i<=59)) {
				sys_time &= ~((MASK_TIME_MINUTE<<SHIFT_TIME_MINUTE)|(MASK_TIME_SECOND<<SHIFT_TIME_SECOND));
				sys_time |= i<<SHIFT_TIME_MINUTE;
				save_time = 1;
			}
		}
		break;

		case FORM_SETTING_LANG:
		{
			nvram[NVRAM_LANGUAGE] = confSetting->num_list;
		}
		break;

		case FORM_SETTING_LANG+1:
		{
			nvram[NVRAM_KEYBOARD] = confSetting->num_list;
		}
		break;

		case FORM_SETTING_DELAY:
		{
			i = strToInt(confSetting->input);
			if ((i>=0) && (i<=255)) {
				nvram[NVRAM_DELAY] = i;
			}
		}
		break;

		case FORM_SETTING_FAN_MODE:
		{
			fanmode = confSetting->num_list & 3;
			ckbd_set(CKBD_CFG_FAN0_MODE, fanmode_to_ikbd[fanmode]);
		}
		break;

		case FORM_SETTING_FAN_TLO:
		case FORM_SETTING_FAN_THI:
		{
			uint8_t idx = num_setting - FORM_SETTING_FAN_TLO;
			i = strToInt(confSetting->input);
			if (i > 99) { i = 99; }
			if (i <  0) { i =  0; }
			fantemp[idx] = i;
			ckbd_set(CKBD_CFG_EIFFEL_TEMP0_LO + (1 - idx), i);
		}
		break;

		case FORM_SETTING_FAN_SLO:
		case FORM_SETTING_FAN_SHI:
		{
			uint8_t fs = 0;
			uint8_t idx = num_setting - FORM_SETTING_FAN_SLO;
			i = strToInt(confSetting->input);
			if (i >= 100) { i = 100; fs = 255; }
			else if (i <= 0) { i = 0; fs = 0; }
			else { fs = (i * 255) / 100; }
			fanspeed[idx] = i;
			ckbd_set(CKBD_CFG_FAN0_MIN_SPEED + idx, fs);
		}
		break;

		case FORM_SETTING_RGB_I:
		case FORM_SETTING_RGB_R:
		case FORM_SETTING_RGB_G:
		case FORM_SETTING_RGB_B:
		{
			
			uint8_t idx = num_setting - FORM_SETTING_RGB_I;
			i = strToInt(confSetting->input) & 0xff;
			argb[idx] = i;
			ckbd_set(CKBD_CFG_RGB_I + idx, i);
			ckbd_send(CKBD_CMD_CKBD_ARGB);
			ckbd_send(0);
			ckbd_send(0xff);
			ckbd_send(1);
			ckbd_send(((uint16_t)argb[0] * argb[1]) >> 8);
			ckbd_send(((uint16_t)argb[0] * argb[2]) >> 8);
			ckbd_send(((uint16_t)argb[0] * argb[3]) >> 8);
		}
		break;
	}

	if (save_date) {
		Tsetdate(sys_date);
	}
	if (save_time) {
		Tsettime(sys_time);
	}
	if (refresh_nvram) {
		updateValues();
		vt_displayForm(form_nvram);
	}

    if (save_date || save_time || refresh_nvram) {
        exit_flag |= EXIT_FLAG_WARM_RESET;
    }
}

static void updateClock(void) {
	unsigned short sys_date, sys_time;

	/* Update date string */
	sys_date = Tgetdate();
	format_number(&form_setting_nvram[FORM_SETTING_DATE].text[0], sys_date & 31, 2, '0');
	format_number(&form_setting_nvram[FORM_SETTING_DATE].text[0+3], (sys_date>>5) & 15, 2, '0');
	format_number(&form_setting_nvram[FORM_SETTING_DATE].text[0+3+3], 1980 + ((sys_date>>9) & 127), 4, '0');

	/* Update time string */
	sys_time = Tgettime();
	format_number(&form_setting_nvram[FORM_SETTING_TIME].text[0], (sys_time>>11) & 31, 2, '0');
	format_number(&form_setting_nvram[FORM_SETTING_TIME].text[0+3], (sys_time>>5) & 63, 2, '0');
	format_number(&form_setting_nvram[FORM_SETTING_TIME].text[0+3+3], (sys_time & 31)<<1, 2, '0');
}

static void updateValues(void) {
	int i;

	/* Language, keyboard */
	i = nvram[NVRAM_LANGUAGE];
	if (i>NUM_LANG_TOS) {
		i = 0;
	}
	strCopy(lang_tos[i], form_setting_nvram[FORM_SETTING_LANG].text);

	i = nvram[NVRAM_KEYBOARD];
	if (i>=NUM_LANG_KBD) {
		i = 0;
	}
	strCopy(lang_kbd[i], form_setting_nvram[FORM_SETTING_LANG+1].text);

	/* Boot delay */
	format_number(form_setting_nvram[FORM_SETTING_DELAY].text, nvram[NVRAM_DELAY], 1, ' ');

	/* fan */
	strCopy(fanmode_list[fanmode&3], form_setting_nvram[FORM_SETTING_FAN_MODE].text);
	format_number(form_setting_nvram[FORM_SETTING_FAN_TLO].text, fantemp[0], 3, ' ');
	format_number(form_setting_nvram[FORM_SETTING_FAN_THI].text, fantemp[1], 3, ' ');
	format_number(form_setting_nvram[FORM_SETTING_FAN_SLO].text, fanspeed[0], 3, ' ');
	format_number(form_setting_nvram[FORM_SETTING_FAN_SHI].text, fanspeed[1], 3, ' ');

	/* rgb */
	format_number(form_setting_nvram[FORM_SETTING_RGB_I].text, argb[0], 3, ' ');
	format_number(form_setting_nvram[FORM_SETTING_RGB_R].text, argb[1], 3, ' ');
	format_number(form_setting_nvram[FORM_SETTING_RGB_G].text, argb[2], 3, ' ');
	format_number(form_setting_nvram[FORM_SETTING_RGB_B].text, argb[3], 3, ' ');
}

