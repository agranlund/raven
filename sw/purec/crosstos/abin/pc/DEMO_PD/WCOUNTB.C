/************************************************************************/
/*                                                                      */
/*       WCOUNTB.C                                                      */
/*                                                                      */
/*       Ein Beispielprogramm fÅr die Verwendung des Pure Debuggers.    */
/*                                                                      */
/*       ACHTUNG:       DIESES PROGRAMM ENTHéLT EINIGE FEHLER!!!        */
/*                                                                      */
/*       Copyright (c) 1989 Borland International.                      */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXWORDLEN 16               /* maximale LÑnge eines Wortes      */
#define NUL   ((char)0)             /* Endemarke (Nullzeichen)          */
#define SPACE ((char)0x20)          /* Trennzeichen zwischen Wîrtern    */

char *nextword( char *lineptr );
int  wordlen( char *wordstart );

/* -------------------------------------------------------------------- */
/*       Hauptprogramm                                                  */
/* -------------------------------------------------------------------- */

void main( void )
{
   FILE       *infile;                  /* Eingabedatei                 */
   char       linebuf[1024];            /* "momentane Zeile"            */
   char       *lineptr;                 /* Zeiger auf das erste/nÑchste */
                                        /* Wort in linebuf              */
   int        i;                        /* fÅr Schleifen usw.           */
   static int wordlens[MAXWORDLEN + 1], /* speichert die WortlÑngen     */
              longwords;                /* FÅr Wîrter > MAXWORDLEN      */


   printf( "VORSICHT!\n\n");
   printf( "WCOUNTB.C ist ein öbungsprogramm fÅr Pure Debugger.\n");
   printf( "Wenn Sie dieses Programm nicht vom Debugger aus gestartet\n" );
   printf( "haben, dann drÅcken Sie bitte nur <Return>!\n\n" );
   printf( "Ansonsten hÑngt sich das System auf.\n\n" );
   printf( "Weitere Informationen finden Sie im Benutzerhandbuch.\n\n" );

   printf( "Geben Sie den Namen der Textdatei ein (z.B. TEXT.DAT): " );

   gets( linebuf );                 /* linebuf wird hier "mi·braucht"   */

   if ( !strlen( linebuf ))         /* nur RETURN gedrÅckt?             */
      exit( 1 );                    /* Dann Programm vorzeitig beenden. */

   infile = fopen( linebuf, "r" );  /* ôffnen der Datei                 */
   if ( !infile )
   {
      printf( "Fehler beim ôffnen von %s\n", linebuf );
      exit( 1 );
   }

                                    /* Lesen einer Zeile                */
   while ( fgets(linebuf, sizeof( linebuf ), infile ))
   {
      printf( "%s", linebuf );      /* Ausgabe                          */

                                    /* PrÅfung des Pufferendes und      */
                                    /* Entfernen des abschlie·enden \n  */
      i = strlen( linebuf );        /* LÑnge der Zeile                  */
      if ( linebuf[i - 1] != '\n' ) /* Zeilenvorschub letztes Zeichen?  */
         printf( "Beginn einer Åberlangen Zeile:\n\t%70s\n", linebuf );
      else
         linebuf[i - 1] = NUL;      /* Zeilenvorschub ersetzen          */

      lineptr = nextword( linebuf );/* Ermitteln des ersten Wortanfangs */

      while ( *lineptr )
      {
         i = wordlen( lineptr );    /* Bestimmung der WortlÑnge         */
         if ( i > MAXWORDLEN )      /* grî·er als MAXWORDLEN?           */
            longwords++;            /* ja - wird als "langes Wort"      */
         else                       /* gespeichert                      */
            wordlens[i]++;          /* sonst den entsprechenden ZÑhler  */
                                    /* erhîhen                          */

         lineptr += i;              /* Zeiger um die WortlÑnge erhîhen  */

         lineptr = nextword( lineptr ); /* gibt es ein weiteres Wort?   */
      }
   }

   printf( "  LÑnge  Anzahl\n" );   /* Ausgabe der WortlÑngen           */
   for ( i = 1; i <= MAXWORDLEN; i++ )
      printf( "  %5d %5d\n", i, wordlens[i] );
   printf( "öberlange Wîrter: %5d\n", longwords );

   fclose(infile);                  /* Schlie·en der Datei und          */
   exit( 0 );                       /* Programmende                     */
}

/* -------------------------------------------------------------------- */
/*       char *nextword( char *lineptr );                               */
/*                                                                      */
/*       Funktion liefert einen Zeiger auf den Start des nÑchsten       */
/*       'Wortes' in einer Zeile.                                       */
/* -------------------------------------------------------------------- */

char *nextword( char *lineptr )
{
   while ( *lineptr == SPACE )      /* Solange der Zeiger auf ein Leer- */
      lineptr++;                    /* zeichen zeigt, wird er erhîht    */

   return( lineptr );               /* Ergebnis: Zeiger auf das         */
}                                   /* nÑchste "Nicht-Leerzeichen"      */

/* -------------------------------------------------------------------- */
/*       int wordlen( char *wordstart );                                */
/*                                                                      */
/*       Funktion berechnet die LÑnge eines 'Wortes'.                   */
/* -------------------------------------------------------------------- */

int wordlen( char *wordstart )
{
   char *charptr;                   /* Arbeitszeiger                    */

   charptr = wordstart;             /* Arbeitszeiger auf den Åber-      */
                                    /* gebenen Startwert                */

                                    /* Solange das "momentane" Zeichen  */
                                    /* != NUL und != SPACE ist:         */
   while ( *charptr & *charptr != SPACE )
      charptr++;                    /* Arbeitszeiger auf das nÑchste    */
                                    /* Zeichen                          */

   return( charptr - wordstart );   /* Ergebnis:                        */
}                                   /* Wortende - Start = LÑnge         */

/* -------------------------------------------------------------------- */
/*       Ende WCOUNTB.C                                                 */
/* -------------------------------------------------------------------- */
