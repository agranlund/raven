/* MCOMMAND.C

   Copyright (c)  Heimsoeth & Borland  1988

*/


#include <string.h>
#include <stdio.h>
#include <math.h>
#include "io.h"
#include "mcalc.h"
#include "conio.h"

char *name = MSGNAME;

void moverowup(void)
/* Moves up one row */
{       displcell(curcol, currow, NOHIGHLIGHT, NOUPDATE);
        if(currow > toprow)
                currow--;
        else if(toprow != 0)
        {       scroll(DOWN, 1, LEFTMARGIN + 1, 3, 80, SCREENROWS + 2);
                displrow(--toprow, NOUPDATE);
                currow--;
                setbottomrow();
        }
} /* moverowup */

void moverowdown(void)
/* Moves down one row */
{       displcell(curcol, currow, NOHIGHLIGHT, NOUPDATE);
        if(currow < bottomrow)
                currow++;
        else if(bottomrow < (MAXROWS - 1))
        {       scroll(UP, 1, LEFTMARGIN + 1, 3, 80, SCREENROWS + 2);
                toprow++;
                currow++;
                setbottomrow();
                displrow(bottomrow, NOUPDATE);
        }
} /* moverowdown */

void movecolleft(void)
/* Moves left one column */
{       int col, oldleftcol;
        unsigned char oldcolstart[SCREENCOLS];
        oldleftcol = leftcol;
        memmove(oldcolstart, colstart, sizeof(colstart));
        displcell(curcol, currow, NOHIGHLIGHT, NOUPDATE);
        if(curcol > leftcol)
                curcol--;
        else if(leftcol != 0)
        {       curcol--;
                leftcol--;
                setrightcol();
                setleftcol();
                if(oldleftcol <= rightcol)
                        scroll(RIGHT, colstart[oldleftcol - leftcol] - LEFTMARGIN, LEFTMARGIN + 1,
                               3, 80, SCREENROWS + 2);
                clearlastcol();
                for(col = leftcol; col <= oldleftcol - 1; col++)
                        displcol(col, NOUPDATE);
        }
} /* movecolleft */

void movecolright(void)
/* Moves right one column */
{       int col, oldleftcol, oldrightcol;
        unsigned char oldcolstart[SCREENCOLS];
        memmove(oldcolstart, colstart, sizeof(colstart));
        oldleftcol = leftcol;
        oldrightcol = rightcol;
        displcell(curcol, currow, NOHIGHLIGHT, NOUPDATE);
        if(curcol < rightcol)
                curcol++;
        else if(rightcol < (MAXCOLS - 1))
        {       curcol++;
                rightcol++;
                setleftcol();
                setrightcol();
                if(oldrightcol >= leftcol)
                        scroll(LEFT, oldcolstart[leftcol - oldleftcol] - LEFTMARGIN,
                               LEFTMARGIN + 1, 3, 80, SCREENROWS + 2);
                clearlastcol();
                for(col = oldrightcol + 1; col <= rightcol; col++)
                        displcol(col, NOUPDATE);
        }
} /* movecolright */

void recalc(void)
/* Recalculates all of the numbers in the speadsheet */
{       int col, row, dummy;
        for(col = 0; col <= lastcol; col++)
        {       for(row = 0; row <= lastrow; row++)
                {       if((cell[col][row] != NULL) && (cell[col][row]->attrib == FORMULA))
                                cell[col][row]->v.f.fvalue = parse(cell[col][row]->v.f.formula, &dummy);
                }
        }
        displscreen(UPDATE);
} /* recalc */

void changeautocalc(int newmode)
/* Changes and prints the current AutoCalc value on the screen */
{       char s[15];
        if(!autocalc && newmode)
                recalc();
        autocalc = newmode;
        if(autocalc)
                strcpy(s, MSGAUTOCALC);
        else
                s[0] = '\0';
        writef(73, 1, strlen(MSGAUTOCALC), s);
} /* autocalc */

