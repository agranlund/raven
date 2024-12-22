/*
 * wtest.c - the WERCS test program for Lattice C 5
 *
 * 28/1/90 - Extended to do a PROGDEF AGK
 *
 * This program MUST be compiled with stack checks *off*
 *
 * Copyright (c) 1990 HiSoft
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aes.h>
#include <vdi.h>
#include "wrsc.h"

/* global variables */
OBJECT *menu_ptr;
short screenx,screeny,screenw,screenh;
int radio;
int deskflag,finished=0;
int checked;
char edit[20];
int handle;

int __stdargs __saveds round_button(PARMBLK *pb)
{
	OBJECT *tree;
	int slct, flip;
	short pxy[4];
	
	tree = pb->pb_tree;
	slct = SELECTED & pb->pb_currstate;
	flip = SELECTED & (pb->pb_currstate ^ pb->pb_prevstate);

	pxy[0]=pb->pb_xc;
	pxy[1]=pb->pb_yc;
	pxy[2]=pb->pb_xc+pb->pb_wc-1;
	pxy[3]=pb->pb_yc+pb->pb_hc-1;
	vs_clip(handle,1,pxy);

	pxy[0]=pb->pb_x-2;
	pxy[1]=pb->pb_y-2;
	pxy[2]=pb->pb_x+pb->pb_w-1+4;
	pxy[3]=pb->pb_y+pb->pb_h-1+4;

	if (flip)
	{
		pxy[0]-=1;
		pxy[1]-=1;
		pxy[2]+=1;
		pxy[3]+=1;
	}
	vsf_color(handle, WHITE);
	vsf_style(handle, FIS_SOLID);
	vsf_interior(handle, SOLID);
	vsf_perimeter(handle, 1);
	v_rfbox(handle, pxy);

	if (!flip)
	{
		short junk;

		pxy[0]-=1;
		pxy[1]-=1;
		pxy[2]+=1;
		pxy[3]+=1;
		
		vsl_color(handle, BLACK);
		vsl_type(handle,SOLID);
		v_rbox(handle, pxy);

		vst_alignment(handle,1,5,&junk,&junk);
		vst_color(handle,BLACK);
		v_gtext(handle,pb->pb_x+pb->pb_w/2,pb->pb_y,(char *)pb->pb_parm);
		vst_alignment(handle,0,0,&junk,&junk);
		if (tree[pb->pb_obj].ob_flags & DEFAULT)
		{
			pxy[0]-=4;
			pxy[1]-=4;
			pxy[2]+=4;
			pxy[3]+=4;
	
			vsl_width(handle,3);
			v_rbox(handle, pxy);
			vsl_width(handle,1);
		}
	}
	vsl_color(handle, BLACK);
	return 0;
}

void obj_init(void)
{
	OBJECT *tree;
	static APPLBLK obj[2];
	short junk;
	
	rsrc_gaddr(R_TREE,TestDialog,&tree);
	obj[0].ab_parm=(long)tree[DOK].ob_spec;
	tree[DOK].ob_spec=obj;
	
	obj[1].ab_parm=(long)tree[DCancel].ob_spec;
	tree[DCancel].ob_spec=obj+1;

	obj[0].ab_code = obj[1].ab_code = round_button;

	tree[DOK].ob_type=tree[DCancel].ob_type=G_PROGDEF;

	handle=graf_handle(&junk,&junk,&junk,&junk);
}

void initialise(void)
{
	if (appl_init())
		exit(EXIT_FAILURE);
	if (!rsrc_load("WRSC.RSC"))
	{
		form_alert(1,"[1][Resource file error][ Quit ]");
		exit(EXIT_FAILURE);
	}
	deskflag=0;
	rsrc_gaddr(R_TREE,Menu1,&menu_ptr);
	menu_bar(menu_ptr,1);
	obj_init();
	
	wind_get(DESK,WF_WORKXYWH,&screenx,&screeny,&screenw,&screenh);
	graf_mouse(ARROW,NULL);
	*edit=0;
	radio=DRadio1;
}

