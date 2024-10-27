#include <gem.h>

#include <mintbind.h>
#include <cookie.h>
#include <mint/ssystem.h>

#ifdef __GNUC__
/*
 * This special version of Ssystem(S_INQUIRE) is used
 * to ensure 68000/coldfire compatible code
 */
static __attribute_noinline__ long ssystem_inq(void)
{
	register long retvalue __asm__("d0");
	register short _a  __asm__("d0") = (short)(S_INQUIRE);
	register long _b  __asm__("d1") = 0;
	
	__asm__ volatile
	(
		"movl	%4,%%sp@-\n\t"
		"movl	%3,%%sp@-\n\t"
		"movw	%2,%%sp@-\n\t"
		"movw	%1,%%sp@-\n\t"
		"trap	#1\n\t"
		"lea	%%sp@(12),%%sp"
	: "=r"(retvalue) /* outputs */
	: "g"(0x154), "r"(_a), "r"(_b), "r"(_b) /* inputs  */
	: __CLOBBER_RETURN("d0") "d2", "a0", "a1", "a2", "cc" /* clobbered regs */
	  AND_MEMORY
	);
	return retvalue;
}
#else
#define ssystem_inq() Ssystem(S_INQUIRE, 0l, 0l)
#endif


short __has_no_ssystem(void)
{
	return ssystem_inq() == 0 ? __ck_zero : __ck_one;
}