void changeformdisplay(int newmode)
/* Changes and prints the current formula display value on the screen */
{       char s[15];
        formdisplay = newmode;
        if(formdisplay)
                strcpy(s, MSGFORMDISPLAY);
        else
                s[0] = '\0';
        writef(65, 1, strlen(MSGFORMDISPLAY), s);
} /* autocalc */

void editcell(CELLPTR ecell)
/* Edits a selected cell */
{       char s[MAXINPUT + 1];
        if(ecell == NULL)
                return;
        switch(ecell->attrib)
        {       case TEXT:
                        strcpy(s, ecell->v.text);
                        break;
                case VALUE:
                        if(ecell->v.value == HUGE_VAL)
                                strcpy(s, "0");
                        else
                                sprintf(s, "%.*lf", MAXPLACES, ecell->v.value);
                        break;
                case FORMULA:
                        strcpy(s, ecell->v.f.formula);
                        break;
        } /* switch */
        if(!editstring(s, "", MAXINPUT) || (s[0] == '\0'))
                return;
        act(s);
        changed = TRUE;
} /* editcell */

void clearsheet(void)
/* Clears the current spreadsheet */
{       int col, row;
        for(row = 0; row<= lastrow; row++)
        {       for(col = 0; col <= lastcol; col++)
                        deletecell(col, row, NOUPDATE);
        }
        initvars();
        setrightcol();
        setbottomrow();
        displscreen(NOUPDATE);
        printfreemem();
        changed = FALSE;
} /* clearsheet */

struct CELLREC rec;

void loadsheet(char *filename)
/* Loads a new spreadsheet */
{       int size, allocated, reallastcol = 0, reallastrow = 0, file;
        char check[81];
        if(filename[0] == '\0')
        {       writeprompt(MSGFILENAME);
                if(!editstring(filename, "", MAXINPUT))
                        return;
        }
        if(access(filename, 0))
        {       errormsg(MSGNOEXIST);
                return;
        }
        if((file = open(filename, O_RDWR)) == -1)
        {       errormsg(MSGNOOPEN);
                return;
        }
        read(file, check, strlen(name) + 1);
        if(strcmp(check, name) != 0)
        {       errormsg(MSGNOMICROCALC);
                close(file);
                return;
        }
        writef(1, 25, 79, MSGLOADING);
        gotoxy(strlen(MSGLOADING) + 1, 25);
        clearsheet();
        read(file, (char *)&size, 1);
        read(file, (char *)&lastcol, 2);
        read(file, (char *)&lastrow, 2);
        read(file, (char *)&size, 2);
        read(file, colwidth, sizeof(colwidth));
        do
        {       if(read(file, (char *)&curcol, 2) <= 0)
                        break;
                read(file, (char *)&currow, 2);
                read(file, &format[curcol][currow], 1);
                read(file, (char *)&size, 2);
                read(file, (char *)&rec, size);
                switch(rec.attrib)
                {       case TEXT:
                                if((allocated = alloctext(curcol, currow, rec.v.text)) == TRUE)
                                        setoflags(curcol, currow, NOUPDATE);
                                break;
                        case VALUE:
                                allocated = allocvalue(curcol, currow, rec.v.value);
                                break;
                        case FORMULA:
                                allocated = allocformula(curcol, currow, rec.v.f.formula, rec.v.f.fvalue);
                                break;
                } /* switch */
                if(!allocated)
                {       errormsg(MSGFILELOMEM);
                        lastrow = reallastrow;
                        lastcol = reallastcol;
                        format[curcol][currow] = DEFAULTFORMAT;
                        break;
                }
                else
                {       if(curcol > reallastcol)
                                reallastcol = curcol;
                        if(currow > reallastrow)
                                reallastrow = currow;
                }
        } while (TRUE);
        writef(1, 25, strlen(MSGLOADING), "");
        gotoxy(1, 25);
        printfreemem();
        close(file);
        curcol = currow = 0;
        setrightcol();
        displscreen(NOUPDATE);
        changed = FALSE;
} /* loadsheet */

