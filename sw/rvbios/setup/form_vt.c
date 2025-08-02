/*
	VT52 functions

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

#include <mint/osbind.h>
#include "form_vt.h"
#include "misc.h"

/*--- Defines ---*/

#define INPUT_BUF_LEN (1+1+16+1+1)

/*--- Variables ---*/

static char vt_input[INPUT_BUF_LEN];
static char vtStr[5];

static int vt_selected;
static int vt_prev_selected;
static const form_setting_t *form_settings;

static int vt_list_selected;

int FORM_X, FORM_Y, FORM_W, FORM_H;

/*--- Functions ---*/

void vt_init(int x, int y, int w, int h)
{
	FORM_X = x;
	FORM_Y = y;
	FORM_W = w;
	FORM_H = h;
}

void vt_setCursorPos(int x,int y)
{
	vtStr[0]=27;
	vtStr[1]='Y';
	vtStr[2]=32+y;
	vtStr[3]=32+x;
	vtStr[4]=0;
	Cconws(vtStr);
}

void vt_setBgColor(int col)
{
	vtStr[0]=27;
	vtStr[1]='c';
	vtStr[2]=col;
	vtStr[3]=0;
	Cconws(vtStr);
}

void vt_setFgColor(int col)
{
	vtStr[0]=27;
	vtStr[1]='b';
	vtStr[2]=col;
	vtStr[3]=0;
	Cconws(vtStr);
}

const char *vt_readString(void)
{
	int i, length;

	length = form_settings[vt_selected].length;

	if (length<1) {
		length=1;
	}
	if (length>16) {
		length=16;
	}

	for (i=0; i<INPUT_BUF_LEN; i++) {
		vt_input[i]=0;
	}
	vt_input[0] = length+1;

	Cconrs((_CCONLINE*)vt_input);

	/* Put NULL to terminate string */
	vt_input[2+vt_input[1]]=0;

	return &vt_input[2];
}

void vt_clearForm(void)
{
	int i;

	vt_setBgColor(COL_FORM_BG);
	for (i=0; i<FORM_H; i++) {
		vt_setCursorPos(FORM_X,FORM_Y+i);
		Cconws(DEL_EOL);
	}
}

void vt_displayForm_idx(form_t *form, int start, int count)
{
	int i=0;
	vt_setBgColor(COL_FORM_BG);
	while (form->flag != FORM_END)
	{
		if ((i>=start) && (i<start+count))
		{
			switch (form->flag)
			{
				case FORM_TITLE:
					vt_setFgColor(COL_FORM_TITLE);
					vt_setCursorPos(FORM_X+form->posx, FORM_Y+form->posy);
					Cconws(form->text);
					break;

				case FORM_FOOTER:
					vt_setFgColor(COL_FORM_FG);
					vt_setCursorPos(FORM_X+form->posx, FORM_Y+FORM_H-1-form->posy);
					Cconws(form->text);
					break;

				case FORM_TEXT:
					vt_setFgColor(COL_FORM_FG);
					vt_setCursorPos(FORM_X+form->posx, FORM_Y+form->posy);
					Cconws(form->text);
					break;
			}
		}
		form++;
		i++;
	}
}

void vt_displayForm(form_t *form)
{
	vt_displayForm_idx(form, 0, 1000);
}


void vt_initSettings(const form_setting_t *settings)
{
	form_settings = settings;
	vt_selected = 0;
	vt_prev_selected = -1;
}

void vt_setting_prev(void)
{
    int newselection;
	if (!form_settings || (vt_selected == 0)) {
		return;
    }

    for (newselection = (vt_selected - 1); newselection >= 0; newselection--) {
        if (form_settings[newselection].input != SETTING_HIDDEN) {
            vt_selected = newselection;
            return;
        }
    }
}

void vt_setting_next(void)
{
    int newselection;
	if (!form_settings) {
		return;
	}

    newselection = vt_selected + 1;
    while (1) {
        if (form_settings[newselection].input == SETTING_END) {
            return;
        }
        else if (form_settings[newselection].input != SETTING_HIDDEN) {
            vt_selected = newselection;
            return;
        }
        newselection++;
    }
}

