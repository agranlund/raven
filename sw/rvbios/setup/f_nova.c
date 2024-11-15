/*
    NOVA setup

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
#include <stdio.h>
#include <mint/osbind.h>
#include <mint/ostruct.h>
#include <mint/falcon.h>
#include <mint/sysvars.h>

#include "../rvnova/rvnova.h"

#include "form_vt.h"
#include "f_nova.h"
#include "misc.h"

#define FORM_X0					2
#define FORM_Y0					2
#define FORM_TEXTPOS			21

#define MAX_DRIVERS				16
#define MAX_MODES				32

#define FORM_SETTING_DRVENABLE	0
#define FORM_SETTING_VDIENABLE	1
#define FORM_SETTING_DRIVER		2
#define FORM_SETTING_BOOTRES	3
#define FORM_SETTING_DESKRES	4
#define FORM_SETTING_GDOS		5
#define FORM_SETTING_GDOSNAME	6


static void exitFormNova(void);
static void confirmFormNova(int num_setting, conf_setting_u confSetting);

typedef struct
{
	char name[14];
} driver_t;

typedef struct
{
	char name[13];
	uint8_t  b;
	uint16_t w;
	uint16_t h;
} mode_t;

int num_drivers;
driver_t driverdata[MAX_DRIVERS];
char* driverlist[MAX_DRIVERS+1];

int num_modes;
mode_t modedata[MAX_MODES];
char* modelist_drv[MAX_MODES+1];
char* modelist_vdi[MAX_MODES+1];

int firstTimeSetup = 1;
const char* path_nova 	= "c:\\nova"; 
const char* path_inf 	= "c:\\auto\\xmenu.inf";
const char* path_bib	= "auto\\sta_vdi.bib";

rvnova_menuinf_t inf;

static form_t form_nova[]={
	{FORM_TEXT, "Nova ............... [x]", 			FORM_X0, FORM_Y0+0},
	{FORM_TEXT, "Nova VDI ........... [x]",				FORM_X0, FORM_Y0+1},

	{FORM_TEXT, "Driver ............. none          ",	FORM_X0, FORM_Y0+3},

	{FORM_TEXT, "Boot resolution .... none          ",	FORM_X0, FORM_Y0+5},
	{FORM_TEXT, "Desk resolution .... none          ",	FORM_X0, FORM_Y0+6},

	{FORM_TEXT, "GDOS ............... [x]", 			FORM_X0, FORM_Y0+8},
	{FORM_TEXT, "Driver .............               ",	FORM_X0, FORM_Y0+9},

	{FORM_END, 0,0,0},
};

form_setting_t form_setting_nova[]={
	{FORM_X0+FORM_TEXTPOS+1, FORM_Y0+0, NULL, SETTING_BOOL, 1},
	{FORM_X0+FORM_TEXTPOS+1, FORM_Y0+1, NULL, SETTING_BOOL, 1},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+3, NULL, SETTING_LIST, 12, driverlist},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+5, NULL, SETTING_LIST, 12, modelist_drv},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+6, NULL, SETTING_LIST, 12, modelist_vdi},
	{FORM_X0+FORM_TEXTPOS+1, FORM_Y0+8, NULL, SETTING_BOOL, 1},
	{FORM_X0+FORM_TEXTPOS+0, FORM_Y0+9, NULL, SETTING_INPUT, 8},
	{0, 0, NULL, SETTING_END}
};

const form_menu_t form_menu_nova={
	displayFormNova,
	updateFormNova,
	initFormNova,
	confirmFormNova,
	NULL, exitFormNova
};


/*--- Functions ---*/

static void exitFormNova(void)
{
	rvnova_saveinf(&inf, path_inf);
}

