/*	$NetBSD: emu->h,v 1.1 2007/12/01 20:14:10 joerg Exp $	*/

/****************************************************************************
*
*  Realmode X86 Emulator Library
*
*  Copyright (C) 1996-1999 SciTech Software, Inc.
*  Copyright (C) David Mosberger-Tang
*  Copyright (C) 1999 Egbert Eich
*  Copyright (C) 2007 Joerg Sonnenberger
*
*  ========================================================================
*
*  Permission to use, copy, modify, distribute, and sell this software and
*  its documentation for any purpose is hereby granted without fee,
*  provided that the above copyright notice appear in all copies and that
*  both that copyright notice and this permission notice appear in
*  supporting documentation, and that the name of the authors not be used
*  in advertising or publicity pertaining to distribution of the software
*  without specific, written prior permission.  The authors makes no
*  representations about the suitability of this software for any purpose.
*  It is provided "as is" without express or implied warranty.
*
*  THE AUTHORS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
*  EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
*  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
*  USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
*  OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
*  PERFORMANCE OF THIS SOFTWARE.
*
****************************************************************************/

#ifndef _X86_CORE_H_
#define _X86_CORE_H_

#ifdef RAVEN_ROM
#include "lib.h"
#else
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#endif

#ifndef DEBUG_X86CORE
#define DEBUG_X86CORE 0
#endif

/*---------------------- Macros and type definitions ----------------------*/

#if DEBUG_X86CORE
#ifndef RAVEN_ROM
#include <stdio.h>
#endif
#define x86dbg(...) printf(__VA_ARGS__)
#define x86err(...) printf(__VA_ARGS__)
#else
#define x86dbg(...)
#define x86err(...)
#endif


/* 8 bit registers */
#define R_AH  register_a.I8_reg.h_reg
#define R_AL  register_a.I8_reg.l_reg
#define R_BH  register_b.I8_reg.h_reg
#define R_BL  register_b.I8_reg.l_reg
#define R_CH  register_c.I8_reg.h_reg
#define R_CL  register_c.I8_reg.l_reg
#define R_DH  register_d.I8_reg.h_reg
#define R_DL  register_d.I8_reg.l_reg

/* 16 bit registers */
#define R_AX  register_a.I16_reg.x_reg
#define R_BX  register_b.I16_reg.x_reg
#define R_CX  register_c.I16_reg.x_reg
#define R_DX  register_d.I16_reg.x_reg

/* 32 bit extended registers */
#define R_EAX  register_a.I32_reg.e_reg
#define R_EBX  register_b.I32_reg.e_reg
#define R_ECX  register_c.I32_reg.e_reg
#define R_EDX  register_d.I32_reg.e_reg

/* special registers */
#define R_SP  register_sp.I16_reg.x_reg
#define R_BP  register_bp.I16_reg.x_reg
#define R_SI  register_si.I16_reg.x_reg
#define R_DI  register_di.I16_reg.x_reg
#define R_IP  register_ip.I16_reg.x_reg
#define R_FLG register_flags

/* special registers */
#define R_ESP  register_sp.I32_reg.e_reg
#define R_EBP  register_bp.I32_reg.e_reg
#define R_ESI  register_si.I32_reg.e_reg
#define R_EDI  register_di.I32_reg.e_reg
#define R_EIP  register_ip.I32_reg.e_reg
#define R_EFLG register_flags

/* segment registers */
#define R_CS  register_cs
#define R_DS  register_ds
#define R_SS  register_ss
#define R_ES  register_es
#define R_FS  register_fs
#define R_GS  register_gs

/*
 * General EAX, EBX, ECX, EDX type registers.  Note that for
 * portability, and speed, the issue of byte swapping is not addressed
 * in the registers.  All registers are stored in the default format
 * available on the host machine.  The only critical issue is that the
 * registers should line up EXACTLY in the same manner as they do in
 * the 386.  That is:
 *
 * EAX & 0xff  === AL
 * EAX & 0xffff == AX
 *
 * etc.  The result is that alot of the calculations can then be
 * done using the native instruction set fully.
 */