void savesheet(void)
/* Saves the current spreadsheet */
{       char filename[MAXINPUT+1], eof = 26;
        int size, col, row, overwrite, file;
        CELLPTR cellptr;
        filename[0] = '\0';
        writeprompt(MSGFILENAME);
        if(!editstring(filename, "", MAXINPUT))
                return;
        if(!access(filename, 0))
        {       if(!getyesno(&overwrite, MSGOVERWRITE) || (overwrite == 'N'))
                        return;
        }
        if((file = open(filename, O_RDWR | O_CREAT | O_TRUNC | O_BINARY,
                  S_IREAD | S_IWRITE)) == -1)
        {       errormsg(MSGNOOPEN);
                return;
        }
        writef(1, 25, 79, MSGSAVING);
        gotoxy(strlen(MSGSAVING) + 1, 25);
        write(file, name, strlen(name) + 1);
        write(file, &eof, 1);
        write(file, (char *)&lastcol, 2);
        write(file, (char *)&lastrow, 2);
        size = MAXCOLS;
        write(file, (char *)&size, 2);
        write(file, colwidth, sizeof(colwidth));
        for(row = 0; row <= lastrow; row++)
        {       for(col = lastcol; col >= 0; col--)
                {       if(cell[col][row] != NULL)
                        {       cellptr = cell[col][row];
                                switch(cellptr->attrib)
                                {       case TEXT:
                                                size = strlen(cellptr->v.text) + 3;
                                                break;
                                        case VALUE:
                                                size = (int) sizeof(double) + 2;
                                                break;
                                        case FORMULA:
                                                size = strlen(cellptr->v.f.formula) + 3 + (int) sizeof(double);
                                                break;
                                } /* switch */
                                write(file, (char *)&col, 2);
                                write(file, (char *)&row, 2);
                                write(file, (char *)&format[col][row], 1);
                                write(file, (char *)&size, 2);
                                write(file, (char *)cellptr, size);
                        }
                }
        }
        close(file);
        writef(1, 25, strlen(MSGSAVING), "");
        gotoxy(1, 25);
        changed = FALSE;
} /* savesheet */

int pagerows(int row, int toppage, int border)
/* Returns the number of rows to print */
{       int rows;
        rows = toppage ? 66 - TOPMARGIN : 66;
        if(border)
                rows--;
        if(row + rows - 1 > lastrow)
                return(lastrow - row + 1);
        else
                return(rows);
} /* pagerows */

int pagecols(int col, int border, int columns)
/* Returns the number of columns to print starting at col */
{       int len = ((col == 0) && (border)) ? columns - LEFTMARGIN : columns;
        int firstcol = col;
        while((len > 0) && (col <= lastcol))
                len -= colwidth[col++];
        if(len < 0)
                col--;
        return(col - firstcol);
} /* pagecols */

void printsheet(void)
/* Prints a copy of the spreadsheet to a file or to the printer */
{       char filename[MAXINPUT + 1], s[133], colstr[MAXCOLWIDTH + 1];
        FILE *file;
        int columns, counter1, counter2, counter3, col = 0, row, border, toppage,
            lcol, lrow, printed, oldlastcol;
        filename[0] = '\0';
        writeprompt(MSGPRINT);
        if(!editstring(filename, "", MAXINPUT))
                return;
        if(filename[0] == '\0')
                strcpy(filename, "PRN:");
        if((file = fopen(filename, "wt")) == NULL)
        {       errormsg(MSGNOOPEN);
                return;
        }
        oldlastcol = lastcol;
        for(counter1 = 0; counter1 <= lastrow; counter1++)
        {       for(counter2 = lastcol; counter2 < MAXCOLS; counter2++)
                {       if(format[counter2][counter1] >= OVERWRITE)
                        lastcol = counter2;
                }
        }
        if(!getyesno(&columns, MSGCOLUMNS))
                return;
        columns = (columns == 'Y') ? 131 : 79;
        if(!getyesno(&border, MSGBORDER))
                return;
        border = (border == 'Y');
        while(col <= lastcol)
        {       row = 0;
                toppage = TRUE;
                lcol = pagecols(col, border, columns) + col;
                while(row <= lastrow)
                {       lrow = pagerows(row, toppage, border) + row;
                        printed = 0;
                        if(toppage)
                        {       for(counter1 = 0; counter1 < TOPMARGIN; counter1++)
                                {       fprintf(file, "\n");
                                        printed++;
                                }
                        }
                        for(counter1 = row; counter1 < lrow; counter1++)
                        {       if((border) && (counter1 == row) && (toppage))
                                {       if((col == 0) && (border))
                                                sprintf(s, "%*s", LEFTMARGIN, "");
                                        else
                                                s[0] = '\0';
                                        for(counter3 = col; counter3 < lcol; counter3++)
                                        {       centercolstring(counter3, colstr);
                                                strcat(s, colstr);
                                        }
                                        fprintf(file, "%s\n", s);
                                        printed++;
                                }
                                if((col == 0) && (border))
                                        sprintf(s, "%-*d", LEFTMARGIN, counter1 + 1);
                                else
                                        s[0] = '\0';
                                for(counter2 = col; counter2 < lcol; counter2++)
                                        strcat(s, cellstring(counter2, counter1, FORMAT));
                                fprintf(file, "%s\n", s);
                                printed++;
                        }
                        row = lrow;
                        toppage = FALSE;
                        if(printed < 66)
                                fprintf(file, "%c", FORMFEED);
                }
                col = lcol;
        }
        fclose(file);
        lastcol = oldlastcol;
} /* printsheet */