static void initDrivers(void)
{
	int i;
	_DTA dta;
	_DTA* dtaold;
	char* temp;
	char str[64];

	dtaold = Fgetdta();
	Fsetdta(&dta);

	num_drivers = 0;
	sprintf(str, "%s\\*.*", path_nova);
	if (Fsfirst(str, FA_DIR) == 0) {
		do { if ((dta.dta_attribute & FA_DIR) && (dta.dta_name[0] != '.')) {
			strCopy(dta.dta_name, driverdata[num_drivers].name);
			driverlist[num_drivers] = driverdata[num_drivers].name;
			num_drivers++;
		} } while (Fsnext() == 0);
	}
	driverlist[num_drivers] = 0;

	for (i = 0; i < (num_drivers - 1); i++) {
		if (strCompare(driverlist[i], driverlist[i+1]) > 0) {
			temp = driverlist[i];
			driverlist[i] = driverlist[i+1];
			driverlist[i+1] = temp;
			i = -1;
		}
	}

	if ((inf.drvpath[0]==0) && (num_drivers > 0)) {
		strCopy(driverlist[0], inf.drvpath);
	}

	Fsetdta(dtaold);
}

static mode_t* findMode(uint16_t w, uint16_t h, uint8_t b) {
	int i; int best_i = -1;
	uint16_t diff_w = 0xffff;
	uint16_t diff_h = 0xffff;
	for (i = 0; i < num_modes; i++) {
		if (modedata[i].b == b) {
			uint16_t dw = (w >= modedata[i].w) ? (w - modedata[i].w) : (modedata[i].w - w);
			uint16_t dh = (h >= modedata[i].h) ? (h - modedata[i].h) : (modedata[i].h - h);
			if ((best_i < 0) || (dw < diff_w) || ((dw == diff_w) && (dh < diff_h))) {
				diff_w = dw; diff_h = dh; best_i = i;
			}
		}
	}
	return (best_i >= 0) ? &modedata[best_i] : 0L;
}

static void initResolutions(void)
{
	bib_t bib;
	int i, num_bwmodes;
	char fname[128];
	sprintf(fname, "%s\\%s\\%s", path_nova, inf.drvpath, path_bib); 
	rvnova_loadbib(&bib, fname);
	num_modes = 0; num_bwmodes = 0;
	for (i=0; i<bib.num && i<MAX_MODES; i++) {
		if (bib.res[i].planes == 16 && bib.res[i].colors < 64000U) {
			modedata[num_modes].b = 15;
		} else {
			modedata[num_modes].b = bib.res[i].planes;
		}
		modedata[num_modes].w = bib.res[i].real_x+1;
		modedata[num_modes].h = bib.res[i].real_y+1;
		sprintf(modedata[num_modes].name, "%dx%dx%d", modedata[i].w, modedata[i].h, modedata[i].b);
		modelist_vdi[num_modes] = modedata[num_modes].name;
		if (modedata[num_modes].b == 1) {
			modelist_drv[num_bwmodes] = modedata[num_modes].name;
			num_bwmodes++;
		}
		num_modes++;
	}
	modelist_drv[num_bwmodes] = 0;
	modelist_vdi[num_modes] = 0;
	rvnova_freebib(&bib);

	/* todo: sort resolution lists */

}

static void updateString(char* src, char* dst, int len)
{
	int i;
	for (i = 0; (i < len) && *src; i++) { *dst++ = *src++; }
	for (; i < len; i++) { *dst++ = ' '; }
	*dst = 0;
}

void refreshFormNova(void)
{
	mode_t* mode;
	form_nova[FORM_SETTING_DRVENABLE].text[FORM_TEXTPOS+1] = (inf.drv_enable ? 'x' : ' ');
	form_nova[FORM_SETTING_VDIENABLE].text[FORM_TEXTPOS+1] = (inf.vdi_enable ? 'x' : ' ');

	updateString(inf.drvpath, &form_nova[FORM_SETTING_DRIVER].text[FORM_TEXTPOS], 12);

	mode = findMode(inf.drv_res.w, inf.drv_res.h, inf.drv_res.b);
	if (mode) {
		updateString(mode->name, &form_nova[FORM_SETTING_BOOTRES].text[FORM_TEXTPOS], 12);
	}

	mode = findMode(inf.vdi_res.w, inf.vdi_res.h, inf.vdi_res.b);
	if (mode) {
		updateString(mode->name, &form_nova[FORM_SETTING_DESKRES].text[FORM_TEXTPOS], 12);
	}

	form_nova[FORM_SETTING_GDOS].text[FORM_TEXTPOS+1] = (inf.menuinf.gdos ? 'x' : ' ');
	updateString(inf.menuinf.gdosfile, &form_nova[FORM_SETTING_GDOSNAME].text[FORM_TEXTPOS], 8);
}

