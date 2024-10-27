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
#include "form_nova.h"
#include "misc.h"

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
	/* Static info */
	{FORM_TITLE, "NOVA", FORM_X+((FORM_W-5)>>1), FORM_Y},
	{FORM_END, 0,0,0}
};

form_setting_t form_setting_nova[]={
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