void setcolwidth(int col)
/* Sets the new column width for a selected column */
{       int width, row;
        writeprompt(MSGCOLWIDTH);
        if(!getint(&width, MINCOLWIDTH, MAXCOLWIDTH))
                return;
        colwidth[col] = width;
        setrightcol();
        if(rightcol < col)
        {       rightcol = col;
                setleftcol();
                setrightcol();
        }
        for(row = 0; row <= lastrow; row++)
        {       if((cell[col][row] != NULL) && (cell[col][row]->attrib == TEXT))
                        clearoflags(col + 1, row, NOUPDATE);
                else
                        clearoflags(col, row, NOUPDATE);
                updateoflags(col, row, NOUPDATE);
        }
        displscreen(NOUPDATE);
        changed = TRUE;
} /* setcolwidth */

void gotocell(void)
/* Moves to a selected cell */
{       writeprompt(MSGGOTO);
        if(!getcell(&curcol, &currow))
                return;
        leftcol = curcol;
        toprow = currow;
        setbottomrow();
        setrightcol();
        setleftcol();
        displscreen(NOUPDATE);
} /* gotocell */

void formatcells(void)
/* Prompts the user for a selected format and range of cells */
{       int col, row, col1, col2, row1, row2, temp, newformat = 0;
        writeprompt(MSGCELL1);
        if(!getcell(&col1, &row1))
                return;
        writeprompt(MSGCELL2);
        if(!getcell(&col2, &row2))
                return;
        if((col1 != col2) && (row1 != row2))
                errormsg(MSGDIFFCOLROW);
        else
        {       if(col1 > col2)
                        swap(&col1, &col2);
                if(row1 > row2)
                        swap(&row1, &row2);
                if(!getyesno(&temp, MSGRIGHTJUST))
                        return;
                newformat += (temp == YESCHAR) * RJUSTIFY;
                if(!getyesno(&temp, MSGDOLLAR))
                        return;
                newformat += (temp == YESCHAR) * DOLLAR;
                if(!getyesno(&temp, MSGCOMMAS))
                        return;
                newformat += (temp == YESCHAR) * COMMAS;
                if(newformat & DOLLAR)
                        newformat += 2;
                else
                {       writeprompt(MSGPLACES);
                        if(!getint(&temp, 0, MAXPLACES))
                                return;
                        newformat += temp;
                }
                for(col = col1; col <= col2; col++)
                {       for(row = row1; row <= row2; row++)
                        {       format[col][row] = (format[col][row] & OVERWRITE) | newformat;
                                if((col >= leftcol) && (col <= rightcol) &&
                                   (row >= toprow) && (row <= bottomrow))
                                displcell(col, row, NOHIGHLIGHT, NOUPDATE);
                        }
                }
        }
        changed = TRUE;
} /* formatcells */

