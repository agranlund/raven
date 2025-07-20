/*
	NVRAM settings

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

#include "form_vt.h"
#include "misc.h"
#include "nvram.h"
#include "f_nvram.h"
#include "f_exit.h"
#include "../rvbios.h"

/*--- Defines ---*/

#define FORM_DATE 	0
#define FORM_TIME 	1
#define FORM_LANG 	2
#define FORM_DELAY 	4

#define FORM_SETTING_DATE 0
#define FORM_SETTING_TIME (FORM_SETTING_DATE+3)
#define FORM_SETTING_LANG (FORM_SETTING_TIME+2)
#define FORM_SETTING_DELAY (FORM_SETTING_LANG+2)

#define MASK_DATE_DAY ((1<<5)-1)
#define SHIFT_DATE_DAY	0
#define MASK_DATE_MONTH ((1<<4)-1)
#define SHIFT_DATE_MONTH	5
#define MASK_DATE_YEAR ((1<<7)-1)
#define SHIFT_DATE_YEAR	9

#define MASK_TIME_SECOND ((1<<5)-1)
#define SHIFT_TIME_SECOND	0
#define MASK_TIME_MINUTE ((1<<6)-1)
#define SHIFT_TIME_MINUTE	5
#define MASK_TIME_HOUR ((1<<5)-1)
#define SHIFT_TIME_HOUR	11

#define NUM_LANG_TOS 6
#define NUM_LANG_KBD 18

#define FORM_X0					2
#define FORM_Y0					2
#define FORM_TEXTPOS			21

/*--- Types ---*/

/*--- Const ---*/

static const char *lang_tos[NUM_LANG_TOS+1]={
	"American",
    "German  ",
    "French  ",
    "British ",
	"Spanish ",
    "Italian ",
	NULL
};

static const char *lang_kbd[NUM_LANG_KBD+1]={
	"US", "DE", "FR", "UK",
	"ES", "IT", "SE", "SF",
	"SG", "TR", "FI", "NO",
	"DK", "SA", "NL", "CZ",
	"HU", "PL",
	NULL
};

/*--- Functions prototypes ---*/

static void exitFormNvram();

static void reloadFromNvram(void);
static void saveToNvram(void);
static void confirmFormNvram(int num_setting, conf_setting_u confSetting);

static void readClock(void);
static void readNvram(void);

/*--- Variables ---*/

static form_t form_nvram[]={
	/* Static info */

	{FORM_TEXT, "Date ............... --/--/----", 		FORM_X0, FORM_Y0+0},
	{FORM_TEXT, "Time ............... --:--:--", 		FORM_X0, FORM_Y0+1},

	{FORM_TEXT, "Language ...........                 ",FORM_X0, FORM_Y0+3},
	{FORM_TEXT, "Keyboard ........... --", 				FORM_X0, FORM_Y0+4},

	{FORM_TEXT, "Boot Delay ......... --s",				FORM_X0, FORM_Y0+6},

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
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+6, NULL, SETTING_INPUT, 2},
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

/*--- Functions ---*/

static void exitFormNvram(void)
{
	saveToNvram();
}

void initFormNvram(void)
{
	form_setting_nvram[FORM_SETTING_LANG].text = &form_nvram[FORM_LANG].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_LANG+1].text = &form_nvram[FORM_LANG+1].text[FORM_TEXTPOS];

	form_setting_nvram[FORM_SETTING_DELAY].text = &form_nvram[FORM_DELAY].text[FORM_TEXTPOS];

	form_setting_nvram[FORM_SETTING_DATE].text = &form_nvram[FORM_DATE].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_DATE+1].text = &form_nvram[FORM_DATE].text[FORM_TEXTPOS+3];
	form_setting_nvram[FORM_SETTING_DATE+2].text = &form_nvram[FORM_DATE].text[FORM_TEXTPOS+6];

	form_setting_nvram[FORM_SETTING_TIME].text = &form_nvram[FORM_TIME].text[FORM_TEXTPOS];
	form_setting_nvram[FORM_SETTING_TIME+1].text = &form_nvram[FORM_TIME].text[FORM_TEXTPOS+3];
/*	form_setting_nvram[FORM_SETTING_TIME+2].text = &form_nvram[FORM_TIME].text[FORM_TEXTPOS+6]; */

	NVMaccess(NVM_READ, 0, 17, nvram);
}

void displayFormNvram(void)
{
	start_tick = ticks_get();

	readClock();
	readNvram();

	vt_displayForm(form_nvram);
}

