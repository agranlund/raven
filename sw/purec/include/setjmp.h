/*
 *	ISO C99 Standard: 7.13 Nonlocal jumps	<setjmp.h>
 */

#ifndef _SETJMP_H
#define _SETJMP_H 1

#ifndef	_FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

#include <bits/setjmp.h>
#include <bits/sigset.h>

/* Calling environment, plus possibly a saved signal mask.  */
struct __jmp_buf_tag
  {
    /* NOTE: The machine-dependent definitions of `__sigsetjmp'
       assume that a `jmp_buf' begins with a `__jmp_buf' and that
       `__mask_was_saved' follows it.  Do not move these members
       or add others before it.  */
    __jmp_buf __jmpbuf;		/* Calling environment.  */
    __int32_t __mask_was_saved;	/* Saved the signal mask?  */
    __sigset_t __saved_mask;	/* Saved signal mask.  */
  };


__BEGIN_NAMESPACE_STD

typedef struct __jmp_buf_tag jmp_buf[1];

/* Store the calling environment in ENV, also saving the signal mask.
   Return 0. */
int setjmp(jmp_buf __env) __THROWNL;

__END_NAMESPACE_STD

/* Store the calling environment in ENV, also saving the
   signal mask if SAVEMASK is nonzero.  Return 0.
   This is the internal name for `sigsetjmp'. */
extern int __sigsetjmp(jmp_buf __env, int __savemask) __THROWNL;
extern int sigsetjmp  (jmp_buf env, int savemask) __THROWNL;

#if !defined(__FAVOR_BSD) && !defined(_PUREC_SOURCE)
/* Store the calling environment in ENV, not saving the signal mask.
   Return 0. */
extern int _setjmp (jmp_buf __env) __THROWNL;

/* Do not save the signal mask.  This is equivalent to the `_setjmp'
   BSD function.  */
# define setjmp(env)	_setjmp (env)
#else
/* We are in 4.3 BSD-compatibility mode in which `setjmp'
   saves the signal mask like `sigsetjmp (ENV, 1)'.  We have to
   define a macro since ISO C says `setjmp' is one.  */
# define setjmp(env)	setjmp (env)
#endif


__BEGIN_NAMESPACE_STD

/* Jump to the environment saved in ENV, making the
   `setjmp' call there return VAL, or 1 if VAL is 0.  */
void longjmp(jmp_buf __env, int __val) __THROWNL __NORETURN;

__END_NAMESPACE_STD

#if defined __USE_BSD || defined __USE_XOPEN
/* Same.  Usually `_longjmp' is used with `_setjmp', which does not save
   the signal mask.  But it is how ENV was saved that determines whether
   `longjmp' restores the mask; `_longjmp' is just an alias.  */
extern void _longjmp (struct __jmp_buf_tag __env[1], int __val) __THROWNL __NORETURN;
#endif


#ifdef __USE_POSIX
/* Use the same type for `jmp_buf' and `sigjmp_buf'.
   The `__mask_was_saved' flag determines whether
   or not `longjmp' will restore the signal mask.  */
typedef struct __jmp_buf_tag sigjmp_buf[1];

/* Store the calling environment in ENV, also saving the
   signal mask if SAVEMASK is nonzero.  Return 0.  */
# define sigsetjmp(env, savemask) __sigsetjmp (env, savemask)

/* Jump to the environment saved in ENV, making the
   sigsetjmp call there return VAL, or 1 if VAL is 0.
   Restore the signal mask if that sigsetjmp call saved it.
   This is just an alias `longjmp'.  */
extern void siglongjmp (sigjmp_buf __env, int __val) __THROWNL __NORETURN;
#endif


/* Define helper functions to catch unsafe code.  */
#if __USE_FORTIFY_LEVEL > 0
# include <bits/setjmp2.h>
#endif

__END_DECLS

#endif /* setjmp.h */
