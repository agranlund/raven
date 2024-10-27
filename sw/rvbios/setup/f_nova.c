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

#include <mint/osbind.h>
#include <mint/falcon.h>
#include <mint/sysvars.h>

#include "form_vt.h"
#include "f_nova.h"
#include "misc.h"

#define FORM_X0					2
#define FORM_Y0					2
#define FORM_TEXTPOS			21


/*--- Defines ---*/


/*--- Types ---*/

typedef struct {
	unsigned short vmode;
	unsigned short width, height;
} videomode_t;

/*--- Const ---*/


/*--- Functions prototypes ---*/

static void confirmFormNova(int num_setting, conf_setting_u confSetting);


/*--- Variables ---*/

static form_t form_nova[]={
	{FORM_TEXT, "Nova ............... [x]", 			FORM_X0, FORM_Y0+0},
	{FORM_TEXT, "Nova VDI ........... [x]",				FORM_X0, FORM_Y0+1},
	{FORM_TEXT, "Driver ............. ET4KW32I", 		FORM_X0, FORM_Y0+2},

	{FORM_TEXT, "Boot resolution .... 960x540x1", 		FORM_X0, FORM_Y0+4},
	{FORM_TEXT, "Desk resolution .... 1280x720x8", 		FORM_X0, FORM_Y0+5},

	{FORM_TEXT, "BDOS ............... [x]", 			FORM_X0, FORM_Y0+7},
	{FORM_TEXT, "Driver ............. NVDI.PRG", 		FORM_X0, FORM_Y0+8},

	{FORM_END, 0,0,0},
};

form_setting_t form_setting_nova[]={
	{0, 0, NULL, SETTING_END},
	{0, 0, NULL, SETTING_END}
};

const form_menu_t form_menu_nova={
	displayFormNova,
	updateFormNova,
	initFormNova,
	confirmFormNova
};


/*--- Functions ---*/

void initFormNova(void)
{
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

}
