#include <stdio.h>

void f( void )
{
   int i = 1;

   i = 2;
   printf( "f1:   i = %d\n", i );
}

void main( void )
{
   int i = 0;

   f( );
   i = 3;
   printf( "main: i = %d\n", i );
}

