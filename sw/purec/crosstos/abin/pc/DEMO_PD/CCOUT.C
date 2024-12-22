/************************************************************************/
/*                                                                      */
/*       CCOUT.C                                                        */
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

/* -------------------------------------------------------------------- */
/*       void print_cstatist( int *statarray );                         */
/*                                                                      */
/*       Ausdrucken der ZeichenhÑufigkeitsstatistik.                    */
/* -------------------------------------------------------------------- */

void print_cstatist( int *statarray )
{
    int i;

    printf( "\nErgebnis:\n---------\n" );
    printf( "\n  Buchstabe  Anzahl  Buchstabe  Anzahl  Buchstabe  Anzahl  Buchstabe  Anzahl\n" );   /* Ausgabe der WortlÑngen         */
    for ( i = 0; i < 24; i+=4 )
        printf( "%7c   %7d %9c  %7d %9c  %7d %9c  %7d\n",
                (char)i + 'A', statarray[i],
                (char)i + 1 + 'A', statarray[i + 1],
                (char)i + 2 + 'A', statarray[i + 2],
                (char)i + 3 + 'A', statarray[i + 3] );
    printf( "%7c   %7d %9c  %7d %9c\n\n",
            (char)i + 'A', statarray[i],
            (char)i + 1 + 'A', statarray[i + 1] );

    printf( "  Sonstige Zeichen: %d\n", statarray[i + 2] );
}

/* -------------------------------------------------------------------- */
/*       Ende CCOUT.C                                                   */
/* -------------------------------------------------------------------- */
