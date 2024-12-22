/*      SETJMP.H

        Context Switch Definition Includes

        Copyright (c) Borland International 1990
        All Rights Reserved.
*/


#if !defined( __SETJMP )
#define __SETJMP


typedef long jmp_buf[16];

void    longjmp(jmp_buf jmp_buffer, int return_value);
int     setjmp(jmp_buf jmp_buffer);


#endif

/************************************************************************/