void initFormNova(void)
{
	if (firstTimeSetup) {
		if (!rvnova_loadinf(&inf, path_inf)) {
			/* default settings */
			inf.menuinf.gdosfile[0] = 'N';
			inf.menuinf.gdosfile[1] = 'V';
			inf.menuinf.gdosfile[2] = 'D';
			inf.menuinf.gdosfile[3] = 'I';
			inf.vdi_enable = 1;
			inf.drv_res.w = 640;
			inf.drv_res.h = 480;
			inf.drv_res.b = 1;
			inf.vdi_res.w = 640;
			inf.vdi_res.h = 480;
			inf.vdi_res.b = 8;
		}
		/* forced settings */
		inf.menuinf.guikey = 0xf;
		inf.menuinf.output = 1;

		initDrivers();
		initResolutions();
		firstTimeSetup = 0;
	}

	form_setting_nova[FORM_SETTING_DRVENABLE].text = &form_nova[FORM_SETTING_DRVENABLE].text[FORM_TEXTPOS+1];
	form_setting_nova[FORM_SETTING_VDIENABLE].text = &form_nova[FORM_SETTING_VDIENABLE].text[FORM_TEXTPOS+1];
	form_setting_nova[FORM_SETTING_DRIVER].text = &form_nova[FORM_SETTING_DRIVER].text[FORM_TEXTPOS];
	form_setting_nova[FORM_SETTING_BOOTRES].text = &form_nova[FORM_SETTING_BOOTRES].text[FORM_TEXTPOS];
	form_setting_nova[FORM_SETTING_DESKRES].text = &form_nova[FORM_SETTING_DESKRES].text[FORM_TEXTPOS];
	form_setting_nova[FORM_SETTING_GDOS].text = &form_nova[FORM_SETTING_GDOS].text[FORM_TEXTPOS+1];
	form_setting_nova[FORM_SETTING_GDOSNAME].text = &form_nova[FORM_SETTING_GDOSNAME].text[FORM_TEXTPOS];
	refreshFormNova();
}

void displayFormNova(void)
{
	vt_displayForm(form_nova);
}

void updateFormNova(void)
{
}

static void confirmFormNova(int num_setting, conf_setting_u confSetting)
{
	switch(num_setting) {
		case FORM_SETTING_DRVENABLE:
			inf.drv_enable = inf.drv_enable ? 0 : 1;
			break;
		case FORM_SETTING_VDIENABLE:
			inf.vdi_enable = inf.vdi_enable ? 0 : 1;
			break;
		case FORM_SETTING_DRIVER:
			strCopy(driverlist[confSetting.num_list], inf.drvpath);
			initResolutions();
			break;
		case FORM_SETTING_BOOTRES:
			{
				mode_t* mptr = modedata;
				while (mptr) {
					if (mptr->name == modelist_drv[confSetting.num_list]) {
						inf.drv_res.w = mptr->w;
						inf.drv_res.h = mptr->h;
						inf.drv_res.b = mptr->b;
						break;
					}
					mptr++;
				}
			}
			break;
		case FORM_SETTING_DESKRES:
			{
				mode_t* mptr = modedata;
				while (mptr) {
					if (mptr->name == modelist_vdi[confSetting.num_list]) {
						inf.vdi_res.w = mptr->w;
						inf.vdi_res.h = mptr->h;
						inf.vdi_res.b = mptr->b;
						break;
					}
					mptr++;
				}
			}
			break;
		case FORM_SETTING_GDOS:
			inf.menuinf.gdos = inf.menuinf.gdos ? 0 : 1;
			break;
		case FORM_SETTING_GDOSNAME:
			strCopyUpper(confSetting.input, inf.menuinf.gdosfile);
			break;
	}
	refreshFormNova();
	displayFormNova();
}
