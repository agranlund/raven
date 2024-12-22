/* MCALC.C

   Copyright (c)  Heimsoeth & Borland  1988

*/


#define MAIN

#include <string.h>
#include <stdarg.h>
#include <tos.h>
#include "mcalc.h"
#include "conio.h"

CELLPTR cell[MAXCOLS][MAXROWS], curcell;
unsigned char format[MAXCOLS][MAXROWS];
unsigned char colwidth[MAXCOLS];
unsigned char colstart[SCREENCOLS];
int leftcol, rightcol, toprow, bottomrow, curcol, currow, lastcol, lastrow;
char changed = FALSE;
char formdisplay = FALSE;
char autocalc = TRUE;
char stop = FALSE;
char snow;
long memleft;
extern int directvideo;

void run(void)
/* The main program loop */
{       int input;
        do
        {       displcell(curcol, currow, HIGHLIGHT, NOUPDATE);
                curcell = cell[curcol][currow];
                showcelltype();
                input = getkey();
                switch(input)
                {       case '/':
                                mainmenu();
                                break;
                        case F1:
                                editcell(curcell);
                                break;
                        case DELKEY:
                                deletecell(curcol, currow, UPDATE);
                                printfreemem();
                                if(autocalc)
                                        recalc();
                                break;
                        case PGUPKEY:
                                toprow -= 20;
                                currow -= 20;
                                if(currow < 0)
                                        currow = toprow = 0;
                                else
                                        if(toprow < 0)
                                        {       currow -= toprow;
                                                toprow = 0;
                                        }
                                setbottomrow();
                                displscreen(NOUPDATE);
                                break;
                        case PGDNKEY:
                                toprow += 20;
                                currow += 20;
                                if((currow >= MAXROWS) && (toprow >= MAXROWS))
                                {       currow = MAXROWS - 1;
                                        toprow = MAXROWS - 20;
                                }
                                else
                                        if(toprow > (MAXROWS - 20))
                                        {       currow -= (toprow + 20 - MAXROWS);
                                                toprow = MAXROWS - 20;
                                        }
                                setbottomrow();
                                displscreen(NOUPDATE);
                                break;
                        case CTRLLEFTKEY:
                                displcell(curcol, currow, NOHIGHLIGHT, NOUPDATE);
                                if(leftcol == 0)
                                        curcol = 0;
                                else
                                {       curcol = rightcol = leftcol - 1;
                                        setleftcol();
                                        setrightcol();
                                        displscreen(NOUPDATE);
                                }
                                break;
                        case CTRLRIGHTKEY:
                                displcell(curcol, currow, NOHIGHLIGHT, NOUPDATE);
                                if(rightcol == MAXCOLS - 1)
                                        curcol = rightcol;
                                else
                                {       curcol = leftcol = rightcol + 1;
                                        setrightcol();
                                        setleftcol();
                                        displscreen(NOUPDATE);
                                }
                                break;
                        case HOMEKEY:
                                currow = curcol = leftcol = toprow = 0;
                                setrightcol();
                                setbottomrow();
                                displscreen(NOUPDATE);
                                break;
                        case ENDKEY:
                                rightcol = curcol = lastcol;
                                currow = bottomrow = lastrow;
                                settoprow();
                                setleftcol();
                                setrightcol();
                                displscreen(NOUPDATE);
                                break;
                        case UPKEY:
                                moverowup();
                                break;
                        case DOWNKEY:
                                moverowdown();
                                break;
                        case LEFTKEY:
                                movecolleft();
                                break;
                        case RIGHTKEY:
                                movecolright();
                                break;
                        default:
                                if((input >= ' ') && (input <= '~'))
                                        getinput(input);
                                break;
                } /* switch */
        }       while(!stop);
} /* run */

int main(int argc, char *argv[])
{       textmode(MONO);
        directvideo = TRUE;
        window(1, 1, 80, 25);
        writef((80 - strlen(MSGHEADER)) >> 1, 11, strlen(MSGHEADER), MSGHEADER);
        writef((80 - strlen(MSGKEYPRESS)) >> 1, 13, strlen(MSGKEYPRESS), MSGKEYPRESS);
        gotoxy(80, 25);
        getkey();
        clrscr();
        initvars();
        memleft = memsize;
        redrawscreen();
        if(argc > 1)
                loadsheet(argv[1]);
        clearinput();
        run();
        return(0);
}