void vt_setting_prevRow(void)
{
    int cur_row, newselection;
	if (!form_settings || (vt_selected == 0)) {
		return;
	}

    cur_row = form_settings[vt_selected].posy;
    for (newselection = (vt_selected - 1); newselection >= 0; newselection--) {
        if ((form_settings[newselection].posy != cur_row) && (form_settings[newselection].input != SETTING_HIDDEN)) {
            vt_selected = newselection;
            return;
        }
    }
}

void vt_setting_nextRow(void)
{
	int cur_row, newselection;
    if (!form_settings) {
		return;
	}

	cur_row = form_settings[vt_selected].posy;
    newselection = vt_selected + 1;
    while (1) {
        if (form_settings[newselection].input == SETTING_END) {
            return;
        }
        else if ((form_settings[newselection].posy != cur_row) && (form_settings[newselection].input != SETTING_HIDDEN)) {
            vt_selected = newselection;
            return;
        }
        newselection++;
    }
}

int vt_setting_getType(void)
{
	return form_settings[vt_selected].input;
}

static void vt_setting_display(int selected, int reverse_video)
{
	int len,i;
	char *str;

	if (!form_settings) {
		return;
	}

	vt_setCursorPos(FORM_X+form_settings[selected].posx, FORM_Y+form_settings[selected].posy);

	len = form_settings[selected].length;
	switch(form_settings[selected].input) {
		case SETTING_BOOL:
			len = 1;
			break;
		case SETTING_FUNC:
			len = strLength(form_settings[selected].text);
			break;
	}

	if (len==0) {
		return;
	}

	str = form_settings[selected].text;
	if (reverse_video) {
		Cconws(REV_ON);
	}
	for (i=0; i<len; i++) {
		char c[2]={0,0};

		c[0] = *str++;
		Cconws(c);
	}
	if (reverse_video) {
		Cconws(REV_OFF);
	}

	vt_setCursorPos(FORM_X+form_settings[selected].posx, FORM_Y+form_settings[selected].posy);
}

void vt_setting_clear(void)
{
	vt_setBgColor(COL_FORM_BG);
	vt_setFgColor(COL_FORM_FG);
	if (vt_prev_selected>=0) {
		vt_setting_display(vt_prev_selected, 0);
    }
}

void vt_setting_show(void)
{
	vt_setBgColor(COL_FORM_BG);
	vt_setFgColor(COL_FORM_FG);
	if ((vt_prev_selected>=0) && (vt_prev_selected!=vt_selected)) {
		vt_setting_display(vt_prev_selected, 0);
	}
	vt_setting_display(vt_selected, 1);
	vt_prev_selected = vt_selected;
}

/* Record new value entered */
void vt_setting_newValue(const form_menu_t *form_menu, const char *str)
{
	conf_setting_u confSetting;

	if (!form_settings) {
		return;
	}

	switch(form_settings[vt_selected].input) {
		case SETTING_INPUT:
		case SETTING_BOOL:
			if (form_menu->confirm) {
				confSetting.input = str;
				form_menu->confirm(vt_selected, &confSetting);
			}
			break;
		case SETTING_LIST:
			if (form_menu->confirm) {
				confSetting.num_list = vt_list_selected;
				form_menu->confirm(vt_selected, &confSetting);
			}
			break;
		case SETTING_UPDOWN:
			if (form_menu->confirm) {
				confSetting.num_list = vt_list_selected;
				form_menu->confirm(vt_selected, &confSetting);
			}
			break;
	}
}

void vt_setting_execFunc(void)
{
	setting_func_t  settingFunc;

	if (!form_settings) {
		return;
	}

	settingFunc = (setting_func_t) form_settings[vt_selected].param;
	settingFunc();
}

void vt_setting_listInit(void)
{
	vt_list_selected = 0;
}

void vt_setting_listPrint(void)
{
	const char **setting_list = (const char **) form_settings[vt_selected].param;

	if (setting_list) {
		Cconws(setting_list[vt_list_selected]);
	}
}

void vt_setting_listPrev(void)
{
	if (vt_list_selected>0) {
		--vt_list_selected;
	}
}

void vt_setting_listNext(void)
{
	const char **setting_list = (const char **) form_settings[vt_selected].param;

	if (setting_list) {
		if (setting_list[vt_list_selected+1]) {
			++vt_list_selected;
		}
	}
}

void vt_setting_updown(int dir)
{
	setting_updown_t  settingFunc;

	if (!form_settings) {
		return;
	}

	settingFunc = (setting_updown_t) form_settings[vt_selected].param;
	settingFunc(dir);
}
