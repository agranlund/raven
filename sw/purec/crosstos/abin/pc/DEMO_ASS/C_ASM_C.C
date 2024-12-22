/* C_ASM.C
   Demoprogramm zum Aufruf von Assemblerroutinen aus
   C-Programmen und umgekehrt
*/

#include <stdio.h>

void SumMulFac( int x, int y );    /* definiert in ASM.S */


void fakult( int x )
{
    double y;
    int i;

    printf( "Die FakultÑt von %d ist ", x );
    if( x < 0 )
    {
        puts( "nicht definiert" );
    }
    else
    {
        for( i = 2, y = 1.0; i <= x; i++ )
            y *= i;

        printf( "%lg\n", y );
    }
}

int main( void )
{
    int x, y;

    puts( "Bitte zwei Integerzahlen eingeben:" );
    scanf( "%d %d", &x, &y );
    SumMulFac( x, y );

    puts( "Bitte <RETURN> drÅcken" );
    getchar();
    return( 0 );
}
