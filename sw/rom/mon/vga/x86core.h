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
#include "../lib.h"
#else
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#endif

/*---------------------- Macros and type definitions ----------------------*/

#ifdef DEBUG_X86CORE
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

/* flag conditions   */
#define FB_CF 0x0001            /* CARRY flag  */
#define FB_PF 0x0004            /* PARITY flag */
#define FB_AF 0x0010            /* AUX  flag   */
#define FB_ZF 0x0040            /* ZERO flag   */
#define FB_SF 0x0080            /* SIGN flag   */
#define FB_TF 0x0100            /* TRAP flag   */
#define FB_IF 0x0200            /* INTERRUPT ENABLE flag */
#define FB_DF 0x0400            /* DIR flag    */
#define FB_OF 0x0800            /* OVERFLOW flag */

/* 80286 and above always have bit#1 set */
#define F_ALWAYS_ON  (0x0002)   /* flag bits always on */

/*
 * Define a mask for only those flag bits we will ever pass back 
 * (via PUSHF) 
 */
#define F_MSK (FB_CF|FB_PF|FB_AF|FB_ZF|FB_SF|FB_TF|FB_IF|FB_DF|FB_OF)

/* following bits masked in to a 16bit quantity */

#define F_CF 0x0001             /* CARRY flag  */
#define F_PF 0x0004             /* PARITY flag */
#define F_AF 0x0010             /* AUX  flag   */
#define F_ZF 0x0040             /* ZERO flag   */
#define F_SF 0x0080             /* SIGN flag   */
#define F_TF 0x0100             /* TRAP flag   */
#define F_IF 0x0200             /* INTERRUPT ENABLE flag */
#define F_DF 0x0400             /* DIR flag    */
#define F_OF 0x0800             /* OVERFLOW flag */

#define SET_FLAG(flag)        	(emu->x86.R_FLG |= (flag))
#define CLEAR_FLAG(flag)      	(emu->x86.R_FLG &= ~(flag))
#define ACCESS_FLAG(flag)     	(emu->x86.R_FLG & (flag))
#define CLEARALL_FLAG(m)    	(emu->x86.R_FLG = 0)

#define CONDITIONAL_SET_FLAG(COND,FLAG) \
  if (COND) SET_FLAG(FLAG); else CLEAR_FLAG(FLAG)

#define F_PF_CALC 0x010000      /* PARITY flag has been calced    */
#define F_ZF_CALC 0x020000      /* ZERO flag has been calced      */
#define F_SF_CALC 0x040000      /* SIGN flag has been calced      */

#define F_ALL_CALC      0xff0000        /* All have been calced   */

/*
 * Emulator machine state.
 * Segment usage control.
 */
#define SYSMODE_SEG_DS_SS       0x00000001
#define SYSMODE_SEGOVR_CS       0x00000002
#define SYSMODE_SEGOVR_DS       0x00000004
#define SYSMODE_SEGOVR_ES       0x00000008
#define SYSMODE_SEGOVR_FS       0x00000010
#define SYSMODE_SEGOVR_GS       0x00000020
#define SYSMODE_SEGOVR_SS       0x00000040
#define SYSMODE_PREFIX_REPE     0x00000080
#define SYSMODE_PREFIX_REPNE    0x00000100
#define SYSMODE_PREFIX_DATA     0x00000200
#define SYSMODE_PREFIX_ADDR     0x00000400
#define SYSMODE_INTR_PENDING    0x10000000
#define SYSMODE_EXTRN_INTR      0x20000000
#define SYSMODE_HALTED          0x40000000

#define SYSMODE_SEGMASK (SYSMODE_SEG_DS_SS      | \
						 SYSMODE_SEGOVR_CS      | \
						 SYSMODE_SEGOVR_DS      | \
						 SYSMODE_SEGOVR_ES      | \
						 SYSMODE_SEGOVR_FS      | \
						 SYSMODE_SEGOVR_GS      | \
						 SYSMODE_SEGOVR_SS)
#define SYSMODE_CLRMASK (SYSMODE_SEG_DS_SS      | \
						 SYSMODE_SEGOVR_CS      | \
						 SYSMODE_SEGOVR_DS      | \
						 SYSMODE_SEGOVR_ES      | \
						 SYSMODE_SEGOVR_FS      | \
						 SYSMODE_SEGOVR_GS      | \
						 SYSMODE_SEGOVR_SS      | \
						 SYSMODE_PREFIX_DATA    | \
						 SYSMODE_PREFIX_ADDR)

#define  INTR_SYNCH           0x1

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
	uint32_t		mode;
    volatile int	intr;   /* mask of pending interrupts */
	uint8_t			intno;
	uint8_t			__pad[3];
};

struct X86EMU
{
    char            *mem_base;
    size_t          mem_size;

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
};


#if !defined(EXCLUDE_X86CORE)

/* decode.c */

void X86EMU_exec(struct X86EMU *);
void X86EMU_exec_call(struct X86EMU *, uint16_t, uint16_t);
void X86EMU_exec_intr(struct X86EMU *, uint8_t);
void X86EMU_halt_sys(struct X86EMU *);


#endif /* !defined(EXCLUDE_X86CORE) */

#endif /* _X86_CORE_H_ */
