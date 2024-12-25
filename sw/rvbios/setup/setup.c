/*
	Setup GUI

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
#include <linea.h>

#include <mint/osbind.h>

#include "form_vt.h"
#include "f_nvram.h"
#include "f_nova.h"
#include "f_cpu.h"
#include "f_exit.h"
#include "misc.h"
#include "../rvbios.h"

/*--- Defines ---*/

#define NUM_MENU_ENTRIES 7

enum {
	STATE_MENU=0,		/* Selecting a form in left menu */
	STATE_FORM_SELECT,	/* Selecting a parameter to edit */
	STATE_FORM_INPUT,	/* Input a string */
	STATE_FORM_LIST,	/* List selection */
	STATE_FORM_UPDOWN	/* Selection using up/down calculation */
};

/*--- Types ---*/

typedef struct {
	const char	*name;
	const form_menu_t	*form;
	const form_setting_t	*settings;
} menu_t;

/*--- Variables ---*/

static const form_menu_t form_menu_empty={
	vt_clearForm,
	NULL
};

static const menu_t menu[NUM_MENU_ENTRIES]={
	{"NVRAM ",	    &form_menu_nvram,       &form_setting_nvram[0]},
	{"",	        &form_menu_empty,       NULL},
	{"SYSTEM",	    &form_menu_cpu,         &form_setting_cpu[0]},
	{"",	        &form_menu_empty,       NULL},
	{"NOVA  ",      &form_menu_nova,       	&form_setting_nova[0]},
	{"",	        &form_menu_empty,       NULL},	
	{"Exit  ",	    &form_menu_exit,        &form_setting_exit[0]},
};

static int stat_refresh = 1;
static int menu_refresh = 1;
static int form_refresh = 1;
static int setup_state = STATE_MENU;
static int menu_row = 0;

static int SCR_W = 80;
static int SCR_H = 29;

static int STAT_X, STAT_Y, STAT_H;
static int MENU_X, MENU_Y;

/*--- Functions prototypes ---*/

static void display_banner(void);
static void display_menu(void);
static void display_status(void);

static void setup_menu(unsigned long key_pressed);
static void setup_form_select(unsigned long key_pressed);
static void setup_list_select(unsigned long key_pressed);
static void setup_updown_select(unsigned long key_pressed);


extern void reset_warm(void);
extern void reset_cold(void);

/*--- Functions ---*/

int setup_main(void)
{

	SCR_W = Vdiesc->v_cel_mx;
	SCR_H = Vdiesc->v_cel_my;

	vt_init(11, 1, SCR_W-11, SCR_H-2);

	STAT_X = FORM_X+1;
	STAT_Y = SCR_H-1;
	STAT_H = 1;

	MENU_X = 0;
	MENU_Y = 1;

    /* init screen */
	vt_setFgColor(COL_BLACK);
	vt_setBgColor(COL_WHITE);

    Cconws(C_OFF CLEAR_HOME);
	display_banner();
	vt_initSettings(NULL);

    exit_state = SETUP_CONTINUE;
    exit_flag = EXIT_FLAG_NORMAL;

	while (exit_state == SETUP_CONTINUE) {
		unsigned long key_pressed;

		/* Refresh menu, form, and status line */
        if (stat_refresh) {
		    display_status();
            stat_refresh = 0;
        }

		if (menu_refresh) {
			display_menu();
			menu_refresh = 0;
			form_refresh = 1;
			if (menu[menu_row].form->init) {
				menu[menu_row].form->init();
			}
			vt_initSettings(menu[menu_row].settings);
		}
		if (form_refresh) {
			vt_clearForm();
			menu[menu_row].form->display();
			form_refresh = 0;
		}

		/* Wait key press */
		while (Cconis()==0) {
			Vsync();

			if (menu[menu_row].form->update) {
				menu[menu_row].form->update();
			}

			/* Replace cursor on setting */
			if (setup_state == STATE_FORM_SELECT) {
				vt_setting_show();
			}
		}

		key_pressed = Cnecin();

		switch(setup_state) {
			case STATE_MENU:
				setup_menu(key_pressed);
				break;
			case STATE_FORM_SELECT:
				setup_form_select(key_pressed);
				break;
			case STATE_FORM_LIST:
				setup_list_select(key_pressed);
				break;
			case STATE_FORM_UPDOWN:
				setup_updown_select(key_pressed);
				break;
		}
	}



    if (exit_flag & EXIT_FLAG_COLD_RESET) {
        reset_cold();
    } else if (exit_flag & EXIT_FLAG_WARM_RESET) {
        /* reset_warm(); */
        reset_cold(); 
    }

    /* restore screen */
/*
	vt_setFgColor(COL_BLACK);
	vt_setBgColor(COL_WHITE);
	Cconws(CLEAR_HOME "\r\n");
*/

#if 0
#ifdef SETUP_STANDALONE
	cpufreq_changed = 0;
#endif
	if ((exit_type == SETUP_RESET) || cpufreq_changed) {
		Super(0);

		__asm__ __volatile__(
			"jmp\t0xe00030"
		);
	}
#endif

	return 1;
}

