/* Copyright (C) 1991, 1992, 1997, 1998 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Modified for MiNTLib by Guido Flohr <guido@freemint.de>.  */


#include <stdio.h>
#include <string.h>


#ifdef fputs_unlocked
# undef fputs_unlocked
#endif


/* Write the string S to STREAM.  */
int
fputs(const char* s, FILE* stream)
{
    size_t len = strlen(s);
    int    ret = len;

    if (len == 1) {
        if (*s == '\n' && !stream->__mode.__binary && fputc('\r', stream) < 0) {
            ret = EOF;
        } else if (fputc(*s, stream) < 0) {
            ret = EOF;
        }
    } else if (stream->__mode.__binary) {
        if (fwrite(s, sizeof(char), len, stream) != len) {
            ret = EOF;
        }
    } else {
        char* nl = strchr(s, '\n');

        while (nl != NULL) {
            if (nl == s || (nl > s && nl[-1] != '\r')) {
                *nl = '\0';
                len = strlen(s);

                if (fwrite(s, sizeof(char), len, stream) != len || fwrite("\r\n", sizeof(char), 2, stream) != 2) {
                    ret = EOF;
                    *nl = '\n';
                    break;
                }

                *nl = '\n';
                s   = nl + 1;
            }

            nl = strchr(nl + 1, '\n');
        }

        if (ret != EOF && s != NULL) {
            len = strlen(s);

            if (fwrite(s, sizeof(char), len, stream) != len) {
                ret = EOF;
            }
        }
    }

    return ret;
}