struct X86EMU_register32 {
	uint32_t e_reg;
};

struct X86EMU_register16 {
	uint16_t filler0;
	uint16_t x_reg;
};

struct X86EMU_register8 {
	uint8_t filler0, filler1;
	uint8_t h_reg, l_reg;
};


union X86EMU_register {
	struct X86EMU_register32	I32_reg;
	struct X86EMU_register16	I16_reg;
	struct X86EMU_register8		I8_reg;
};

struct X86EMU_regs {
	uint16_t		register_cs;
	uint16_t		register_ds;
	uint16_t		register_es;
	uint16_t		register_fs;
	uint16_t		register_gs;
	uint16_t		register_ss;
	uint32_t		register_flags;
	union X86EMU_register	register_a;
	union X86EMU_register	register_b;
	union X86EMU_register	register_c;
	union X86EMU_register	register_d;

	union X86EMU_register	register_sp;
	union X86EMU_register	register_bp;
	union X86EMU_register	register_si;
	union X86EMU_register	register_di;
	union X86EMU_register	register_ip;

	/*
	 * MODE contains information on:
	 *  REPE prefix             2 bits  repe,repne
	 *  SEGMENT overrides       5 bits  normal,DS,SS,CS,ES
	 *  Delayed flag set        3 bits  (zero, signed, parity)
	 *  reserved                6 bits
	 *  interrupt #             8 bits  instruction raised interrupt
	 *  BIOS video segregs      4 bits  
	 *  Interrupt Pending       1 bits  
	 *  Extern interrupt        1 bits
	 *  Halted                  1 bits
	 */
	uint32_t		    mode;
    volatile int32_t	intr;   /* mask of pending interrupts */
	uint8_t			    intno;
	uint8_t			    __pad[3];
};

typedef struct X86EMU
{
    struct X86EMU*  _this;

    char            *mem_base;
    uint32_t        mem_size;

    uint32_t        isa_iobase;
    uint32_t        isa_membase;

    void            *sys_private;
    struct          X86EMU_regs	x86;

    jmp_buf         exec_state;

    uint64_t        cur_cycles;

    unsigned int	cur_mod:2;
    unsigned int	cur_rl:3;
    unsigned int	cur_rh:3;
    uint32_t        cur_offset;

    uint8_t         (*emu_rdb)(struct X86EMU *, uint32_t addr);
    uint16_t        (*emu_rdw)(struct X86EMU *, uint32_t addr);
    uint32_t        (*emu_rdl)(struct X86EMU *, uint32_t addr);
    void            (*emu_wrb)(struct X86EMU *, uint32_t addr,uint8_t val);
    void            (*emu_wrw)(struct X86EMU *, uint32_t addr, uint16_t val);
    void            (*emu_wrl)(struct X86EMU *, uint32_t addr, uint32_t val);

    uint8_t         (*emu_inb)(struct X86EMU *, uint16_t addr);
    uint16_t        (*emu_inw)(struct X86EMU *, uint16_t addr);
    uint32_t        (*emu_inl)(struct X86EMU *, uint16_t addr);
    void            (*emu_outb)(struct X86EMU *, uint16_t addr, uint8_t val);
    void            (*emu_outw)(struct X86EMU *, uint16_t addr, uint16_t val);
    void            (*emu_outl)(struct X86EMU *, uint16_t addr, uint32_t val);

    void            (*_X86EMU_intrTab[256])(struct X86EMU *, int);

    void            (*_X86EMU_run)(struct X86EMU *);
    void            (*_X86EMU_call)(struct X86EMU *, uint16_t seg, uint16_t offs);
    void            (*_X86EMU_int)(struct X86EMU *, uint8_t num);

} _X86EMU;


#endif /* _X86_CORE_H_ */
