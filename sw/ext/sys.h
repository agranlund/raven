#ifndef _SYS_H_
#define _SYS_H_

//---------------------------------------------------------------------
// types
//---------------------------------------------------------------------
#ifndef uint8
typedef unsigned char       uint8;
#endif
#ifndef uint16
typedef unsigned short      uint16;
#endif
#ifndef uint32
typedef unsigned int        uint32;
#endif
#ifndef int8
typedef char                int8;
#endif
#ifndef int16
typedef short               int16;
#endif
#ifndef int32
typedef int                 int32;
#endif
#ifndef bool
typedef int                 bool;
#endif
#ifndef true
#define true                1
#endif
#ifndef false
#define false               0
#endif
#ifndef null
#define null                0
#endif



static inline uint16 disableInterrupts() {
    register uint16 _sr;
    __asm__ __volatile__(
        " move.w    sr,%0\n\t"
        " or.w      #0x0700,d1\n\t"
    : "=d"(_sr) : : "cc" );
    return _sr;
}

static inline void restoreInterrupts(uint16 _sr) {
    __asm__ __volatile__(
        " move.w    sr,d0\n\t"
    : : "d"(_sr) : "cc" );
}

#endif // _SYS_H_
