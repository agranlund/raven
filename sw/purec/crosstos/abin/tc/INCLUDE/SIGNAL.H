/*      SIGNAL.H

        Signal Definitions

        Copyright (c) Borland International 1990
        All Rights Reserved.
*/


#if  !defined( __SIGNAL__ )
#define __SIGNAL__


#define SIGABRT         80      /* ANSI, abort */
#define SIGTERM         81      /* ANSI, termination */
#define SIGFPE          82      /* ANSI, floating point error */
#define SIGINT          83      /* ANSI, break request */
#define SIGALRM         84      /* UNIX, Alarm */
#define SIGKILL         85      /* UNIX, Kill Process */

#define SIGSEGV          2      /* ANSI, bus error (segment violation) */
#define SIGADR           3      /* adress error */
#define SIGILL           4      /* ANSI, illegal instruction */
#define SIGDIV0          5      /* division by zero */
#define SIGCHK           6      /* CHK */
#define SIGTRAPV         7      /* TRAPV */
#define SIGPRIV          8      /* privilege violation */
#define SIGTRACE         9      /* TRACE */
#define SIGLINEA        10      /* LINEA */
#define SIGLINEF        11      /* LINEF */
#define SIGSPURI        24      /* spurious interrupt */
#define SIGAUTO1        25      /* auto interrupt #1 */
#define SIGAUTO2        26      /* auto interrupt #2, Hblank */
#define SIGAUTO3        27      /* auto interrupt #3 */
#define SIGAUTO4        28      /* auto interrupt #4, Vblank */
#define SIGAUTO5        29      /* auto interrupt #5 */
#define SIGAUTO6        30      /* auto interrupt #6 */
#define SIGAUTO7        31      /* auto interrupt #7 */
#define SIGTRAP0        32      /* TRAP # 0 */
#define SIGTRAP1        33      /* TRAP # 1, GEMDOS */
#define SIGTRAP2        34      /* TRAP # 2, AES/VDI */
#define SIGTRAP3        35      /* TRAP # 3 */
#define SIGTRAP4        36      /* TRAP # 4 */
#define SIGTRAP5        37      /* TRAP # 5 */
#define SIGTRAP6        38      /* TRAP # 6 */
#define SIGTRAP7        39      /* TRAP # 7 */
#define SIGTRAP8        40      /* TRAP # 8 */
#define SIGTRAP9        41      /* TRAP # 9 */
#define SIGTRAP10       42      /* TRAP #10 */
#define SIGTRAP11       43      /* TRAP #11 */
#define SIGTRAP12       44      /* TRAP #12 */
#define SIGTRAP13       45      /* TRAP #13, BIOS */
#define SIGTRAP14       46      /* TRAP #14, XBIOS */
#define SIGTRAP15       47      /* TRAP #15 */
#define SIGFPU0         48      /* FPU (reserved) */
#define SIGFPU1         49      /* FPU Inexact result (reserved) */
#define SIGFPU2         50      /* FPU Divide by zero (reserved) */
#define SIGFPU3         51      /* FPU (reserved) */
#define SIGFPU4         52      /* FPU (reserved) */
#define SIGFPU5         53      /* FPU (reserved) */
#define SIGFPU6         54      /* FPU signaling NaN (reserved) */
#define SIGMMU0         56      /* MMU (reserved) */
#define SIGMMU1         57      /* MMU (reserved) */
#define SIGMMU2         58      /* MMU (reserved) */
#define SIGMFP0         64      /* MFP  0, CENTRONICS */
#define SIGMFP1         65      /* MFP  1, RS232 CD */
#define SIGMFP2         66      /* MFP  2, RS232 CTS */
#define SIGMFP3         67      /* MFP  3, Blitter */
#define SIGMFP4         68      /* MFP  4, RS232 Baud Rate */
#define SIGMFP5         69      /* MFP  5, 200Hz Clock (enabled) */
#define SIGMFP6         70      /* MFP  6, KEYBD, MIDI (enabled) */
#define SIGMFP7         71      /* MFP  7, FDC/HDC */
#define SIGMFP8         72      /* MFP  8, Hsync */
#define SIGMFP9         73      /* MFP  9, RS232 transmit error (ena.) */
#define SIGMFP10        74      /* MFP 10, RS232 transmit buffer empty  (ena.)*/
#define SIGMFP11        75      /* MFP 11, RS232 receive error (ena.) */
#define SIGMFP12        76      /* MFP 12, RS232 receive buffer full  (ena.)*/
#define SIGMFP13        77      /* MFP 13, Timer A */
#define SIGMFP14        78      /* MFP 14, RS232 RI */
#define SIGMFP15        79      /* MFP 15, Monitor */

typedef void (*sigfunc_t)( int );

#define SIG_SYS         (sigfunc_t)0L
#define SIG_DFL         (sigfunc_t)-1L
#define SIG_IGN         (sigfunc_t)-2L
#define SIG_ERR         (sigfunc_t)-3L

sigfunc_t signal( int sig, sigfunc_t func );
int raise( int sig );

typedef int sig_atomic_t;

#endif

/************************************************************************/
