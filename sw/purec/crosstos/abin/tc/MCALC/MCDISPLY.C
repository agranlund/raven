/* MCDISPLY.C

   Copyright (c)  Heimsoeth & Borland  1988

*/


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "mcalc.h"
#include "conio.h"

void writef(int col, int row, int width, va_list arg_list, ...)
/* Prints a string in video memory at a selected location */
{       va_list arg_ptr;
        char *format;
        char output[81];
        int len;
        va_start(arg_ptr, arg_list);
        format = arg_list;
        vsprintf(output, format, arg_ptr);
        output[width] = '\0';
        if((len = strlen(output)) < width)
                memset(&output[len], ' ', width - len);
        gotoxy(col, row);
        couts(output);
} /* writef */

void scroll(int direction, int lines, int x1, int y1, int x2, int y2)
/* Scrolls an area of the screen */
{       if(lines == 0)
                cwindfill(x1, y1, x2, y2);
        else
                switch(direction)
                {       case UP:
                                movetext(x1, y1 + lines, x2, y2, x1, y1);
                                cwindfill(x1, y2 - lines + 1, x2, y2);
                                break;
                        case DOWN:
                                movetext(x1, y1, x2, y2 - lines, x1, y1 + lines);
                                cwindfill(x1, y1, x2, y1 + lines - 1);
                                break;
                        case LEFT:
                                movetext(x1 + lines, y1, x2, y2, x1, y1);
                                cwindfill(x2 - lines + 1, y1, x2, y2);
                                break;
                        case RIGHT:
                                movetext(x1, y1, x2 - lines, y2, x1 + lines, y1);
                                cwindfill(x1, y1, x1 + lines - 1, y2);
                                break;
                } /* switch */
} /* scroll */

void printcol(void)
/* Prints the column headings */
{       int col;
        char colstr[MAXCOLWIDTH + 1];
        scroll(UP, 0, 1, 2, 80, 2);
        for(col = leftcol; col <= rightcol; col++)
        {       centercolstring(col, colstr);
                writef(colstart[col - leftcol] + 1, 2, colwidth[col], colstr);
        }
} /* printcol */

void clearlastcol()
/* Clears any data left in the last column */
{       int col;
        if((col = colstart[rightcol - leftcol] + colwidth[rightcol]) < 80)
                scroll(UP, 0, col + 1, 3, 80, SCREENROWS + 2);
} /* clearlastcol */

void printrow(void)
/* Prints the row headings */
{       int row;
        for(row = 0; row < SCREENROWS; row++)
                writef(1, row + 3, LEFTMARGIN, "%-d", row + toprow + 1);
} /* printrow */

void displcell(int col, int row, int highlighting, int updating)
/* Displays the contents of a cell */
{       char *s;
        if((updating) &&
          ((cell[col][row] == NULL) || (cell[col][row]->attrib != FORMULA)))
                return;
        s = cellstring(col, row, FORMAT);
        if(highlighting)
                revon();
        writef(colstart[col - leftcol] + 1, row - toprow + 3, colwidth[col], "%s", s);
        revoff();
} /* displaycell */

void displcol(int col, int updating)
/* Displays a column on the screen */
{       int row;
        for(row = toprow; row <= bottomrow; row++)
                displcell(col, row, NOHIGHLIGHT, updating);
} /* displaycol */

void displrow(int row, int updating)
/* Displays a row on the screen */
{       int col;
        for(col = leftcol; col <= rightcol; col++)
                displcell(col, row, NOHIGHLIGHT, updating);
} /* displayrow */

void displscreen(int updating)
/* Displays the current screen of the spreadsheet */
{       int row;
        for(row = toprow; row <= bottomrow; row++)
                displrow(row, updating);
        clearlastcol();
} /* displayscreen */

void clearinput(void)
/* Clears the input line */
{       scroll(UP, 0, 1, 25, 80, 25);
        gotoxy(1, 25);
} /* clearinput */

void showcelltype(void)
/* Prints the type of cell and what is in it */
{       char colstr[3], *s;
        scroll(UP, 0, 1, 24, 80, 25);
        formdisplay = !formdisplay;
        s = cellstring(curcol, currow, NOFORMAT);
        colstring(curcol, colstr);
        if(curcell == NULL)
                writef(1, 23, 10, "%s%d %s", colstr, currow + 1, MSGEMPTY);
        else
        {       switch(curcell->attrib)
                {       case TEXT:
                                writef(1, 23, 10, "%s%d %s", colstr, currow + 1, MSGTEXT);
                                break;
                        case VALUE:
                                writef(1, 23, 10, "%s%d %s", colstr, currow + 1, MSGVALUE);
                                break;
                        case FORMULA:
                                writef(1, 23, 10, "%s%d %s", colstr, currow + 1, MSGFORMULA);
                                break;
                } /* switch */
                writef(1, 24, 80, "%s", s);
        }
        formdisplay = !formdisplay;
        gotoxy(80, 23);
} /* showcelltype */

void redrawscreen(void)
/* Displays the entire screen */
{       setrightcol();
        setbottomrow();
        writef(1, 1, strlen(MSGMEMORY), MSGMEMORY);
        writef(29, 1, strlen(MSGCOMMAND), MSGCOMMAND);
        changeautocalc(autocalc);
        changeformdisplay(formdisplay);
        printfreemem();
        displscreen(NOUPDATE);
} /* redrawscreen */

