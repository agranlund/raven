/*
 * Copyright (c) 2022 Anders Granlund
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _FPE_ATARI_H_
#define    _FPE_ATARI_H_

extern int fpe_install();        // returns 0 on success, negative number on fail


#include "string.h"

//#define DEBUG_FPE
//#define DIAGNOSTIC
//#define DEBUG

#if defined(DEBUG_FPE) || defined(DIAGNOSTIC) || defined(DEBUG)
#include "stdio.h"
#endif

#define _BYTE_ORDER         _BIG_ENDIAN
#define _LITTLE_ENDIAN      1234    /* LSB first: i386, vax */
#define _BIG_ENDIAN         4321    /* MSB first: 68000, ibm, net */
#define _PDP_ENDIAN         3412    /* LSB first in word, MSW first in long */

#define LITTLE_ENDIAN       _LITTLE_ENDIAN
#define BIG_ENDIAN          _BIG_ENDIAN
#define PDP_ENDIAN          _PDP_ENDIAN
#define BYTE_ORDER          _BYTE_ORDER


#define __packed            __attribute__((__packed__))
#define __aligned(x)        __attribute__((__aligned__(x)))
#define __section(x)        __attribute__((__section__(x)))

typedef char                int8_t;
typedef short               int16_t;
typedef int                 int32_t;
typedef long long           int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;
typedef uint32_t            ksiginfo_t;        // dummy

typedef unsigned char       u_char;
typedef unsigned short      u_short;
typedef unsigned int        u_int;
typedef unsigned long       u_long;
typedef unsigned long long  u_longlong;

typedef unsigned int        uint_t;

#define SIGILL              4
#define SIGFPE              8
#define SIGSEGV             11

#define ILL_ILLOPC          1
#define SEGV_ACCERR         2


/* user and super spaces are the same */
inline int copyin(const void *src, void *dst, size_t len) {
    memcpy(dst, src, len);
    return len;
}
inline int copyout(const void *src, void *dst, size_t len) {
    memcpy(dst, src, len);
    return len;
}
inline int ufetch_short(const unsigned short *from, unsigned short *to) {
    *to = *from;
    return 0;
}


/* fpe_atari.c */
#include "machine/frame.h"
extern void panic(const char *fmt, ...);
extern int fpe_abort(struct frame *frame, int signo, int code);

#endif /* _FPE_ATARI_H_ */