void updateFormNvram(void)
{
	/* Update time/date after 1 second */

	cur_tick = ticks_get();
	if (cur_tick-start_tick<200) {
		return;
	}

	start_tick = cur_tick;

	readClock();
	vt_displayForm_idx(form_nvram, FORM_DATE, 2);
}

static void reloadFromNvram(void)
{
	NVMaccess(NVM_READ, 0, 17, nvram);

	readClock();
	readNvram();

	vt_displayForm(form_nvram);
}

static void saveToNvram(void)
{
	NVMaccess(NVM_WRITE, 0, 17, nvram);
}

static void confirmFormNvram(int num_setting, conf_setting_u confSetting)
{
	int i;
	unsigned short sys_date, sys_time;
	int save_date = 0;
	int save_time = 0;
	int refresh_nvram = 0;

	sys_date = Tgetdate();
	sys_time = Tgettime();

	switch(num_setting) {
		case FORM_SETTING_DATE:
			i = strToInt(confSetting.input);
			if ((i>=1) && (i<=31)) {
				sys_date &= ~(MASK_DATE_DAY<<SHIFT_DATE_DAY);
				sys_date |= i<<SHIFT_DATE_DAY;
				save_date = 1;
			}
			break;
		case FORM_SETTING_DATE+1:
			i = strToInt(confSetting.input);
			if ((i>=1) && (i<=12)) {
				sys_date &= ~(MASK_DATE_MONTH<<SHIFT_DATE_MONTH);
				sys_date |= i<<SHIFT_DATE_MONTH;
				save_date = 1;
			}
			break;
		case FORM_SETTING_DATE+2:
			i = strToInt(confSetting.input);
			if ((i>=1980) && (i<=1980+127)) {
				sys_date &= ~(MASK_DATE_YEAR<<SHIFT_DATE_YEAR);
				sys_date |= (i-1980)<<SHIFT_DATE_YEAR;
				save_date = 1;
			}
			break;
		case FORM_SETTING_TIME:
			i = strToInt(confSetting.input);
			if ((i>=0) && (i<=23)) {
				sys_time &= ~(MASK_TIME_HOUR<<SHIFT_TIME_HOUR);
				sys_time |= i<<SHIFT_TIME_HOUR;
				save_time = 1;
			}
			break;
		case FORM_SETTING_TIME+1:
			i = strToInt(confSetting.input);
			if ((i>=0) && (i<=59)) {
				sys_time &= ~((MASK_TIME_MINUTE<<SHIFT_TIME_MINUTE)|(MASK_TIME_SECOND<<SHIFT_TIME_SECOND));
				sys_time |= i<<SHIFT_TIME_MINUTE;
				save_time = 1;
			}
			break;
		case FORM_SETTING_DELAY:
			i = strToInt(confSetting.input);
			if ((i>=0) && (i<=255)) {
				nvram[NVRAM_DELAY] = i;
			}
			refresh_nvram = 1;
			break;
		case FORM_SETTING_LANG:
			nvram[NVRAM_LANGUAGE] = confSetting.num_list;
			refresh_nvram = 1;
			break;
		case FORM_SETTING_LANG+1:
			nvram[NVRAM_KEYBOARD] = confSetting.num_list;
			refresh_nvram = 1;
			break;
	}

	if (save_date) {
		Tsetdate(sys_date);
	}
	if (save_time) {
		Tsettime(sys_time);
	}
	if (refresh_nvram) {
		readNvram();
		vt_displayForm(form_nvram);
	}

    if (save_date || save_time || refresh_nvram) {
        exit_flag |= EXIT_FLAG_WARM_RESET;
    }
}

static void readClock(void)
{
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

static void readNvram(void)
{
	int i;

	/* Language, keyboard */
	i = nvram[NVRAM_LANGUAGE];
	if (i>NUM_LANG_TOS) {
		i = 0;
	}
	strCopy(lang_tos[i], &form_nvram[FORM_LANG].text[FORM_TEXTPOS]);

	i = nvram[NVRAM_KEYBOARD];
	if (i>=NUM_LANG_KBD) {
		i = 0;
	}
	strCopy(lang_kbd[i], &form_nvram[FORM_LANG+1].text[FORM_TEXTPOS]);

	/* Boot delay */
	format_number(&form_nvram[FORM_DELAY].text[FORM_TEXTPOS], nvram[NVRAM_DELAY], 2, ' ');
}