void deletecol(int col)
/* Deletes a column */
{       int counter, row;
        for(counter = 0; counter <= lastrow; counter++)
                deletecell(col, counter, NOUPDATE);
        printfreemem();
        if(col != MAXCOLS - 1)
        {       memmove(&cell[col][0], &cell[col + 1][0], MAXROWS * sizeof(CELLPTR) *
                        (MAXCOLS - col - 1));
                memmove(&format[col][0], &format[col + 1][0], MAXROWS * (MAXCOLS - col - 1));
                memmove(&colwidth[col], &colwidth[col + 1], MAXCOLS - col - 1);
        }
        memset(&cell[MAXCOLS - 1][0], 0, MAXROWS * sizeof(CELLPTR));
        memset(&format[MAXCOLS - 1][0], DEFAULTFORMAT, MAXROWS);
        colwidth[MAXCOLS - 1] = DEFAULTWIDTH;
        if((lastcol >= col) && (lastcol > 0))
                lastcol--;
        setrightcol();
        if(curcol > rightcol)
        {       rightcol++;
                setleftcol();
        }
        clearlastcol();
        for(counter = 0; counter <= lastcol; counter++)
        {       for(row = 0; row <= lastrow; row++)
                {       if((cell[counter][row] != NULL) && (cell[counter][row]->attrib == FORMULA))
                                fixformula(counter, row, COLDEL, col);
                        updateoflags(col, row, NOUPDATE);
                }
        }
        while(col <= rightcol)
                displcol(col++, NOUPDATE);
        changed = TRUE;
        recalc();
} /* deletecol */

void insertcol(int col)
/* Inserts a column */
{       int counter, row;
        if(lastcol == MAXCOLS - 1)
        {       for(counter = 0; counter <= lastrow; counter++)
                        deletecell(lastcol, counter, NOUPDATE);
                printfreemem();
        }
        if(col != MAXCOLS - 1)
        {       memmove(&cell[col + 1][0], &cell[col][0], MAXROWS * sizeof(CELLPTR) *
                        (MAXCOLS - col - 1));
                memmove(&format[col + 1][0], &format[col][0], MAXROWS * (MAXCOLS - col - 1));
                memmove(&colwidth[col + 1], &colwidth[col], MAXCOLS - col - 1);
        }
        memset(&cell[col][0], 0, MAXROWS * sizeof(CELLPTR));
        memset(&format[col][0],  DEFAULTFORMAT, MAXROWS);
        colwidth[col] = DEFAULTWIDTH;
        lastcol = MAXCOLS - 1;
        setlastcol();
        setrightcol();
        if(curcol > rightcol)
        {       rightcol++;
                setleftcol();
        }
        for(counter = 0; counter <= lastcol; counter++)
        {       for(row = 0; row <= lastrow; row++)
                {       if((cell[counter][row] != NULL) && (cell[counter][row]->attrib == FORMULA))
                                fixformula(counter, row, COLADD, col);
                        updateoflags(col, row, NOUPDATE);
                }
        }
        while(col <= rightcol)
                displcol(col++, NOUPDATE);
        changed = TRUE;
        recalc();
} /* insertcol */

void deleterow(int row)
/* Deletes a row */
{       int counter, rowc;
        for(counter = 0; counter <= lastcol; counter++)
                deletecell(counter, row, NOUPDATE);
        printfreemem();
        if(row != MAXROWS - 1)
        {       for(counter = 0; counter < MAXCOLS; counter++)
                {       memmove(&cell[counter][row], &cell[counter][row + 1],
                                sizeof(CELLPTR) * (MAXROWS - row - 1));
                        memmove(&format[counter][row], &format[counter][row + 1], MAXROWS - row - 1);
                }
        }
        else
        {       for(counter = 0; counter <= lastcol; counter++)
                {       cell[counter][MAXROWS - 1] = NULL;
                        format[counter][MAXROWS - 1] = DEFAULTFORMAT;
                }
        }
        if((lastrow >= row) && (lastrow > 0))
                lastrow--;
        for(counter = 0; counter <= lastcol; counter++)
        {       for(rowc = 0; rowc <= lastrow; rowc++)
                {       if((cell[counter][rowc] != NULL) &&
                           (cell[counter][rowc]->attrib == FORMULA))
                                fixformula(counter, rowc, ROWDEL, row);
                }
        }
        while (row <= bottomrow)
                displrow(row++, NOUPDATE);
        changed = TRUE;
        recalc();
} /* deleterow */

