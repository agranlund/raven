#ifndef __sigset_t_defined
#define __sigset_t_defined 1

/* A `sigset_t' has a bit for each signal.  */
typedef unsigned long int __sigset_t;

#if defined __USE_POSIX
/* A set of signals to be blocked, unblocked, or waited for.  */
typedef __sigset_t sigset_t;
#endif

#endif

#ifndef __sig_atomic_t_defined
#define __sig_atomic_t_defined 1

#if !defined(__MSHORT__) && !defined(__PUREC__)
typedef int __sig_atomic_t;
#else
typedef long int __sig_atomic_t;
#endif

/* An integral type that can be modified atomically, without the
   possibility of a signal arriving in the middle of the operation.  */
typedef __sig_atomic_t sig_atomic_t;

#endif