void setdesk(OBJECT *newdesk)
{
	wind_newdesk(newdesk,ROOT);
	/* force the AES to redraw the whole screen */
	form_dial(FMD_FINISH,0,0,0,0,screenx,screeny,screenw,screenh);
}

void deinitialise(void)
{
	if (deskflag)
	{
		setdesk(NULL);
		deskflag=0;
	}
	menu_bar(menu_ptr,0);
	rsrc_free();
	appl_exit();
}

int handle_dialog(OBJECT *dlog,int editnum)
{
	short x,y,w,h;
	int but;
	
	form_center(dlog,&x,&y,&w,&h);
	form_dial(FMD_START,0,0,0,0,x,y,w,h);
	form_dial(FMD_GROW,x+w/2,y+h/2,0,0,x,y,w,h);
	objc_draw(dlog,0,10,x,y,w,h);
	but=form_do(dlog,editnum);
	form_dial(FMD_SHRINK,x+w/2,y+h/2,0,0,x,y,w,h);
	form_dial(FMD_FINISH,0,0,0,0,x,y,w,h);
	dlog[but].ob_state&=~SELECTED;	/* de-select exit button */
	return but;
}


/*
 * copy a string into a TEDINFO structure.
 */
void set_tedinfo(OBJECT *tree,int obj,char *source)
{
	char *dest;
	
	dest=((TEDINFO *)tree[obj].ob_spec)->te_ptext;
	strcpy(dest,source);
}

/*
 * copy the string from a TEDINFO into another string
 */
void get_tedinfo(OBJECT *tree, int obj, char *dest)
{
	char *source;

	source=((TEDINFO *)tree[obj].ob_spec)->te_ptext;	/* extract address */
	strcpy(dest,source);
}

void set_button(OBJECT *tree,int parent,int button)
{
	int b;
	
	for (b=tree[parent].ob_head; b!=parent; b=tree[b].ob_next)
		if (b==button)
			tree[b].ob_state|=SELECTED;
		else
			tree[b].ob_state&=~SELECTED;
}

int get_button(OBJECT *tree,int parent)
{
	int b;

	b=tree[parent].ob_head;
	for (; b!=parent && !(tree[b].ob_state&SELECTED); b=tree[b].ob_next)
		;

	return b;
}

void test_dialog(void)
{
	OBJECT *dlog;
	int result;
	
	rsrc_gaddr(R_TREE,TestDialog,&dlog);
	set_tedinfo(dlog,DEditable,edit);
	set_button(dlog,DParent,radio);
	result=handle_dialog(dlog,DEditable);
	if (result==DOK)
	{
		get_tedinfo(dlog,DEditable,edit);
		radio=get_button(dlog,DParent);
	}
}

void handle_menu(int title,int item)
{
	char *aboutstr;

	switch (item)
	{
		case MAbout:
			rsrc_gaddr(R_STRING,AAlert,&aboutstr);
			form_alert(1,aboutstr);
			break;
			
		case MQuit:
			finished=1;
			break;

		case MCheckme:
			checked^=1;
			menu_icheck(menu_ptr,MCheckme,checked);
			break;

		case MDialog:
			test_dialog();
			break;

		case MInstall:
			if (deskflag)
			{
				deskflag=0;
				setdesk(NULL);
			}
			else
			{
				OBJECT *temp;
				
				deskflag=1;
				rsrc_gaddr(R_TREE,NewDesktop,&temp);
				temp[0].ob_x=screenx;
				temp[0].ob_y=screeny;
				temp[0].ob_width=screenw;
				temp[0].ob_height=screenh;
				setdesk(temp);
			}
			break;
	}
	menu_tnormal(menu_ptr,title,1);
}

void handle_events(void)
{
	short mbuff[8];

	for (; !finished;)
	{
		evnt_mesag(mbuff);
		if (mbuff[0]==MN_SELECTED)
		{
			wind_update(BEG_UPDATE);
			handle_menu(mbuff[3],mbuff[4]);
			wind_update(END_UPDATE);
		}
	}
}

int main(void)
{
	initialise();
	handle_events();
	deinitialise();
	return EXIT_SUCCESS;
}
