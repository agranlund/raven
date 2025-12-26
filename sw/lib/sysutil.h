#ifndef _LIBSYSUTIL_H_
#define _LIBSYSUTIL_H_
#include <stdbool.h>
#include <stdint.h>

#if defined(__PUREC__)
/* implemented in pcsyslib.lib */

#ifndef inline
#define inline
#endif

extern long     Supmain(int args, char** argv, long(*func)(int, char**));
extern long     SupmainEx(int args, char** argv, long(*func)(int, char**), void* stack);

extern uint16_t SetIPL(uint16_t ipl);
extern uint16_t DisableInterrupts(void);
extern void     RestoreInterrupts(uint16_t sr);
static void     nop(void) 0x4e71;

#elif defined(__GNUC__)

#ifndef cdecl
#define cdecl
#endif

typedef struct { int args; char** argv; long(*func)(int, char**); void* stack; } __s_args_t;

static long __attribute__ ((noinline)) SupmainTrampoline(__s_args_t* args) {
   long ret;
	__asm__ volatile                                \
    (                                               \
        /* a3 = oldssp = Super(0) */                \
        "   move.l  #0,-(%%sp)\n"                   \
        "   move.w  #0x20,-(%%sp)\n"                \
        "   trap    #1\n"                           \
        "   addq.l  #6,%%sp\n"                      \
        "   move.l  d0,%%a3\n"                      \
        "   \n"                                     \
        /* set override stack */                    \
        /* a4 = savesp */                           \
        "   move.l  12(%1),%%d3\n"                  \
        "   cmp.l   #0,%%d3\n"                      \
        "   bne.b   1f\n"                           \
        "   move.l  %%sp,%%d3\n"                    \
        "1: move.l  %%sp,%%a4\n"                    \
        "   and.w   #-16,%%d3\n"                    \
        "   move.l  %%d3,%%sp\n"                    \
        "   \n"                                     \
        /* main(args, argv) */                      \
        "   movem.l %%d1-%%d7/%%a2-%%a6,-(%%sp)\n"  \
        "   move.l  4(%1),-(%%sp)\n"                \
        "   move.l  0(%1),-(%%sp)\n"                \
        "   move.l  8(%1),%%a2\n"                   \
        "   jsr     (%%a2)\n"                       \
        "   addq.l  #8,%%sp\n"                      \
        "   movem.l (%%sp)+,%%d1-%%d7/%%a2-%%a6\n"  \
        "   move.l  %%d0,%0\n"                      \
        "   \n"                                     \
        /* restore savesp */                        \
        "   move.l  %%a4,%%sp\n"                    \
        "   \n"                                     \
        /* Super(oldssp) */                         \
        "   move.l  %%a3,-(%%sp)\n"                 \
        "   move.w  #0x20,-(%%sp)\n"                \
        "   trap    #1\n"                           \
        "   move.l  %%a4,%%sp\n"                    \
    : "=r"(ret) \
    : "a"(args) \
    : "%%d0", "%%d1", "%%d2", "%%d3", "%%a0", "%%a1", "%a2", "%%a3", "%%a4", "cc", "memory");
    return ret;
}

static long SupmainEx(int args, char** argv, long(*func)(int args, char** argv), void* stack) {
    __s_args_t a;
    a.args = args;
    a.argv = argv;
    a.func = func;
    a.stack = stack;
    return SupmainTrampoline(&a);
}

static long Supmain(int args, char** argv, long(*func)(int, char**)) {
    return SupmainEx(args, argv, func, (void*)0L);
}

static inline uint16_t SetIPL(uint16_t ipl) {
	register long __retvalue __asm__("d0");
	__asm__ volatile                \
	(                               \
        "move.w %%sr,%%d1\n\t"      \
        "and.w  #0x0700,%1\n\t"     \
        "and.w  #0xf8ff,%%d1\n\t"   \
        "or.w   %1,%%d1\n\t"        \
        "move.w %%sr,%0\n\t"        \
        "and.w  #0x0700,%0\n\t"     \
        "move.w %%d1,%%sr\n\t"      \
	: "=r"(__retvalue)              \
	: "r"(ipl)                      \
	: __CLOBBER_RETURN("d0") "d1", "cc", "memory");
    return __retvalue;
}

static inline uint16_t DisableInterrupts(void) { return SetIPL(0x0700); }
static inline void RestoreInterrupts(uint16_t sr) { SetIPL(sr); }
static inline void nop() { __asm__ __volatile__( "nop\n\t" : : : ); }

#endif /* __GNUC__*/

#endif /* _LIBSYSUTIL_H_ */
