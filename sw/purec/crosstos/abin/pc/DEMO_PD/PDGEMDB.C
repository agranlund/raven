/************************************************************************/
/*                                                                      */
/*       PDGEMDB.C                                                      */
/*                                                                      */
/*       Ein Beispielprogramm fÅr die Verwendung des Pure Debuggers.    */
/*                                                                      */
/*       ACHTUNG:       DIESES PROGRAMM ENTHéLT EINIGE FEHLER!!!        */
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

#define ME_HIDE               0
#define ME_SHOW               1
#define ME_NORM               1

#define RSC_NAME              "PDTEST.RSC"

/* -------------------------------------------------------------------- */
/*       Typendefinition.                                               */
/* -------------------------------------------------------------------- */

typedef enum
   {
      FALSE,
      TRUE
   }
      boolean;

/* -------------------------------------------------------------------- */
/*       Globale Variablen.                                             */
/* -------------------------------------------------------------------- */

static OBJECT  *M_tree;
static boolean *Done;

/* -------------------------------------------------------------------- */
/*       Funktionsprototypen.                                           */
/* -------------------------------------------------------------------- */

void gem_prg( void );
void hndl_event( int * msgbuff );

/* -------------------------------------------------------------------- */
/*       void gem_prg( void );                                          */
/*                                                                      */
/*       Hier steht der Quelltext fÅr ein beliebiges GEM-Programm.      */
/*       (Hier Anzeigen eines MenÅs.)                                   */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void gem_prg( void )
{
   static boolean done = FALSE;           /* 'Mach weiter'-Flag.        */
          int     msgbuff[8];             /* Mitteilungspuffer.         */

   *Done = done;                          /* Programm noch nicht fertig */

   if ( rsrc_load( RSC_NAME ) > 0 )       /* Laden des Resource-Files.  */
   {
      rsrc_gaddr( 0, 0, &M_tree );        /* Startadresse des MenÅs     */
                                          /* bestimmen.                 */
      graf_mouse( M_OFF, 0 );             /* Maus ausblenden.           */
      menu_bar( M_tree, ME_SHOW );        /* MenÅ anzeigen.             */
      graf_mouse( M_ON, 0 );
      graf_mouse( ARROW, 0 );             /* Mauszeiger ist der Pfeil.  */

      do
      {
         evnt_mesag( msgbuff );           /* Ereignis abwarten.         */

         hndl_event( msgbuff );           /* Ergebnis auswerten.        */
      }
      while ( !done );                    /* Solange nichts getan ist!  */

      graf_mouse( M_OFF,0 );
      menu_bar( M_tree, ME_HIDE );        /* MenÅ verschwinden lassen.  */
      graf_mouse( M_ON, 0 );
      rsrc_free( );                       /* Durch Rsc-Datei belegten   */
   }                                      /* Speicher freigeben.        */
   else
      form_alert( 1, "[3][Rsc-Datei nicht gefunden.][Abbruch]" );
}

/* -------------------------------------------------------------------- */
/*       void hndl_event( int *msgbuff );                               */
/*                                                                      */
/*       Auswerten mîglicher Ereignisse.                                */
/*                                                                      */
/*       -> msgbuff              Zeiger auf Puffer mit Ergebnis des     */
/*                               GEM-Ereignisalarms.                    */
/*                                                                      */
/*       <- Done                 (globale Varibale)                     */
/*                               TRUE  - Programmabbruch                */
/*                               FALSE - sonst                          */
/* -------------------------------------------------------------------- */

void hndl_event( int *msgbuff )
{
   *Done = TRUE;

   if ( msgbuff[0] == MN_SELECTED )       /* MenÅpunkt wurde ausgewÑhlt */
   {
      if ( msgbuff[4] != 16 )             /* Ungleich 16 ist alles, nur */
                                          /* nicht das Ende!            */
         form_alert( 1, "[0][ Pure Debugger Demo| |(c) 1989 \
Borland Int.  |All rights reserved.][ Ok ]" );
      else
         *Done = FALSE;                   /* sonst ist es vollbracht.   */

      menu_tnormal( M_tree, msgbuff[3], ME_NORM );
   }                                      /* MenÅtitel normal zeichnen. */
}

/* -------------------------------------------------------------------- */
/*       Ende von PDGEMDB.C                                             */
/* -------------------------------------------------------------------- */
