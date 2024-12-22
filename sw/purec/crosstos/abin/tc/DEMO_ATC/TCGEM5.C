/************************************************************************/
/*                                                                      */
/*       Beispielprogramm:                              FORM_ALERT( )   */
/*                                                      FORM_ERROR( )   */
/*                                 FSEL_INPUT( ) oder FSEL_EXINPUT( )   */
/*                                                                      */
/*       ôffnen einer Datei mit Fehlerbehandlung.                       */
/*                                                                      */
/*       Copyright (c) 1989 Borland International                       */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

/* -------------------------------------------------------------------- */
/*       Include-Files einbinden.                                       */
/* -------------------------------------------------------------------- */

#include <aes.h>
#include <tos.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* -------------------------------------------------------------------- */
/*       Konstantendefinitionen.                                        */
/* -------------------------------------------------------------------- */
                                 /* Indizes fÅr Meldungen:              */
                                 /* ----------------------------------- */
#define MOPENED               0  /* Datei konnte geîffnet werden        */
#define MFSEL                 1  /* Programmende                        */
#define MERROR                2  /* Datei konnte nicht geîffnet werden  */

#define STFILELEN            13  /* Maximale LÑnge eines Filenamens     */
#define STPATHLEN            64  /* Maximale LÑnge eines Pfades         */

#define EOS                '\0'  /* Ende eines Strings                  */
#define BACKSLASH          '\\'

/* -------------------------------------------------------------------- */
/*       Funktionsprototypen.                                           */
/* -------------------------------------------------------------------- */

void close_vwork( void );        /* aus TCFRAME.C                       */

void gem_prg( void );
void usage( int m );
void build_fname( char *dest, char *s1, char *s2 );
int  op_fbox( void );

/* -------------------------------------------------------------------- */
/*       Globale Variablen.                                             */
/* -------------------------------------------------------------------- */

char Path[STPATHLEN] = "A:\\*.*";

char *Mess[] =                      /* Texte fÅr Meldungen              */
{
    "[2][Datei konnte geîffnet werden.][Ende|Weiter]",
    "[3][Abbruch der Dateiauswahl.][ Ok ]"
};

/* -------------------------------------------------------------------- */
/*       void gem_prg( void );                                          */
/*                                                                      */
/*       Beispiel fÅr die Verwendung der Funktion fsel_input( ).        */
/*       (Bzw. der Dateiauswahl unter GEM generell.)                    */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void gem_prg( void )
{
   Path[0] = (char) Dgetdrv( ) + 'A' - 1; /* Aktuelles Laufwerk in Pfad */

   while ( op_fbox( ) != 0 )           /* Solange erfolgreich gewÑhlt   */
      usage( MOPENED );                /* wurde weiter.                 */
   usage( MFSEL );                     /* Jetzt aber Schluû.            */
}

/* -------------------------------------------------------------------- */
/*       void usage( int m );                                           */
/*                                                                      */
/*       Ausgabe einer vordefinierten Meldung.                          */
/*                                                                      */
/*       -> m                    Nummer der Meldung.                    */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void usage( int m )
{
   int endcode = 0;

   if ( m < 2 )
   {
      if( form_alert( 1, Mess[m] ) == 2) /* Wenn die Datei vorhanden    */
                                         /* ist, und der Benutzer im    */
                                         /* Programm weitermachen will, */
         return;                         /* dann Programm fortsetzen.   */
   }
   else                                  /* Datei ist nicht vorhanden.  */
   {
      form_error( ENOENT );
      endcode = -1;
   }

   close_vwork( );                       /* Sonst Programmabbruch.      */
   exit( endcode );
}

/* -------------------------------------------------------------------- */
/*       int op_fbox( void )                       open file via box    */
/*                                                                      */
/*       Datei mittels Dateiauswahlbox îffnen.                          */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      0  Falls das Programm beendet werden   */
/*                                  soll.                               */
/* -------------------------------------------------------------------- */

int op_fbox( void )
{
   char n[STFILELEN],                  /* Buffer fÅr Dateinamen         */
        x[STPATHLEN + STFILELEN];      /* Buffer fÅr Pfadnamen + Datei- */
                                       /* namen.                        */
   int  b;                             /* EnthÑlt Code des Buttons der  */
                                       /* zum Abbruch der Dateiauswahl  */
                                       /* fÅhrte.                       */
   int  version;                       /* wird GEMDOS-Versionsnummer    */
                                       /* erhalten.                     */
   int  result;

   *n = EOS;                           /* Dateinamen lîschen.           */

   version = Sversion ( );             /* Berechne die GEMDOS-Version,  */
   version >>= 8;                      /* da fsel_exinput erst ab  */
                                       /* Version 1.40 zur VerfÅgung    */
                                       /* steht.                        */

   if ( version <= 20 )
                                       /* Dateiauswahl.                 */
      result = fsel_input( Path, n, &b );
   else
      result = fsel_exinput( Path, n, &b, "Testauswahl" );

   if ( result == 0 )
      usage( result );                 /* Fehler dabei aufgetreten.     */
   else if ( b != 0)
   {
      build_fname( x, Path, n );       /* Pfad- und Dateinamen konkat.  */
                                       /* Datei 'testen'.               */
      if ( Fattrib( x, 0, 0 ) < 0 )
         usage( MERROR );              /* Fehler dabei aufgetreten.     */
   }

   return ( b );
}

/* -------------------------------------------------------------------- */
/*                                                  build a file name   */
/*       void build_fname( char *dest, char *s1, char *s2 );            */
/*                                                                      */
/*       Konkatoniere Pfadnamen und Dateinamen.                         */
/*                                                                      */
/*       -> dest                 Zielstring.                            */
/*          s1                   Pfadname.                              */
/*          s2                   Dateiname.                             */
/*                                                                      */
/*       <-                      Ergebnis befindet sich in 'dest'.      */
/* -------------------------------------------------------------------- */

void build_fname( char *dest, char *s1, char *s2 )
{
   char *cptr;

   strcpy( dest, s1 );                 /* Pfad kopieren.                */
   cptr = strrchr( dest, (int) BACKSLASH);
   strcpy( ++cptr, s2);                /* Schlieûlich den Dateinamen    */
}                                      /* dranhÑngen.                   */

/* -------------------------------------------------------------------- */
/*       Ende der Beispielprogramms fÅr FORM_ALERT( ), FORM_ERROR( ),   */
/*       und FSEL_INPUT() (oder FSEL_EXINPUT( )).                       */
/* -------------------------------------------------------------------- */