static void display_banner(void)
{
    int i;

    /* clear screen, cursor off */
	vt_setFgColor(COL_BLACK);
	vt_setBgColor(COL_WHITE);
    Cconws(CLEAR_HOME C_OFF);

    /* top banner */
	vt_setFgColor(COL_BANNER_FG);
	vt_setBgColor(COL_BANNER_BG);
    vt_setCursorPos(0, 0);
    Cconws(DEL_EOL);
	vt_setCursorPos((SCR_W-8)>>1,0);
	Cconws("RAVEN060");

    /* bottom banner */
	vt_setFgColor(COL_BANNER_FG);
	vt_setBgColor(COL_BANNER_BG);
    for (i=STAT_Y; i<STAT_Y+STAT_H; i++) {
        vt_setCursorPos(0, STAT_Y+i);
        Cconws(DEL_EOL);
    }

    /* left banner */
	vt_setFgColor(COL_MENU_FG);
	vt_setBgColor(COL_MENU_BG);
    for (i=1; i<STAT_Y; i++) {
        vt_setCursorPos(FORM_X-1,i);
        Cconws(DEL_BOL);
    }
}

static void display_menu(void)
{
	int i;
    for (i=0; i<NUM_MENU_ENTRIES; i++) {
        if (menu[i].form != &form_menu_empty) {
            if (i==menu_row) {
                vt_setFgColor(COL_MENU_FG_SEL);
                vt_setBgColor(COL_MENU_BG_SEL);
            } else {
                vt_setFgColor(COL_MENU_FG);
                vt_setBgColor(COL_MENU_BG);
            }
            vt_setCursorPos(FORM_X-1,MENU_Y+i);
            Cconws(DEL_BOL);
            vt_setCursorPos(MENU_X,MENU_Y+i);
            Cconws(menu[i].name);
        }
    }
}

static void display_status(void)
{
	vt_setFgColor(COL_BANNER_FG);
	vt_setBgColor(COL_BANNER_BG);
	vt_setCursorPos(0,STAT_Y);
    Cconws(DEL_EOL);
	vt_setCursorPos(STAT_X,STAT_Y);
	switch(setup_state) {
		case STATE_MENU:
			Cconws(CLEAR_DOWN "ARROWS: Navigate, ENTER: Select");
			vt_setCursorPos(SCR_W-10, STAT_Y);
			Cconws("F10: Exit");
			break;
		case STATE_FORM_SELECT:
			Cconws(CLEAR_DOWN "ARROWS: Navigate, ENTER: Select, ESC: Menu");
			vt_setCursorPos(SCR_W-10, STAT_Y);
			Cconws("F10: Exit");
			break;
		case STATE_FORM_INPUT:
			Cconws(CLEAR_DOWN "Enter new value: ");
			break;
		case STATE_FORM_LIST:
			Cconws(CLEAR_DOWN "Select new value: ");
			vt_setting_listPrint();
			break;
		case STATE_FORM_UPDOWN:
			Cconws(CLEAR_DOWN "Select new value: ");
			vt_setting_updown(SETTING_DIR_PRINT);
			break;
	}
}

static void setup_menu(unsigned long key_pressed)
{
/*	unsigned char asciicode = key_pressed & 0xff; */
	unsigned char scancode = (key_pressed >> 16) & 0xff;

	switch(scancode) {
        case SCANCODE_F10:
			exit_state = SETUP_EXIT;
            stat_refresh = 1;
			break;
		case SCANCODE_UP:
            if (menu_row > 0) {
                while (1) {
                    if (menu_row==0)
                        break;
                    --menu_row;
                    if (menu[menu_row].form != &form_menu_empty)
                        break;
                }
                menu_refresh = 1;
            }
			break;
		case SCANCODE_DOWN:
            if (menu_row<(NUM_MENU_ENTRIES-1)) {
                while (1) {
                    if (menu_row==(NUM_MENU_ENTRIES-1))
                        break;
                    ++menu_row;
                    if (menu[menu_row].form != &form_menu_empty)
                        break;
                }
                menu_refresh = 1;
            }
			break;
		case SCANCODE_SPACE:
		case SCANCODE_ENTER:
		case SCANCODE_KP_ENTER:
			if (menu[menu_row].settings) {
				setup_state = STATE_FORM_SELECT;
                stat_refresh = 1;
			}
			if (menu[menu_row].form->enter) {
				menu[menu_row].form->enter();
			}
			break;
	}
}

