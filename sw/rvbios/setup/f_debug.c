/*
	DEBUG menu

	Copyright (C) 2026	Anders Granlund

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

void funcStartMonitor_screen(void);
void funcStartMonitor_serial(void);
void funcStartSafeMode(void);

void displayFormDebug(void);
void updateFormDebug(void);
void initFormDebug(void);
void confirmFormDebug(int num_setting, conf_setting_u* confSetting);
void exitFormDebug(void);
void refreshFormDebug(void);



typedef enum {
	FORM_MONITOR_SCREEN=0,
	FORM_MONITOR_SERIAL,
/*    FORM_SAFEMODE,*/
	FORM_MAX,
};

typedef enum {
	FORM_SETTING_MONITOR_SCREEN,
	FORM_SETTING_MONITOR_SERIAL,
/*	FORM_SETTING_SAFEMODE,*/
	FORM_DEBUG_MAX
};

#define FORM_X0         2
#define FORM_Y0			2
#define FORM_X1			39
#define FORM_TEXTPOS	21

form_t form_debug[]={
	{FORM_TEXT, "Launch Monitor (screen)", FORM_X0, FORM_Y0+0},
	{FORM_TEXT, "Launch Monitor (serial)", FORM_X0, FORM_Y0+2},
/*	{FORM_TEXT, "Boot Safemode",    FORM_X0, FORM_Y0+2},*/
	{FORM_END, 0,0,0}
};

form_setting_t form_setting_debug[]={
	{FORM_X0, FORM_Y0+0, NULL, SETTING_FUNC, 1, funcStartMonitor_screen},
	{FORM_X0, FORM_Y0+2, NULL, SETTING_FUNC, 1, funcStartMonitor_serial},
/*    {FORM_X0, FORM_Y0+2, NULL, SETTING_FUNC, 1, funcStartSafeMode},*/
	{0, 0, NULL, SETTING_END}
};


const form_menu_t form_menu_debug = {
	displayFormDebug,
	updateFormDebug,
	initFormDebug,
	confirmFormDebug,
	NULL, exitFormDebug
};


static void loadSettings(void) {
}

static void saveSettings(void) {
}


void refreshFormDebug(void)
{
}

void displayFormDebug(void)
{
	vt_displayForm(form_debug);
}

void updateFormDebug(void)
{
}

void initFormDebug(void)
{
	loadSettings();		
    form_setting_debug[FORM_SETTING_MONITOR_SCREEN].text = &form_debug[FORM_MONITOR_SCREEN].text[0];
    form_setting_debug[FORM_SETTING_MONITOR_SERIAL].text = &form_debug[FORM_MONITOR_SERIAL].text[0];
/*	form_setting_debug[FORM_SETTING_SAFEMODE].text = &form_debug[FORM_SAFEMODE].text[0];*/
	refreshFormDebug();
}

void exitFormDebug(void)
{
    saveSettings();
}

void confirmFormDebug(int num_setting, conf_setting_u* confSetting)
{
    (void)num_setting;
	(void)confSetting;
	refreshFormDebug();
	vt_displayForm_idx(form_debug, 0, FORM_MAX);
}


static void cdecl (*mon_putc_old)(int32_t);
static int32_t cdecl (*mon_getc_old)(void);

static void cdecl mon_putc(int32_t c) {
    if (c == '\n') { Cconout('\r'); }
    Cconout((int16_t)c);
}
static int32_t cdecl mon_getc(void) {
    while(Bconstat(2) != -1);
    return (Bconin(2) & 0xff);
}

extern void display_restore(void);
extern void font_setsize(uint16_t idx);

void funcStartMonitor(int16_t screen) {
    Cconws(CLEAR_HOME "\r\n");
    if (screen) {
        font_setsize(1);
        vt_setFgColor(COL_BLACK);
        vt_setBgColor(COL_WHITE);
        Cconws(CLEAR_HOME "\r\n");
        Cconws(C_ON);
        mon_getc_old = *(raven()->mon_fgetchar);
        mon_putc_old = *(raven()->mon_fputchar);
        *(raven()->mon_fgetchar) = mon_getc;
        *(raven()->mon_fputchar) = mon_putc;
    } else {
        Cconws("Raven monitor on serial port COM1\r\n");
    }

    raven()->mon_Exec("");

    if (screen) {
        *(raven()->mon_fgetchar) = mon_getc_old;
        *(raven()->mon_fputchar) = mon_putc_old;
    }

    display_restore();
    displayFormDebug();
}

void funcStartMonitor_screen(void) {
    funcStartMonitor(1);
}

void funcStartMonitor_serial(void) {
    funcStartMonitor(0);
}

void funcStartSafeMode(void) {
    /* todo: disable cache */
    /* todo: flag safemode boot */
    exit_state = SETUP_EXIT;
    exit_flag = 0;
}
