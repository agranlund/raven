/* MCINPUT.C

   Copyright (c)  Heimsoeth & Borland  1988

*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tos.h>
#include <string.h>
#include "mcalc.h"
#include "conio.h"

int getkey(void)
/* Uses the BIOS to read the next unbuffered keyboard character */
{       long key;
        int lo, hi;
        key = Bconin(2);
        lo = (int) (key & 0xFFL);
        hi = (int) ((key & 0xFF0000L) >> 16);
        return((lo == 0) ? hi + 256 : lo);
} /* getkey */

int editstring(char *s, char *legal, int maxlength)
/* Allows the user to edit a string with only certain characters allowed -
   Returns TRUE if ESC was not pressed, FALSE is ESC was pressed. */
{       int c, update = TRUE, len = strlen(s), pos = len, insert = TRUE;
        do
        {       if(update)
                        {       update = FALSE;
                                writef(1, 25, 79, "%s", s);
                        }
                gotoxy(pos + 1, 25);
                switch(c = getkey())
                {       case HOMEKEY:
                                pos = 0;
                                break;
                        case ENDKEY:
                                pos = len;
                                break;
                        case INSKEY:
                                insert = !insert;
                                break;
                        case LEFTKEY:
                                if(pos > 0)
                                        pos--;
                                break;
                        case RIGHTKEY:
                                if(pos < len)
                                        pos++;
                                break;
                        case BS:
                                if(pos > 0)
                                {       memmove(&s[pos - 1], &s[pos], len - pos + 1);
                                        pos--;
                                        len--;
                                        update = TRUE;
                                }
                                break;
                        case DELKEY:
                                if(pos < len)
                                {       memmove(&s[pos], &s[pos + 1], len - pos);
                                        len--;
                                        update = TRUE;
                                }
                                break;
                        case UPKEY:
                        case DOWNKEY:
                                c = CR;
                                break;
                        case ESC:
                                len = 0;
                                break;
                        default:
                                if(((legal[0] == '\0') || (strchr(legal, c) != NULL)) &&
                                   isprint(c) && (len < maxlength))
                                {       if(insert)
                                        {       memmove(&s[pos + 1], &s[pos], len - pos + 1);
                                                len++;
                                        }
                                        else if(pos >= len)
                                                len++;
                                        s[pos++] = c;
                                        update = TRUE;
                                }
                                break;
                } /* switch */
                s[len] = '\0';
        } while((c != CR) && (c != ESC));
        clearinput();
        return(c != ESC);
} /* editstring */

void getinput(int c)
/* Reads and acts on an input string from the keyboard that started with c */
{       char s[MAXINPUT + 1];
        s[0] = c;       s[1] = '\0';
        if(!editstring(s, "", MAXINPUT) || (s[0] == '\0'))
                return;
        act(s);
        changed = TRUE;
} /* getinput */

int getint(int *number, int low, int high)
/* Reads in a positive integer from low to high */
{       int i, good = FALSE;
        char s[5], message[81];
        s[0] = '\0';
        sprintf(message, MSGBADNUMBER, low, high);
        do
        {       if(!editstring(s, "1234567890", 4))
                        return(FALSE);
                i = atoi(s);
                if((good = (int) ((i >= low) && (i <= high))) == FALSE)
                        errormsg(message);
        } while(!good);
        *number = i;
        return(TRUE);
} /* getint */

int getcell(int *col, int *row)
/* Reads in a cell name that was typed in - Returns FALSE if ESC was pressed */
{       int first = TRUE, good = FALSE, len, numlen = rowwidth(MAXROWS);
        int oldcol = *col, oldrow = *row;
        char data[10], *input, *start, numstring[6];
        data[0] = '\0';
        do
        {       if(!first)
                        errormsg(MSGBADCELL);
                first = FALSE;
                input = data;
                if(!editstring(data, "", numlen + 2))
                {       *col = oldcol;
                        *row = oldrow;
                        return(FALSE);
                }
                *col = toupper(*(input++)) - 'A';
                if(isalpha(*input))
                {       *col *= 26;
                        *col += toupper(*(input++)) - 'A' + 26;
                }
                if(*col >= MAXCOLS)
                        continue;
                start = input;
                for(len = 0; len < numlen; len++)
                {       if(!isdigit(*(input++)))
                        {       input--;
                                break;
                        }
                }
                if(len == 0)
                        continue;
                strncpy(numstring, start, len);
                numstring[len] = '\0';
                *row = atoi(numstring) - 1;
                if((*row >= MAXROWS) || (*row == -1) || (*input != '\0'))
                        continue;
                good = TRUE;
        } while(!good);
        return(TRUE);
} /* getcell */
int getyesno(int *yesno, char *prompt)
/* Prints a prompt and gets a yes or no answer - returns TRUE if ESC was
   pressed, FALSE if not. */
{       writeprompt(prompt);
        do
        {       *yesno = toupper(getkey());
                if(*yesno == ESC)
                        return(FALSE);
        }       while(strchr(YESNOSTRING, *yesno) == NULL);
        return(TRUE);
} /* getyesno */