static void setup_form_select(unsigned long key_pressed)
{
/*	unsigned char asciicode = key_pressed & 0xff; */
	unsigned char scancode = (key_pressed >> 16) & 0xff;

	switch(scancode) {
		case SCANCODE_F10:
			exit_state = SETUP_EXIT;
		case SCANCODE_ESCAPE:
            vt_setting_clear();
			setup_state = STATE_MENU;
            stat_refresh = 1;
			if (menu[menu_row].form->exit) {
				menu[menu_row].form->exit();
			}
			break;
		case SCANCODE_UP:	/* First parameter on previous row */
			vt_setting_prevRow();
			break;
		case SCANCODE_DOWN:	/* First parameter on next row */
			vt_setting_nextRow();
			break;
		case SCANCODE_LEFT:	/* Previous parameter on same row */
			vt_setting_prev();
			break;
		case SCANCODE_RIGHT:	/* Next parameter on same row */
			vt_setting_next();
			break;
		case SCANCODE_SPACE:
		case SCANCODE_ENTER:
		case SCANCODE_KP_ENTER:
			switch(vt_setting_getType()) {
				case SETTING_INPUT:
					{
						const char *strInput;

						setup_state = STATE_FORM_INPUT;
						display_status();


						Cconws(C_ON);
						strInput = vt_readString();
						Cconws(C_OFF);

						vt_setting_newValue(menu[menu_row].form, strInput);
						setup_state = STATE_FORM_SELECT;
                        stat_refresh = 1;
					}
					break;
				case SETTING_FUNC:
					vt_setting_execFunc();
					break;
				case SETTING_BOOL:
					vt_setting_newValue(menu[menu_row].form, NULL);
					break;
				case SETTING_LIST:
					setup_state = STATE_FORM_LIST;
                    stat_refresh = 1;
					vt_setting_listInit();
					break;
				case SETTING_UPDOWN:
					setup_state = STATE_FORM_UPDOWN;
                    stat_refresh = 1;
					break;
			}
			break;
	}
}

static void setup_list_select(unsigned long key_pressed)
{
/*	unsigned char asciicode = key_pressed & 0xff; */
	unsigned char scancode = (key_pressed >> 16) & 0xff;

	switch(scancode) {
		case SCANCODE_ESCAPE:
			setup_state = STATE_FORM_SELECT;
            stat_refresh = 1;
			break;
		case SCANCODE_UP:
		case SCANCODE_LEFT:
			vt_setting_listPrev();
            stat_refresh = 1;
			break;
		case SCANCODE_DOWN:
		case SCANCODE_RIGHT:
			vt_setting_listNext();
            stat_refresh = 1;
			break;
		case SCANCODE_SPACE:
		case SCANCODE_ENTER:
		case SCANCODE_KP_ENTER:
			vt_setting_newValue(menu[menu_row].form, NULL);
			setup_state = STATE_FORM_SELECT;
            stat_refresh = 1;
			break;
	}
}

static void setup_updown_select(unsigned long key_pressed)
{
/*	unsigned char asciicode = key_pressed & 0xff; */
	unsigned char scancode = (key_pressed >> 16) & 0xff;

	switch(scancode) {
		case SCANCODE_ESCAPE:
			setup_state = STATE_FORM_SELECT;
            stat_refresh = 1;
			break;
		case SCANCODE_UP:
		case SCANCODE_LEFT:
			vt_setting_updown(SETTING_DIR_UP);
            stat_refresh = 1;
			break;
		case SCANCODE_DOWN:
		case SCANCODE_RIGHT:
			vt_setting_updown(SETTING_DIR_DOWN);
            stat_refresh = 1;
			break;
		case SCANCODE_SPACE:
		case SCANCODE_ENTER:
		case SCANCODE_KP_ENTER:
			vt_setting_newValue(menu[menu_row].form, NULL);
			setup_state = STATE_FORM_SELECT;
            stat_refresh = 1;
			break;
	}
}
