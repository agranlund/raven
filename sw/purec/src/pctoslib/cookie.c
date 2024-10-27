/*****************************************************************************
 *	TOS/COOKIE.C
 *****************************************************************************/

/*
 * WARNING: functions here are called from the startup code before knowing the
 * actual CPU type. So it must be compatible with any processor (including
 * 680x0 and ColdFire models). So it must NOT be compiled with -m68020
 * or similar
 */

#include <gem.h>

#include <mintbind.h>
#include "cookie.h"
#include <mint/ssystem.h>

typedef struct {
	union {
		unsigned long id;
		long end;
	} cookie;
	long cookie_value;
} COOKIE;

#ifdef __GNUC__
/*
 * This special version of Ssystem() is used
 * to ensure 68000/coldfire compatible code
 */
static volatile const void *sysvar;

static __attribute_noinline__ long ssystem_getlval(void)
{
	register long retvalue __asm__("d0");
	register short _a __asm__("d0") = (short)(S_GETLVAL);
	register long _b __asm__("d1") = (long)sysvar;
	register long _c __asm__("d2") = 0;
	
	__asm__ volatile
	(
		"movl	%4,%%sp@-\n\t"
		"movl	%3,%%sp@-\n\t"
		"movw	%2,%%sp@-\n\t"
		"movw	%1,%%sp@-\n\t"
		"trap	#1\n\t"
		"lea	%%sp@(12),%%sp"
	: "=r"(retvalue) /* outputs */
	: "g"(0x154), "r"(_a), "r"(_b), "r"(_c) /* inputs  */
	: __CLOBBER_RETURN("d0") "a0", "a1", "a2", "cc" /* clobbered regs */
	  AND_MEMORY
	);
	return retvalue;
}

static long __attribute_noinline__ fetch_sysvar(volatile const void *var)
{
	sysvar = *(volatile const void *volatile const *)(&var);
	return ssystem_getlval();
}

static __attribute_noinline__ long ssystem_getcookie(long val)
{
	register long retvalue __asm__("d0");
	register short _a __asm__("d0") = (short)(S_GETCOOKIE);
	register long _b __asm__("d1") = (long)sysvar;
	register long _c __asm__("d2") = val;
	
	__asm__ volatile
	(
		"movl	%4,%%sp@-\n\t"
		"movl	%3,%%sp@-\n\t"
		"movw	%2,%%sp@-\n\t"
		"movw	%1,%%sp@-\n\t"
		"trap	#1\n\t"
		"lea	%%sp@(12),%%sp"
	: "=r"(retvalue) /* outputs */
	: "g"(0x154), "r"(_a), "r"(_b), "r"(_c) /* inputs  */
	: __CLOBBER_RETURN("d0") "a0", "a1", "a2", "cc" /* clobbered regs */
	  AND_MEMORY
	);
	return retvalue;
}

static long __attribute_noinline__ fetch_cookie(volatile const void *var, long val)
{
	sysvar = *(volatile const void *volatile const *)(&var);
	return ssystem_getcookie(val);
}

#else
#define fetch_sysvar(var) Ssystem(S_GETLVAL, (long)(var), 0l)
#define fetch_cookie(id, val) Ssystem(S_GETCOOKIE, (long)(id), (long)(val))
#endif


#ifdef __GNUC__
/*
 * This special version of Setexc() is used
 * to ensure 68000/coldfire compatible code
 */
static __attribute_noinline__ long vec_inq(short vec)
{
	register long retvalue __asm__("d0"); \
	register short _a __asm__("d0") = vec; \
	register long _b __asm__("d1") = (long)__ck_minusone; \
	 \
	__asm__ volatile \
	( \
		"movl	%3,%%sp@-\n\t" \
		"movw	%2,%%sp@-\n\t" \
		"movw	%1,%%sp@-\n\t" \
		"trap	#13\n\t" \
		"addql	#8,%%sp" \
	: "=r"(retvalue) /* outputs */ \
	: "g"(0x5), "r"(_a), "r"(_b) /* inputs  */ \
	: __CLOBBER_RETURN("d0") "d2", "a0", "a1", "a2", "cc" /* clobbered regs */ \
	  AND_MEMORY \
	); \
	return retvalue;
}
#else
#define vec_inq(vec) Setexc(vec, VEC_INQUIRE)
#endif


static COOKIE *_get_jarptr(void)
{
	COOKIE *p;
	
	if (__has_no_ssystem())
	{
		/*
		 * cookie jar ptr is longword aligned, thus
		 * we can use Setexc to fetch its value
		 */
		p = (COOKIE *)vec_inq(0x5A0 / 4);
	} else
	{
		long val;
		
		val = fetch_sysvar((volatile const void *)(0x5a0));
		if (val < 0)
			val = 0;
		p = (COOKIE *)val;
	}
	return p;
}

/******************************************************************************/
/* Cookie_JarInstalled()                                                      */
/* -------------------------------------------------------------------------- */
/* See if the cookie jar is installed.                                        */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/*                   none                                                     */
/* -------------------------------------------------------------------------- */
/* Return value:                                                              */
/*                   TRUE if jar is installed                                 */
/******************************************************************************/

