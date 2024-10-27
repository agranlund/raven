/*
 *	MACROS.H	commonly useful macros
 */

#ifndef	_MACROS_H
#define	_MACROS_H

#ifdef __GNUC__	
/* with GNUC we will use safe versions, these may look like that they
 * have overhead, but they do not -- trust me!
 */

/* absolute value for any type of number */
#define abs(a) \
    ({__typeof__ (a) _a = (a);     \
	      _a  < ((__typeof__ (a))0) ? -(_a) : _a; })

/* maximum and minimum for any type of number */
#define max(a,b) \
    ({__typeof__ (a) _a = (a); __typeof__ (b) _b = (b);  \
	      _a > _b ? _a : _b; })

#define min(a,b) \
    ({__typeof__ (a) _a = (a); __typeof__ (b) _b = (b);     \
	      _a < _b ? _a : _b; })

/* swap any objects (even identically typed structs!) */
/* WARNING: not safe */
#define swap(a,b) \
    ({__typeof__ (a) _t;  \
	      _t = (a); (a) = (b); (b) = _t; })

#else	/* be careful !! */

/* absolute value for any type of number */
#define	abs(x)		((x)<0?(-(x)):(x))

/* maximum and minimum for any type of number */
#define max(x,y)   	(((x)>(y))?(x):(y))
#define	min(x,y)   	(((x)<(y))?(x):(y))

/* swap any objects which can be XORed */
#define	swap(a,b)	((a)=(a)^((b)=(b)^((a)=(a)^(b))))

#endif /* __GNUC__ */

/* lo and hi byte of a word */
#define	lobyte(x)	(((unsigned char *)&(x))[1])
#define	hibyte(x)	(((unsigned char *)&(x))[0])

/* lo and hi word of a long */
#define	loword(x)	(((unsigned short *)&(x))[1])
#define	hiword(x)	(((unsigned short *)&(x))[0])

/* These are often useful.  Since this file is non-standard anyway we
   can afford to have them here.  Summary:
   
   	ROds(offset, x): Bit-rotate argument x of size s (one of
   	                 L for unsigned long, W for unsigned short
   	                 or B for unsigned char) by in direction d
   	                 (one of L for left or R for right) by
   	                 offset bits.
   
   The same macros are available with a 1 appended.  Semantics are
   the same except that offset is always 1.  */

#if defined(__GNUC__) && !defined(__mcoldfire__)
#define ROLL(offset, x)	\
({ unsigned long __arg = (x);	\
    __asm__ ("roll %2,%1": "=d" (__arg) : "0" (__arg), "di" (offset) : "cc"); \
    __arg; })

#define RORL(offset, x)	\
({ unsigned long __arg = (x);	\
    __asm__ ("rorl %2,%1": "=d" (__arg) : "0" (__arg), "di" (offset) : "cc"); \
    __arg; })

#define ROLW(offset, x)	\
({ unsigned short __arg = (x);	\
    __asm__ ("rolw %2,%1": "=d" (__arg) : "0" (__arg), "di" (offset) : "cc"); \
    __arg; })

#define RORW(offset, x)	\
({ unsigned short __arg = (x);	\
    __asm__ ("rorw %2,%1": "=d" (__arg) : "0" (__arg), "di" (offset) : "cc"); \
    __arg; })

#define ROLB(offset, x)	\
({ unsigned char __arg = (x);	\
    __asm__ ("rolb %2,%1": "=d" (__arg) : "0" (__arg), "di" (offset) : "cc"); \
    __arg; })

#define RORB(offset, x)	\
({ unsigned char __arg = (x);	\
    __asm__ ("rorb %2,%1": "=d" (__arg) : "0" (__arg), "di" (offset) : "cc"); \
    __arg; })

#else

#if defined(__PUREC__) || defined(__TURBOC__)

static __inline unsigned long  __ROLL(unsigned long x, int offset) 0xe3b8;
static __inline unsigned short __ROLW(unsigned short x, int offset) 0xe378;
static __inline unsigned char  __RORB(unsigned char x, int offset) 0xe338;
static __inline unsigned long  __RORL(unsigned long x, int offset) 0xe2b8;
static __inline unsigned short __RORW(unsigned short x, int offset) 0xe278;
static __inline unsigned char  __RORB(unsigned char x, int offset) 0xe238;

#define ROLL(offset, x)	__ROLL(x, offset)
#define RORL(offset, x)	__RORL(x, offset)
#define ROLW(offset, x)	__ROLW(x, offset)
#define RORW(offset, x)	__RORW(x, offset)
#define ROLB(offset, x)	__ROLB(x, offset)
#define RORB(offset, x)	__RORB(x, offset)

#else

static __inline unsigned long ROLL(int offset, unsigned long x)
{
	return (x >> (32 - offset)) | (x << offset);
}

static __inline unsigned short ROLW(int offset, unsigned short x)
{
	return (x >> (16 - offset)) | (x << offset);
}

static __inline unsigned char ROLB(int offset, unsigned char x)
{
	return (x >> (8 - offset)) | (x << offset);
}

static __inline unsigned long RORL(int offset, unsigned long x)
{
	return (x >> offset) | (x << (32 - offset));
}

static __inline unsigned short RORW(int offset, unsigned short x)
{
	return (x >> offset) | (x << (16 - offset));
}

static __inline unsigned char RORB(int offset, unsigned char x)
{
	return (x >> offset) | (x << (8 - offset));
}

#endif

#endif

#if defined(__PUREC__) || defined(__TURBOC__)

static __inline unsigned long ROLL1(unsigned long x) 0xe398;
static __inline unsigned short ROLW1(unsigned short x) 0xe358;
static __inline unsigned char ROLB1(unsigned char x) 0xe318;
static __inline unsigned long RORL1(unsigned long x) 0xe298;
static __inline unsigned short RORW1(unsigned short x) 0xe258;
static __inline unsigned char RORB1(unsigned char x) 0xe218;

#else

#define ROLL1(x)	ROLL(1, x)
#define RORL1(x)	RORL(1, x)
#define ROLW1(x)	ROLW(1, x)
#define RORW1(x)	RORW(1, x)
#define ROLB1(x)	ROLB(1, x)
#define RORB1(x)	RORB(1, x)

#endif

#endif /* _MACROS_H */
