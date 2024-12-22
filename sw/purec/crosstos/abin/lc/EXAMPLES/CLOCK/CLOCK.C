/*
 * clock.c  - simple clock in a window Desk Accessory
 *
 * Note: When NOT_A_DA is defined the program is built as a true program,
 * but if optimising some unused assignments will be reported.
 */

#include <aes.h>
#include <stdio.h>
#include <time.h>
#include "rclock.h"
#include "rclock.c"

#ifndef NOT_A_DA
#include <acc.h>

STACK(1024);	// hopefully plenty
#endif

#define W_TYPE (NAME | MOVER | CLOSER)

int
new_window(OBJECT *tree, const char *title)
{
	int wh;
	GRECT p;

	// compute required size for window given object tree
	wind_calc(WC_BORDER, W_TYPE, PTRS((GRECT *)&tree[ROOT].ob_x),
	  &p.g_x, &p.g_y, &p.g_w, &p.g_h);

	wh = wind_create(W_TYPE, ELTS(p));
	if (wh >= 0) {
		wind_title(wh, title);
		wind_open(wh, ELTS(p));
	}
	return wh;
}

int
redraw(int wh,GRECT *p)
{
	objc_draw(rclock,ROOT,MAX_DEPTH,PTRS(p));
	return 1;
}

#ifdef NOT_A_DA
int
#else
void
#endif
main(void)
{
	int id, wh = -1;
	GRECT full;
	
	id = appl_init();

	rsrc_init();
	wind_get(DESK, WF_WXYWH, &full.g_x, &full.g_y, &full.g_w, &full.g_h);
	rc_constrain(&full, (GRECT *)&rclock[ROOT].ob_x);
#ifndef NOT_A_DA
	menu_register(id, clock_menu);	// register as a DA
#else
	wh = new_window(rclock, clock_title);
#endif

	for (;;) {
		short msg[8], junk;
		int which;
		
		which = evnt_multi(MU_TIMER | MU_MESAG,
		  0, 0, 0,			// mouses
		  0, 0, 0, 0, 0,	// rectangle 1
		  0, 0, 0, 0, 0,	// rectangle 2
		  msg,				// message buffer
		  2000, 0,			// respond every 2s
		  &junk, &junk, &junk, &junk, &junk, &junk);

		if (which & MU_MESAG)
			switch (msg[0]) {
#ifndef NOT_A_DA
				case AC_OPEN:
					if (wh < 0)
						wh = new_window(rclock, clock_title);
					else
						wind_set(wh, WF_TOP, wh);
					break;

				case AC_CLOSE:
					wh = -1;
					break;
#endif
				case WM_TOPPED:
					wind_set(msg[3], WF_TOP, msg[3]);
					break;

				case WM_CLOSED:
					wind_close(wh);
					wind_delete(wh);
					wh = -1;
					break;

				case WM_REDRAW:
					wind_redraw(msg[3], (GRECT *)&msg[4], redraw);
					break;

				case WM_MOVED:
					wind_set(wh, WF_CXYWH, PTRS((GRECT *)&msg[4]));
					wind_calc(WC_WORK, W_TYPE, PTRS((GRECT *)&msg[4]),
					  &rclock[ROOT].ob_x,
					  &rclock[ROOT].ob_y,
					  &rclock[ROOT].ob_width,
					  &rclock[ROOT].ob_height);
					break;
			}
		
		if (which & MU_TIMER) {
			short top;
			
			wind_get(DESK, WF_TOP, &top, &junk, &junk, &junk);
			if (top == wh) {	// may only safely call GEMDOS if on top
				time_t t;
				struct tm *tm;
				
				time(&t);
				tm = localtime(&t);
				sprintf(rclock[TIME].ob_spec,"%02d:%02d:%02d",
				  tm->tm_hour,
				  tm->tm_min,
				  tm->tm_sec);
				*(GRECT *)&msg[4] = *(GRECT *)&rclock[ROOT].ob_x;
				wind_redraw(msg[3], (GRECT *)&msg[4], redraw);
			}
		}
#ifdef NOT_A_DA
		if (wh < 0)
			break;
#endif
	}
#ifdef NOT_A_DA
	appl_exit();
	return 0;
#endif
}
