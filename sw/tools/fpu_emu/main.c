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

#include <stdio.h>
#include <mint/cookie.h>
#include <stdint.h>
#include <string.h>

#ifndef C_VFPU
#define C_VFPU  0x56465055UL  // 'VFPU'
#endif

#ifndef C_XVBR
#define C_XVBR  0x58425241UL  // 'XBRA'
#endif

extern uint32_t _PgmSize;
extern int fpe_install(uint32_t cpu);

extern void fpe_FlushCache_020_030();
extern void fpe_FlushCache_040_060();
extern int  fpe_DetectFpu_020_060();
extern uint16_t fpe_SetIPL(uint16_t ipl);
extern uint32_t fpe_SetVBR_010_060(uint32_t vbr);
extern uint32_t fpe_GetVBR_010_060();
extern uint32_t fpe_SetPCR_060(uint32_t pcr);
extern uint32_t fpe_GetPCR_060();


struct VBRProxy     // 32 bytes
{
    uint32_t* vbr;
    uint16_t* proxy;
    uint32_t  reserved[3];
    uint32_t  magic;  // "XVBR"
    uint32_t  ident;  // user id
    uint32_t* old;
};

uint32_t* createVbrProxy(uint32_t* oldvbr) {
    const uint32_t size_header = sizeof(struct VBRProxy);
    const uint32_t size_vbr = 256 * 4 * 1;
    const uint32_t size_proxy = 256 * 2 * 4;
    uint32_t size = size_header + size_vbr + size_proxy;    // header + vbr + proxy
    uint32_t base = (uint32_t) Malloc(size+256);
    if (base == 0)
        return 0;

    // build header
    base = (base + 255) & 0xffffff00;
    memset((void*)base, 0, size);
    struct VBRProxy* p = (struct VBRProxy*) base;
    p->vbr = (uint32_t*) (base + size_header);
    p->proxy = (uint16_t*) (base + size_header + size_vbr);
    p->magic = C_XVBR;
    p->ident = C_VFPU;
    p->old = oldvbr;

    // build vbr + proxy table
    uint32_t old = (uint32_t)p->old;
    if (old < 0x10000) {
        for(uint16_t i=0,j=0; i<256; i++)
        {
            p->vbr[i] = (uint32_t) &p->proxy[j];
            p->proxy[j++] = 0x2F38;         // move.l <addr>.w,-(sp)
            p->proxy[j++] = old;            // addr
            p->proxy[j++] = 0x4E75;         // rts
            p->proxy[j++] = 0x4E75;         // rts
            old += 4;
        }
    } else {
        for(uint16_t i=0,j=0; i<256; i++)
        {
            p->vbr[i] = (uint32_t) &p->proxy[j];
            p->proxy[j++] = 0x2F39;         // move.l <addr>.l,-(sp)
            p->proxy[j++] = (old >> 16);    // hi addr
            p->proxy[j++] = (old & 0xFFFF); // lo addr
            p->proxy[j++] = 0x4E75;         // rts
            old += 4;
        }
    }
    return p->vbr;
}


void Setcookie(uint32_t c, uint32_t d)
{
    int32_t cookies_size = 0;
    int32_t cookies_used = 0;
    int32_t cookies_avail = 0;
    uint32_t* jar = (uint32_t*) *((uint32_t*)0x5A0);
    uint32_t* ptr = jar;
    
    while (1){
        cookies_used++;
        if (ptr[0] == c){
            ptr[1] = d;
            return;
        }
        else if (ptr[0] == 0){
            cookies_size = ptr[1];
            break;
        }
        ptr+=2;
    }

    cookies_avail = cookies_size - cookies_used;
    if (cookies_avail <= 0) {
        int32_t oldsize = (2*4*(cookies_size + 0));
        int32_t newsize = (2*4*(cookies_size + 8));
        uint32_t* newjar = (uint32_t*)Malloc(newsize);
        memcpy(newjar, jar, oldsize);
        *((uint32_t*)0x5A0) = (uint32_t)newjar;
        jar = newjar;
        cookies_size = cookies_size + 8;
    }
    
    jar[(cookies_used<<1)-2] = c;
    jar[(cookies_used<<1)-1] = d;
    jar[(cookies_used<<1)+0] = 0;
    jar[(cookies_used<<1)+1] = cookies_size;
}


int supermain()
{
    uint16_t sr = fpe_SetIPL(0x2700);
    uint32_t cpu = 0; Getcookie(C__CPU, (long*)&cpu);
    uint32_t fpu = 0; Getcookie(C__FPU, (long*)&fpu);

    if (!fpe_install(cpu)) {
        return 1;
    }
    // install to vbr proxy, create one if needed
    int hide_linef = 1;
    if (hide_linef && (cpu >= 10)) {
        volatile uint32_t* vbr_default = 0;
        volatile uint32_t* vbr_proxy = (volatile uint32_t*) fpe_GetVBR_010_060();
        if (vbr_default == vbr_proxy) {
            vbr_proxy = createVbrProxy((uint32_t*)vbr_default);
            if (vbr_proxy) {
                fpe_SetVBR_010_060((uint32_t)vbr_proxy);
            }
        }
        // assign linef handler to proxy
        if (vbr_default != vbr_proxy) {
            vbr_proxy[11] = vbr_default[11];
        }
    }

    // set cookie
    switch (cpu) {
        case 60: fpu = 0x00100000; break;   // 68060
        case 40: fpu = 0x00080000; break;   // 68040
        default: fpu = 0x00040000; break;   // 68881
    }

    Setcookie(C__FPU, fpu); // Atari _CPU cookie
    Setcookie(C_VFPU, fpu); // extra VFPU cookie

    // flush caches and restore interrupts
    if (cpu >= 40) {
        fpe_FlushCache_040_060();
    } else if (cpu >= 20) {
        fpe_FlushCache_020_030();
    }

    fpe_SetIPL(sr);
    return 0;
}


int main()
{
    int ret = Supexec(supermain);
    if (ret == 0) {
        Ptermres(_PgmSize, 0);
        return 0;
    }
    return 0;
}
