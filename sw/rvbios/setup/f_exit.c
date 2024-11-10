/*
    Exit

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

#include "form_vt.h"
#include "f_exit.h"

/*--- Defines ---*/

#define FORM_CONTINUE 0
#define FORM_RESET (FORM_CONTINUE+1)

#define FORM_SETTING_CONTINUE 0
#define FORM_SETTING_RESET (FORM_SETTING_CONTINUE+1)

/*--- Global variables ---*/

unsigned char exit_type;

/*--- Const ---*/

static void exitContinue(void);
static void exitReset(void);

static form_t form_exit[]={
/*
	{FORM_TEXT, "Exit and continue booting", 2,2},
	{FORM_TEXT, "Exit and reset", 2,3},
*/
	{FORM_END, 0,0,0}
};

form_setting_t form_setting_exit[]={
/*
	{2,2, NULL, SETTING_FUNC, 0, exitContinue},
	{2,3, NULL, SETTING_FUNC, 0, exitReset},
*/
	{0, 0, NULL, SETTING_END}
};

/*--- Variables ---*/

static void initFormExit(void);

const form_menu_t form_menu_exit={
	displayFormExit,
	NULL,
	initFormExit,
	NULL,
	exitContinue, NULL
};

/*--- Functions ---*/

static void initFormExit(void)
{
/*
	form_setting_exit[FORM_SETTING_CONTINUE].text = &form_exit[FORM_CONTINUE].text[0];
	form_setting_exit[FORM_SETTING_RESET].text = &form_exit[FORM_RESET].text[0];
*/
}

void displayFormExit(void)
{
	vt_displayForm(form_exit);
}

void exitContinue(void)
{
	exit_type = SETUP_EXIT;
}

void exitReset(void)
{
	exit_type = SETUP_RESET;
}
