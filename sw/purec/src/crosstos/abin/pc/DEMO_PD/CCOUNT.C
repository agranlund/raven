/************************************************************************/
/*                                                                      */
/*       CCOUNT.C                                                       */
/*                                                                      */
/*       Ein Beispielprogramm fÅr die Verwendung des Pure Debuggers.    */
/*                                                                      */
/*       Copyright (c) 1989 Borland International.                      */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NUL   ((char)0)             /* Endemarke (Nullzeichen)          */

extern char *print_cstatist( int *statarray );

/* -------------------------------------------------------------------- */
/*       Hauptprogramm                                                  */
/* -------------------------------------------------------------------- */

void main( int argc, char *argv[] )
{
    FILE       *infile;             /* Eingabedatei                     */
    char       linebuf[1024];       /* "momentane Zeile"                */
    char       *lineptr;            /* Zeiger auf das erste/nÑchste     */
                                    /* Wort in linebuf                  */
    int        i;                   /* fÅr Schleifen usw.               */
    static int cfrequence[(int)('Z' - 'A') + 2];
                                    /* speichert die HÑufigkeit der     */
                                    /* Buchstaben sowie aller anderen   */
                                    /* Zeichen                          */

    printf( "    CCOUNT ermittelt die HÑufigkeitsverteilung der Buchstaben in einem Text\n" );
    printf( "    -----------------------------------------------------------------------\n\n" );

    if ( argc != 2 )
    {
        printf( "Geben Sie den Namen der Textdatei ein (z.B. TEXT.DAT),\n" );
        printf( "oder nur <Return> fÅr Abbruch: " );

        gets( linebuf );            /* linebuf wird hier "mi·braucht"   */

        if ( !strlen( linebuf ))    /* nur RETURN gedrÅckt?             */
            exit( 1 );              /* Dann Programm vorzeitig beenden. */
    }
    else
        strcpy( linebuf, argv[1] );

    infile = fopen( linebuf, "r" ); /* ôffnen der Datei                 */
    if ( !infile )
    {
        printf( "Fehler beim ôffnen von %s\n", linebuf );
        exit( 1 );
    }

    printf( "Einlesen des Textes %s\n", linebuf );

                                    /* Lesen einer Zeile                */
    while ( fgets(linebuf, (int) sizeof( linebuf ), infile ))
    {
                                    /* PrÅfung des Pufferendes und      */
                                    /* Entfernen des abschlie·enden \n  */
        i = (int)strlen( linebuf ); /* LÑnge der Zeile                  */

                                    /* Zeilenvorschub letztes Zeichen?  */
        if ( linebuf[i - 1] != '\n' )
            printf( "\nBeginn einer Åberlangen Zeile:\n\t%70s\n", linebuf );
        else
            linebuf[i - 1] = NUL;   /* Zeilenvorschub ersetzen          */

        strupr( linebuf );
        lineptr = linebuf;

        while ( *lineptr )
        {
            if ( isupper( *lineptr ))
                cfrequence[(int)(*lineptr - 'A')]++;
            else
                cfrequence[(int)('Z' - 'A') + 1]++;

            lineptr++;
        }
    }

    print_cstatist( cfrequence );

    puts( "\nWeiter mit <Return>\n" );
    getchar();

    fclose(infile);                 /* Schlie·en der Datei und          */
    exit( 0 );                      /* Programmende                     */
}

/* -------------------------------------------------------------------- */
/*       Ende CCOUNT.C                                                  */
/* -------------------------------------------------------------------- */