void insertrow(int row)
/* Inserts a row */
{       int counter, rowc;
        if(lastrow == MAXROWS - 1)
        {       for(counter = 0; counter <= lastcol; counter++)
                        deletecell(counter, lastrow, NOUPDATE);
                printfreemem();
        }
        if(row != MAXROWS - 1)
        {       for(counter = 0; counter < MAXCOLS; counter++)
                {       memmove(&cell[counter][row + 1], &cell[counter][row],
                                sizeof(CELLPTR) * (MAXROWS - row - 1));
                        memmove(&format[counter][row + 1], &format[counter][row], MAXROWS - row - 1);
                }
        }
        for(counter = 0; counter <= lastcol; counter++)
        {       cell[counter][row] = NULL;
                format[counter][row] = DEFAULTFORMAT;
        }
        lastrow = MAXROWS - 1;
        setlastrow();
        for(counter = 0; counter <= lastcol; counter++)
        {       for(rowc = 0; rowc <= lastrow; rowc++)
                {       if((cell[counter][rowc] != NULL) &&
                           (cell[counter][rowc]->attrib == FORMULA))
                                fixformula(counter, rowc, ROWADD, row);
                }
        }
        while(row <= bottomrow)
                displrow(row++, NOUPDATE);
        changed = TRUE;
        recalc();
} /* insertrow */

void smenu(void)
/* Executes the commands in the spreadsheet menu */
{       char filename[MAXINPUT + 1];
        filename[0] = '\0';
        switch(getcommand(SMENU, SCOMMAND))
        {       case 0:
                        checkforsave();
                        loadsheet(filename);
                        break;
                case 1:
                        savesheet();
                        break;
                case 2:
                        printsheet();
                break;
                case 3:
                        checkforsave();
                        clearsheet();
                        break;
        } /* switch */
} /* smenu */

void cmenu(void)
/* Executes the commands in the column menu */
{       switch(getcommand(CMENU, CCOMMAND))
        {       case 0:
                        insertcol(curcol);
                        break;
                case 1:
                        deletecol(curcol);
                        break;
                case 2:
                        setcolwidth(curcol);
                        break;
        } /* switch */
} /* cmenu */

void rmenu(void)
/* Executes the commands in the row menu */
{       switch(getcommand(RMENU, RCOMMAND))
        {       case 0:
                        insertrow(currow);
                        break;
                case 1:
                        deleterow(currow);
                        break;
        } /* switch */
} /* rmenu */

void umenu(void)
/* Executes the commands in the utility menu */
{       switch(getcommand(UMENU, UCOMMAND))
        {       case 0:
                        recalc();
                        break;
                case 1:
                        changeformdisplay(!formdisplay);
                        displscreen(UPDATE);
                        break;
        } /* switch */
} /* umenu */

void mainmenu(void)
/* Executes the commands in the main menu */
{       switch(getcommand(MENU, COMMAND))
        {       case 0:
                        smenu();
                        break;
                case 1:
                        formatcells();
                        break;
                case 2:
                        deletecell(curcol, currow, UPDATE);
                        printfreemem();
                        if(autocalc)
                                recalc();
                        break;
                case 3:
                        gotocell();
                        break;
                case 4:
                        cmenu();
                        break;
                case 5:
                        rmenu();
                        break;
                case 6:
                        editcell(curcell);
                        break;
                case 7:
                        umenu();
                        break;
                case 8:
                        changeautocalc(!autocalc);
                        break;
                case 9:
                        checkforsave();
                        stop = TRUE;
                        break;
        } /* switch */
} /* mainmenu */
