/*
 *        NETACTIVE version number, please check it for later releases
 */

/* Prototypen */

#if defined(__PUREC__) || defined(__AHCC__) || defined(__TURBOC__) || (defined(__GNUC__) && !defined(__mc68000__))

#if defined(__AHCC__)
	#define __GEMDOS(b)  cdecl __syscall__(1,b)
#else
	#define __GEMDOS(b)
#endif

short __GEMDOS(127)	Nactive( void );
short __GEMDOS(126)	Nnodeid( void );
short __GEMDOS(125)	Nlogged( short nn );
long __GEMDOS(124)	Nlock( const char *file );
long __GEMDOS(123)	Nunlock( const char *file );
long __GEMDOS(122)	Nlocked(void );
long __GEMDOS(121)	Nprinter( short nn, short kopf, short dd );
void __GEMDOS(120)	Nreset( void );
short __GEMDOS(119)	Nrecord( short handle, short mm, long offset, long leng );
short __GEMDOS(118)	Nmsg( short rw, char *buf, char *id, short node, short leng );
short __GEMDOS(117)	Nremote( short nn );
void __GEMDOS(116)	Ndisable( void );
short __GEMDOS(115)	Nenable( void );

#undef __GEMDOS

#endif /* __PUREC__ */

#if defined(__GNUC__) && defined(__mc68000__)

#define Nactive()        ((short)trap_1_w(127))
#define Nnodeid()        ((short)trap_1_w(126))
#define Nlogged(a)       ((short)trap_1_ww(125,a))
#define Nlock(a)         trap_1_wl(124,a)
#define Nunlock(a)       trap_1_wl(123,a)
#define Nlocked()        trap_1_w(122)
#define Nprinter(a,b,c)  ((short)trap_1_wwww(121,a,b,c))
#define Nreset()         trap_1_w(120)
#define Nrecord(a,b,c,d) ((short)trap_1_wwwll(119,a,b,c,d))
#define Nmsg(a,b,c,d,e)  ((short)trap_1_wwllww(118,a,b,c,d,e))
#define Nremote(a)       ((short)trap_1_ww(117,a))
#define Ndisable()       trap_1_w(116)
#define Nenable()        trap_1_w(115)

#endif /* __GNUC__ */

#define NETACTIVE 100

/*** V 1.0 Beta ***/

#define NLOCK    1
#define NUNLOCK  0

/* rwabs extended rw-codes */

#define NGETMSG 4                  /* get message                          */
#define NSNDMSG 8                  /* send message                         */          
#define NSNDSCN 5                  /* send screen                          */
#define NGETSCN 9                  /* get screen                           */
#define NRCVSCN 6                  /* ? */
#define NDENIES 7                  /* ? */

/* Nmsg Macros */

#define NMSG_READ     0            /* read message                         */
#define NMSG_WRITE    1            /* send message                         */

/* NET ERROR */

#define NET_ERROR   -36
