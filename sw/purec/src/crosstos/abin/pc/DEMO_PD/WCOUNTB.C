/************************************************************************/
/*                                                                      */
/*       WCOUNTB.C                                                      */
/*                                                                      */
/*       Ein Beispielprogramm f�r die Verwendung des Pure Debuggers.    */
/*                                                                      */
/*       ACHTUNG:       DIESES PROGRAMM ENTH�LT EINIGE FEHLER!!!        */
/*                                                                      */
/*       Copyright (c) 1989 Borland International.                      */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXWORDLEN 16               /* maximale L�nge eines Wortes      */
#define NUL   ((char)0)             /* Endemarke (Nullzeichen)          */
#define SPACE ((char)0x20)          /* Trennzeichen zwischen W�rtern    */

char *nextword( char *lineptr );
int  wordlen( char *wordstart );

/* -------------------------------------------------------------------- */
/*       Hauptprogramm                                                  */
/* -------------------------------------------------------------------- */

void main( void )
{
   FILE       *infile;                  /* Eingabedatei                 */
   char       linebuf[1024];            /* "momentane Zeile"            */
   char       *lineptr;                 /* Zeiger auf das erste/n�chste */
                                        /* Wort in linebuf              */
   int        i;                        /* f�r Schleifen usw.           */
   static int wordlens[MAXWORDLEN + 1], /* speichert die Wortl�ngen     */
              longwords;                /* F�r W�rter > MAXWORDLEN      */


   printf( "VORSICHT!\n\n");
   printf( "WCOUNTB.C ist ein �bungsprogramm f�r Pure Debugger.\n");
   printf( "Wenn Sie dieses Programm nicht vom Debugger aus gestartet\n" );
   printf( "haben, dann dr�cken Sie bitte nur <Return>!\n\n" );
   printf( "Ansonsten h�ngt sich das System auf.\n\n" );
   printf( "Weitere Informationen finden Sie im Benutzerhandbuch.\n\n" );

   printf( "Geben Sie den Namen der Textdatei ein (z.B. TEXT.DAT): " );

   gets( linebuf );                 /* linebuf wird hier "mi�braucht"   */

   if ( !strlen( linebuf ))         /* nur RETURN gedr�ckt?             */
      exit( 1 );                    /* Dann Programm vorzeitig beenden. */

   infile = fopen( linebuf, "r" );  /* �ffnen der Datei                 */
   if ( !infile )
   {
      printf( "Fehler beim �ffnen von %s\n", linebuf );
      exit( 1 );
   }

                                    /* Lesen einer Zeile                */
   while ( fgets(linebuf, sizeof( linebuf ), infile ))
   {
      printf( "%s", linebuf );      /* Ausgabe                          */

                                    /* Pr�fung des Pufferendes und      */
                                    /* Entfernen des abschlie�enden \n  */
      i = strlen( linebuf );        /* L�nge der Zeile                  */
      if ( linebuf[i - 1] != '\n' ) /* Zeilenvorschub letztes Zeichen?  */
         printf( "Beginn einer �berlangen Zeile:\n\t%70s\n", linebuf );
      else
         linebuf[i - 1] = NUL;      /* Zeilenvorschub ersetzen          */

      lineptr = nextword( linebuf );/* Ermitteln des ersten Wortanfangs */

      while ( *lineptr )
      {
         i = wordlen( lineptr );    /* Bestimmung der Wortl�nge         */
         if ( i > MAXWORDLEN )      /* gr��er als MAXWORDLEN?           */
            longwords++;            /* ja - wird als "langes Wort"      */
         else                       /* gespeichert                      */
            wordlens[i]++;          /* sonst den entsprechenden Z�hler  */
                                    /* erh�hen                          */

         lineptr += i;              /* Zeiger um die Wortl�nge erh�hen  */

         lineptr = nextword( lineptr ); /* gibt es ein weiteres Wort?   */
      }
   }

   printf( "  L�nge  Anzahl\n" );   /* Ausgabe der Wortl�ngen           */
   for ( i = 1; i <= MAXWORDLEN; i++ )
      printf( "  %5d %5d\n", i, wordlens[i] );
   printf( "�berlange W�rter: %5d\n", longwords );

   fclose(infile);                  /* Schlie�en der Datei und          */
   exit( 0 );                       /* Programmende                     */
}

/* -------------------------------------------------------------------- */
/*       char *nextword( char *lineptr );                               */
/*                                                                      */
/*       Funktion liefert einen Zeiger auf den Start des n�chsten       */
/*       'Wortes' in einer Zeile.                                       */
/* -------------------------------------------------------------------- */

char *nextword( char *lineptr )
{
   while ( *lineptr == SPACE )      /* Solange der Zeiger auf ein Leer- */
      lineptr++;                    /* zeichen zeigt, wird er erh�ht    */

   return( lineptr );               /* Ergebnis: Zeiger auf das         */
}                                   /* n�chste "Nicht-Leerzeichen"      */

/* -------------------------------------------------------------------- */
/*       int wordlen( char *wordstart );                                */
/*                                                                      */
/*       Funktion berechnet die L�nge eines 'Wortes'.                   */
/* -------------------------------------------------------------------- */

int wordlen( char *wordstart )
{
   char *charptr;                   /* Arbeitszeiger                    */

   charptr = wordstart;             /* Arbeitszeiger auf den �ber-      */
                                    /* gebenen Startwert                */

                                    /* Solange das "momentane" Zeichen  */
                                    /* != NUL und != SPACE ist:         */
   while ( *charptr & *charptr != SPACE )
      charptr++;                    /* Arbeitszeiger auf das n�chste    */
                                    /* Zeichen                          */

   return( charptr - wordstart );   /* Ergebnis:                        */
}                                   /* Wortende - Start = L�nge         */

/* -------------------------------------------------------------------- */
/*       Ende WCOUNTB.C                                                 */
/* -------------------------------------------------------------------- */
