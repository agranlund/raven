#undef assert

#ifndef NDEBUG

#ifndef __STDLIB
#include <stdlib.h>
#endif

#ifndef __STDIO
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* This prints an "Assertion failed" message and aborts.  */
void __assert_fail (const char *__assertion,
			   const char *__file,
			   unsigned int __line,
			   const char *__function)
     __attribute__ ((__noreturn__));

# define assert(expr)							      \
  ((void) ((expr) ? 0 :							      \
	   (__assert_fail (#expr,				      \
			   __FILE__, __LINE__, NULL), 0)))

#ifdef __cplusplus
}
#endif

#else
#define assert(expr)
#endif /* NDEBUG */
