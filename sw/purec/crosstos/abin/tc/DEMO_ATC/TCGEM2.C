/************************************************************************/
/*                                                                      */
/*       Beispielprogramm:                              EVNT_KEYBD( )   */
/*                                                                      */
/*       Copyright (c) 1989 Borland International                       */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

/* -------------------------------------------------------------------- */
/*       Include-Files einbinden.                                       */
/* -------------------------------------------------------------------- */

#include <aes.h>

#include "scancode.h"

/* -------------------------------------------------------------------- */
/*       Funktionsprototypen.                                           */
/* -------------------------------------------------------------------- */

void gem_prg( void );

/* -------------------------------------------------------------------- */
/*       void gem_prg( void );                                          */
/*                                                                      */
/*       Beispiel fr die Verwendung der Funktion evnt_keybd( ) in      */
/*       einem GEM-Programm.                                            */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void gem_prg( void )
{
   int kc = 0;

   while ( kc != CNTRL_C )
   {
      kc = evnt_keybd( );
      if ( kc == CNTRL_C )
         form_alert( 1, "[1][Programmende.][Ok]" );
      else
         form_alert( 1, "[2][Programm kann mit|Control-C|verlassen werden][Ok]" );
   }
}

/* -------------------------------------------------------------------- */
/*       Ende der Beispielprogramms fr EVNT_KEYBD( ).                  */
/* -------------------------------------------------------------------- */
