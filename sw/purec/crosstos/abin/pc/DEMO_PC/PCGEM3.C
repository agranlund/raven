/************************************************************************/
/*                                                                      */
/*       Beispielprogramm:                              EVNT_MESAG( )   */
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

#define RSC_NAME              "PCTEST.RSC"

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

OBJECT *M_tree;

/* -------------------------------------------------------------------- */
/*       Funktionsprototypen.                                           */
/* -------------------------------------------------------------------- */

void gem_prg( void );

/* -------------------------------------------------------------------- */
/*       void gem_prg( void );                                          */
/*                                                                      */
/*       Beispiel fÅr die Verwendung der Funktion evnt_mesag( ) in      */
/*       einem GEM-Programm.                                            */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void gem_prg( void )
{
   int     msgbuff[8];                    /* Mitteilungspuffer.         */
   boolean done = FALSE;                  /* 'Mach weiter'-Flag.        */

   if ( rsrc_load( RSC_NAME ) > 0 )       /* Es kann auch ein anderes   */
   {                                      /* Rsc-File geladen werden.   */
      rsrc_gaddr( 0, 0, &M_tree );        /* Startadresse des Baumes    */
                                          /* bestimmen.                 */
      graf_mouse( M_OFF, 0 );             /* Maus ausblenden.           */
      menu_bar( M_tree, ME_SHOW );        /* MenÅ anzeigen.             */
      graf_mouse( M_ON, 0 );
      graf_mouse( ARROW, 0 );             /* Mauszeiger ist der Pfeil.  */

      do
      {
         evnt_mesag( msgbuff );           /* Ereignis abwarten.         */

         if ( msgbuff[0] == MN_SELECTED ) /* MenÅpunkt wurde ausgeÑhlt. */
         {
            if ( msgbuff[4] < 16 )        /* Kleiner 16 ist immer About */
                                          /* bzw. Accessory-Eintrag.    */
               form_alert( 1, "[0][     Pure C Demo| |(c) 1989 \
Borland Int.  |All rights reserved.][ Ok ]" );
            else                          /* Wenn er eine hîhere Nummer */
               done = TRUE;               /* besitzt, dann ist es voll- */
                                          /* bracht.                    */

            menu_tnormal( M_tree, msgbuff[3], ME_NORM );
         }                                /* MenÅtitel normal zeichnen. */
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
/*       Ende der Beispielprogramms fÅr EVNT_MESAG( ).                  */
/* -------------------------------------------------------------------- */
