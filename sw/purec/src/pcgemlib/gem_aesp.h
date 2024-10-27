#ifndef _GEM_AES_P_
# define _GEM_AES_P_

# ifndef _GEMLIB_H_
#  include "mt_gem.h"
# endif


#if defined(__GNUC_INLINE__) && (__GNUC__ > 2 || __GNUC_MINOR__ > 5)

static __inline _WORD _aes_trap (AESPB * aespb)
{
	__asm__ volatile (
		"move.l	%0,d1\n\t"	/* &aespb */
		"move.w	#200,d0\n\t"
		"trap	#2"
		:
		: "g"(aespb)
		: "d0","d1","d2","a0","a1","a2","memory"
	);
	return aespb->intout[0];
}
#define AES_TRAP(aespb) _aes_trap(&aespb)

#else /* no usage of gnu inlines, go the old way */

#define AES_TRAP(aespb) aes(&aespb)

#endif


#define AES_PARAMS(opcode,nintin,nintout,naddrin,naddrout) \
	static _WORD const aes_control[5]={opcode,nintin,nintout,naddrin,naddrout}; \
	_WORD			aes_intin[AES_INTINMAX];			  \
	_WORD			aes_intout[AES_INTOUTMAX];			  \
	void *			aes_addrin[AES_ADDRINMAX];			  \
	void *			aes_addrout[AES_ADDROUTMAX];		  \
 														  \
	AESPB aes_params;									  \
  	aes_params.control = aes_control;				  \
  	aes_params.global  = global_aes ? global_aes : aes_global ;				  \
  	aes_params.intin   = aes_intin; 				  \
  	aes_params.intout  = aes_intout;				  \
  	aes_params.addrin  = aes_addrin;				  \
  	aes_params.addrout = aes_addrout


#endif /* _GEM_AES_P_ */


/* special feature for AES bindings: pointer in parameters (for return values)
 * could be NULL (nice idea by Martin Elsasser against dummy variables) 
 */
#define CHECK_NULLPTR 1
