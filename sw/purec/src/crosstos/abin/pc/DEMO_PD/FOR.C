/************************************************************************/
/*                                                                      */
/*       FOR.C                                                          */
/*                                                                      */
/*       Ein Beispielprogramm fÅr die Verwendung des Pure Debuggers.    */
/*                                                                      */
/*       Copyright (c) 1989 Borland International.                      */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>

void main( void )
{
   int f, fac, i;

   printf( "\nZahl eingeben ( >= 1 ) : " );
   scanf( "%d", &f );

   if ( f >= 1 )
   {
      for( fac = 1, i = 1;
                           i <= f; i++ )
      {
         fac = fac * i;
      }
      printf( "\nFakultÑt von %d ist %d.\n", f, fac );
      exit( 0 );
   }
   else
      exit( -1 );
}

/* -------------------------------------------------------------------- */
/*       Ende FOR.C                                                     */
/* -------------------------------------------------------------------- */