int Cookie_JarInstalled(void)
{
	return _get_jarptr() == 0 ? __ck_zero : __ck_one;
}

/******************************************************************************/
/* Cookie_UsedEntries()                                                       */
/* -------------------------------------------------------------------------- */
/* Inquire the number of used cookie jar entries. The number includes         */
/* the null cookie, so a return value of 0 means that there is no             */
/* cookie jar at all.                                                         */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/*                   none                                                     */
/* -------------------------------------------------------------------------- */
/* Return value:                                                              */
/*                   number of used cookie jar entries                        */
/******************************************************************************/

int Cookie_UsedEntries(void)
{
	COOKIE *p;
	int entries = __ck_zero;
	
	p = _get_jarptr();
	if (p != NULL)
	{
		for (;;)
		{
			++entries;
			if (p->cookie.end == 0)
				break;
			p++;
		}
	}
	return entries;
}

/******************************************************************************/
/* Cookie_JarSize()                                                           */
/* -------------------------------------------------------------------------- */
/* Inquire the total number of cookie jar entries.                            */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/*                   none                                                     */
/* -------------------------------------------------------------------------- */
/* Return value:                                                              */
/*                   total number of cookie jar entries                       */
/******************************************************************************/

int Cookie_JarSize(void)
{
	int size = 0;
	COOKIE *p;
	
	p = _get_jarptr();
	if (p != NULL)
	{
		while (p->cookie.end != 0)
		{
			p++;
		}
		size = (int)p->cookie_value;
	}
	return size;
}

/******************************************************************************/
/* Cookie_ResizeJar()                                                         */
/* -------------------------------------------------------------------------- */
/* Resize the cookie jar to the desired size.                                 */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> newsize        desired cookie jar size, number of entries               */
/* -------------------------------------------------------------------------- */
/* Return value:                                                              */
/*                   TRUE if successful                                       */
/******************************************************************************/

int Cookie_ResizeJar(int newsize)
{
	/* NYI */
	UNUSED(newsize);
	return FALSE;
}

/*** ---------------------------------------------------------------------- ***/

static __attribute_noinline__ COOKIE *SearchJar(unsigned long id)
{
	COOKIE *p;
	
	p = _get_jarptr();
	if (p != NULL)
	{
		for (;;)
		{
			if (p->cookie.end == 0)
				return NULL;
			else if (p->cookie.id == id)
				break;
			else
				p++;
		}
	}
	return p;
}

/******************************************************************************/
/* Cookie_ReadJar()                                                           */
/* -------------------------------------------------------------------------- */
/* Read the value of the specified cookie.                                    */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> id             cookie name                                              */
/* <- value          pointer to cookie value (may be NULL)                    */
/* -------------------------------------------------------------------------- */
/* Return value:                                                              */
/*                   TRUE if successful                                       */
/******************************************************************************/

static int __attribute_noinline__ get_cookie(unsigned long id, long *value)
{
	long ret;
	long val;
	
	val = -42;
	ret = fetch_cookie((volatile const void *)id, (long)&val);
	if (ret == __ck_minusone)
		return __ck_zero;
	/*
	 * Backward compatibility for MiNT 1.14.7:
	 * Ssystem() returns cookie value and ignores arg2!!
	 */
	if (val == -42)
		val = ret;
	*value = val;
	return __ck_one;
}


static void __attribute_noinline__ setval(long valp, long val)
{
	*((long *)valp) = val;
}

int Cookie_ReadJar(unsigned long id, long *value)
{
	long valp = (long)value;
	if (valp == 0)
		valp = (long)&id;

	if (__has_no_ssystem())
	{
		COOKIE *p;
		
		p = SearchJar(id);
		if (p != NULL)
		{
			setval(valp, p->cookie_value);
			return __ck_one;
		}
	} else
	{
		long val;
		
		if (get_cookie(id, &val))
		{
			setval(valp, val);
			return __ck_one;
		}
	}
	return __ck_zero;
}

/******************************************************************************/
/* Cookie_WriteJar()                                                          */
/* -------------------------------------------------------------------------- */
/* Insert a new entry into the cookie jar. If no cookie jar exists            */
/* or the current cookie jar is full, a new, bigger cookie jar is             */
/* installed. The increment in size can be set using Cookie_SetOptions.       */
/* -------------------------------------------------------------------------- */
/* Parameter:                                                                 */
/* -> id             cookie name                                              */
/* -> value          cookie value                                             */
/* -------------------------------------------------------------------------- */
/* Return value:                                                              */
/*                   TRUE if successful                                       */
/******************************************************************************/

int Cookie_WriteJar(unsigned long id, long value)
{
	COOKIE *p;
	
	p = SearchJar(id);
	if (p != NULL)
	{
		p->cookie_value = value;
		return __ck_one;
	}
	return __ck_zero;
}


int Getcookie(long cookie, long *val)
{
	return Cookie_ReadJar(cookie, val) ? __ck_zero : __ck_one;
}
