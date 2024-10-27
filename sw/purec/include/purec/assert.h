/*
 * moved out of <assert.h> because otherwise assert.h
 * is include-fixed due to the reference of stderr
 */

#ifndef	__need_assert
# error "Never use <purec/assert.h> directly; include <assert.h> instead."
#endif

#undef __need_assert
#define assert( expr )\
       ((void)((expr)||(fprintf( stderr, \
       "\nAssertion failed: %s, file %s, line %d\n",\
        #expr, __FILE__, __LINE__ ),\
        ((int (*)(void))abort)())))
