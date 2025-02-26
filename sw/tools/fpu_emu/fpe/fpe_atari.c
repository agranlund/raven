/*
 * Copyright (c) 2022-2024 Anders Granlund
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fpe_atari.h"
#include <mint/cookie.h>
#include <mint/sysvars.h>
#include <stdio.h>
#include "fpu_emulate.h"

#ifndef C_VFPU
#define C_VFPU  0x56465055UL  // 'VFPU'
#endif

#ifndef C_XBRA
#define C_XBRA  0x58425241UL  // 'XBRA'
#endif


//#define DEBUG_FPE

// from fpu_subr.c
#if defined(FPE_SUPPORT_68000)
extern int (*bfffo)(uint32_t);
extern int bfffo_68020(uint32_t);
#endif

// from fpu_emulate.c
extern int (*fpu_emul_fsave)(struct fpemu *, struct instruction *);
extern int (*fpu_emul_frestore)(struct fpemu *, struct instruction *);
extern int fpu_emul_fsave_68060(struct fpemu *, struct instruction *);
extern int fpu_emul_frestore_68060(struct fpemu *, struct instruction *);

// from fpe_atar_asm.S
extern uint32_t fpe_GetPCR_060();
extern uint32_t fpe_DetectFpu_020_060();

extern void fpe_vec_68000();
extern void fpe_vec_68010();
extern void fpe_vec_68020();
extern void fpe_vec_68030();
extern void fpe_vec_68040();
extern void fpe_vec_68060();

#if defined(FPE_SUPPORT_TOS1)
extern void fpe_vec_68000_tos1();
extern void fpe_vec_68010_tos1();
extern void fpe_vec_68020_tos1();
extern void fpe_vec_68030_tos1();
extern uint32_t fpe_tos1bot;
extern uint32_t fpe_tos1top;
#endif

extern uint32_t fpe_vec[5];

int fpe_install(uint32_t cpu)
{
    // detect hardware fpu
    int shouldDetectFpu = 1;
    if (cpu == 60) {
        uint32_t pcr = fpe_GetPCR_060();
        if( ((pcr & 0xffff0000UL) == 0x04310000UL) ||       // EC/LC cpu
            ((pcr & 0x00000002UL) != 0x00000000UL)) {       // FPU disabled
            shouldDetectFpu = 0;
        }
    } else if ((cpu < 20) || (cpu > 60)) {
        shouldDetectFpu = 0;
    }

    if (shouldDetectFpu && fpe_DetectFpu_020_060()) {
        return 0;
    }

    // pick linef handler based on cpu and tos version
    uint32_t vec_new = 0;
#if defined(FPE_SUPPORT_TOS1)
    OSHEADER* oshdr = (OSHEADER*) *((volatile uint32_t*)0x4F2);
    if (oshdr && oshdr->os_version < 0x0200)
    {
        fpe_tos1bot = (uint32_t) oshdr->os_beg;
        fpe_tos1top = fpe_tos1bot + (192 * 1024);
        switch(cpu)
        {
            case 00: vec_new = (uint32_t) fpe_vec_68000_tos1; break;
            case 10: vec_new = (uint32_t) fpe_vec_68010_tos1; break;
            case 20: vec_new = (uint32_t) fpe_vec_68020_tos1; break;
            case 30: vec_new = (uint32_t) fpe_vec_68030_tos1; break;
        }
    }
    else
#endif
    {
        switch(cpu)
        {
            case 00: vec_new = (uint32_t) fpe_vec_68000; break;
            case 10: vec_new = (uint32_t) fpe_vec_68010; break;
            case 20: vec_new = (uint32_t) fpe_vec_68020; break;
            case 30: vec_new = (uint32_t) fpe_vec_68030; break;
            case 40: vec_new = (uint32_t) fpe_vec_68040; break;
            case 60: vec_new = (uint32_t) fpe_vec_68060; break;
        }
    }
    if (vec_new == 0) {
        return 0;
    }

#if defined(FPE_SUPPORT_68000)
    if (cpu >= 20) {
        bfffo = &bfffo_68020;
    }
#endif

    if (cpu == 60) {
        fpu_emul_fsave = fpu_emul_fsave_68060;
        fpu_emul_frestore = fpu_emul_frestore_68060;
    }

    fpe_vec[0] = vec_new;
    fpe_vec[1] = C_XBRA;
    fpe_vec[2] = C_VFPU;
    fpe_vec[3] = *((volatile uint32_t*)0x2C);
    *((volatile uint32_t*)0x2C) = (uint32_t)&fpe_vec[4];

    return 1;
}

void panic(const char* str, ...) {
    // todo
#ifdef DIAGNOSTIC    
    printf("FPE PANIC [%s]!\n\r", str);
#endif
}

int fpe_abort(struct frame *frame, int signo, int code)
{
    // todo
#ifdef DIAGNOSTIC    
    printf("FPE ABORT! pc = %08x\n\r", frame->f_pc);
#endif
    return signo;
}


