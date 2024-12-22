/************************************************************************/
/*                                                                      */
/*       Beispielprogramm:                            APPL_TRECORD( )   */
/*                                                      APPL_TPLAY( )   */
/*                                                                      */
/*       Copyright (c) 1989 Borland International                       */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

/* -------------------------------------------------------------------- */
/*       Include-Files einbinden.                                       */
/* -------------------------------------------------------------------- */

#include <aes.h>

/* -------------------------------------------------------------------- */
/*       Konstantendefinitionen.                                        */
/* -------------------------------------------------------------------- */

#define STARTMES              0
#define EORECORDING           1
#define REPLAYFAST            2
#define REPLAYHURICANE        3
#define GOODBYE               4

#define BUFSIZE               16000

/* -------------------------------------------------------------------- */
/*       Typendefinitionen.                                             */
/* -------------------------------------------------------------------- */

typedef struct
{
    long     code;
    long     event;
}
RECORD;

/* -------------------------------------------------------------------- */
/*    Globale Variablen.                                                */
/* -------------------------------------------------------------------- */

char *Mess[] =
{
    "[1][Alle im folgenden gemachten|Mausbewegungen werden nun|aufgezeichnet.][Start]",
    "[3][Das war's.|Und nun die Wiedergabe:][Play]",
    "[1][Nun eine etwas schneller|Wiedergabe.][Play]",
    "[1][Zum Schluž:|Schnell wie der Wind...][Play]",
    "[3][Damit ist das Programm|beendet.][Ende]"
};

RECORD Buffer[BUFSIZE];

/* -------------------------------------------------------------------- */
/*       Funktionsprototypen.                                           */
/* -------------------------------------------------------------------- */

void gem_prg( void );
int usage( int message);

/* -------------------------------------------------------------------- */
/*       void gem_prg( void );                                          */
/*                                                                      */
/*       Beispiel fr die Verwendung der Funktionen appl_trecord( )     */
/*       und appl_tplay in einem GEM-Programm.                          */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void gem_prg( void )
{
   usage( STARTMES );
   appl_trecord( Buffer, 2000 );
   usage( EORECORDING );
   appl_tplay( Buffer, 2000, 5 );
   usage( REPLAYFAST );
   appl_tplay( Buffer, 2000, 20 );
   usage( REPLAYHURICANE );
   appl_tplay( Buffer, 2000, 100 );
   usage( GOODBYE );
}

/* -------------------------------------------------------------------- */
/*       int usage( int message );                                      */
/*                                                                      */
/*       Es wird eine Meldung fr den Benutzer ausgegeben, auf die er   */
/*       dann reagieren muž.                                            */
/*                                                                      */
/*       -> message              Index der Meldung im Feld Mess (s.o.)  */
/*                                                                      */
/*       <-                      Der Objektindex des Exitbutton wird    */
/*                               zurckgegeben.                         */
/* -------------------------------------------------------------------- */

int usage( int message )
{
   return ( form_alert( 1, *( Mess + message )));
}

/* -------------------------------------------------------------------- */
/*       Ende der Beispielprogramms fr APPL_TRECORD( ), APPL_TPLAY( ). */
/* -------------------------------------------------------------------- */
