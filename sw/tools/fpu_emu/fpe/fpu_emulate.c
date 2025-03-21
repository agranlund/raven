/*	$NetBSD: fpu_emulate.c,v 1.40 2019/12/27 07:41:23 msaitoh Exp $	*/

/*
 * Copyright (c) 1995 Gordon W. Ross
 * some portion Copyright (c) 1995 Ken Nakata
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 4. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Gordon Ross
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

/*
 * mc68881 emulator
 * XXX - Just a start at it for now...
 */

/*
 * This file has been modified for Atari FPU emulator
 */
#include "fpe_atari.h"
/*
#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: fpu_emulate.c,v 1.40 2019/12/27 07:41:23 msaitoh Exp $");
#include <sys/param.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/systm.h>
*/
#include <machine/frame.h>

#if defined(DDB) && defined(DEBUG_FPE)
# include <m68k/db_machdep.h>
#endif

#include "fpu_emulate.h"
#if 0
#define	fpe_abort(tfp, ksi, signo, code)			\
	do {							\
		(ksi)->ksi_signo = (signo);			\
		(ksi)->ksi_code = (code);			\
		(ksi)->ksi_addr = (void *)(frame)->f_pc;	\
		return -1;					\
	} while (/* CONSTCOND */ 0)
#endif

int fpu_emul_fsave_68881(struct fpemu *, struct instruction *);
int fpu_emul_frestore_68881(struct fpemu *, struct instruction *);
int fpu_emul_fsave_68060(struct fpemu *, struct instruction *);
int fpu_emul_frestore_68060(struct fpemu *, struct instruction *);

int (*fpu_emul_fsave)(struct fpemu *, struct instruction *) = fpu_emul_fsave_68881;
int (*fpu_emul_frestore)(struct fpemu *, struct instruction *) = fpu_emul_frestore_68881;

static int fpu_emul_fmovmcr(struct fpemu *, struct instruction *);
static int fpu_emul_fmovm(struct fpemu *, struct instruction *);
static int fpu_emul_arith(struct fpemu *, struct instruction *);
static int fpu_emul_type1(struct fpemu *, struct instruction *);
static int fpu_emul_brcc(struct fpemu *, struct instruction *);
static int test_cc(struct fpemu *, int);

#ifdef DEBUG_FPE
#define DUMP_INSN(insn)							\
	printf("%s: insn={adv=%d,siz=%d,op=%04x,w1=%04x}\n",		\
	    __func__,							\
	    (insn)->is_advance, (insn)->is_datasize,			\
	    (insn)->is_opcode, (insn)->is_word1)
#define DPRINTF(x)	printf x
#else
#define DUMP_INSN(insn)	do {} while (/* CONSTCOND */ 0)
#define DPRINTF(x)	do {} while (/* CONSTCOND */ 0)
#endif


#define FPU_STATE_NULL      0
#define FPU_STATE_IDLE      1
#define FPU_STATE_BUSY      2
#define FPU_STATE_EXCP      3


int16_t fpu_state = FPU_STATE_NULL;
int16_t fpu_type  = 00;

/*
 * Emulate a floating-point instruction.
 * Return zero for success, else signal number.
 * (Typically: zero, SIGFPE, SIGILL, SIGSEGV)
 */
int
fpu_emulate(struct frame *frame /*, struct fpframe *fpf, ksiginfo_t *ksi*/)
{
	static struct fpframe fpf;
	static struct instruction insn;
	static struct fpemu fe;
	int optype, sig;
	unsigned short sval;

	DPRINTF(("sr: %08x\n", frame->f_sr));
	DPRINTF(("pc: %08x\n", frame->f_pc));
	DPRINTF(("vector: %08x\n", frame->f_vector));
	DPRINTF(("format: %08x\n", frame->f_format));
	DPRINTF(("adj: %08x\n", frame->f_stackadj));
	DPRINTF(("d0:%08x d1:%08x d2:%08x d3:%08x\n", frame->f_regs[0], frame->f_regs[1], frame->f_regs[2], frame->f_regs[3]));
	DPRINTF(("d4:%08x d5:%08x d6:%08x d7:%08x\n", frame->f_regs[4], frame->f_regs[5], frame->f_regs[6], frame->f_regs[7]));
	DPRINTF(("a0:%08x a1:%08x a2:%08x a3:%08x\n", frame->f_regs[8], frame->f_regs[9], frame->f_regs[10], frame->f_regs[11]));
	DPRINTF(("a4:%08x a5:%08x a6:%08x a7:%08x\n", frame->f_regs[12], frame->f_regs[13], frame->f_regs[14], frame->f_regs[15]));

	/* initialize insn.is_datasize to tell it is *not* initialized */
	insn.is_datasize = -1;

	fe.fe_frame = frame;
	fe.fe_fpframe = &fpf;
	fe.fe_fpsr = fe.fe_fpframe->fpf_fpsr;
	fe.fe_fpcr = fe.fe_fpframe->fpf_fpcr;

	DPRINTF(("%s: ENTERING: FPSR=%08x, FPCR=%08x\n",
	    __func__, fe.fe_fpsr, fe.fe_fpcr));

	/* always set this (to avoid a warning) */
	insn.is_pc = frame->f_pc;
	insn.is_nextpc = 0;
	if (frame->f_format == 4) {
		/*
		 * A format 4 is generated by the 68{EC,LC}040.  The PC is
		 * already set to the instruction following the faulting
		 * instruction.  We need to calculate that, anyway.  The
		 * fslw is the PC of the faulted instruction, which is what
		 * we expect to be in f_pc.
		 *
		 * XXX - This is a hack; it assumes we at least know the
		 * sizes of all instructions we run across.
		 * XXX TODO: This may not be true, so we might want to save
		 * the PC in order to restore it later.
		 */
#if 0
		insn.is_nextpc = frame->f_pc;
#endif
		insn.is_pc = frame->f_fmt4.f_fslw;
		frame->f_pc = insn.is_pc;
	}

	if (ufetch_short((void *)(insn.is_pc), &sval)) {
		DPRINTF(("%s: fault reading opcode\n", __func__));
		fpe_abort(frame, SIGSEGV, SEGV_ACCERR);
	}

	if ((sval & 0xf000) != 0xf000) {
		DPRINTF(("%s: not coproc. insn.: opcode=0x%x\n",
		    __func__, sval));
		fpe_abort(frame, SIGILL, ILL_ILLOPC);
	}

	if ((sval & 0x0E00) != 0x0200) {
		DPRINTF(("%s: bad coproc. id: opcode=0x%x\n", __func__, sval));
		fpe_abort(frame, SIGILL, ILL_ILLOPC);
	}

	insn.is_opcode = sval;
	optype = (sval & 0x01C0);

	if (ufetch_short((void *)(insn.is_pc + 2), &sval)) {
		DPRINTF(("%s: fault reading word1\n", __func__));
		fpe_abort(frame, SIGSEGV, SEGV_ACCERR);
	}
	insn.is_word1 = sval;
	/* all FPU instructions are at least 4-byte long */
	insn.is_advance = 4;

	DUMP_INSN(&insn);

	/*
	 * Which family (or type) of opcode is it?
	 * Tests ordered by likelihood (hopefully).
	 * Certainly, type 0 is the most common.
	 */
	if (optype == 0x0000) {
		/* type=0: generic */
		fpu_state = FPU_STATE_IDLE;
		if ((sval & 0xc000) == 0xc000) {
			DPRINTF(("%s: fmovm FPr\n", __func__));
			sig = fpu_emul_fmovm(&fe, &insn);
		} else if ((sval & 0xc000) == 0x8000) {
			DPRINTF(("%s: fmovm FPcr\n", __func__));
			sig = fpu_emul_fmovmcr(&fe, &insn);
		} else if ((sval & 0xe000) == 0x6000) {
			/* fstore = fmove FPn,mem */
			DPRINTF(("%s: fmove to mem\n", __func__));
			sig = fpu_emul_fstore(&fe, &insn);
		} else if ((sval & 0xfc00) == 0x5c00) {
			/* fmovecr */
			DPRINTF(("%s: fmovecr\n", __func__));
			sig = fpu_emul_fmovecr(&fe, &insn);
		} else if ((sval & 0xa07f) == 0x26) {
			/* fscale */
			DPRINTF(("%s: fscale\n", __func__));
			sig = fpu_emul_fscale(&fe, &insn);
		} else {
			DPRINTF(("%s: other type0\n", __func__));
			/* all other type0 insns are arithmetic */
			sig = fpu_emul_arith(&fe, &insn);
		}
		if (sig == 0) {
			DPRINTF(("%s: type 0 returned 0\n", __func__));
			sig = fpu_upd_excp(&fe);
		}
	} else if (optype == 0x0080 || optype == 0x00C0) {
		/* type=2 or 3: fbcc, short or long disp. */
		fpu_state = FPU_STATE_IDLE;
		DPRINTF(("%s: fbcc %s\n", __func__,
		    (optype & 0x40) ? "long" : "short"));
		sig = fpu_emul_brcc(&fe, &insn);
	} else if (optype == 0x0040) {
		/* type=1: fdbcc, fscc, ftrapcc */
		fpu_state = FPU_STATE_IDLE;
		DPRINTF(("%s: type1\n", __func__));
		sig = fpu_emul_type1(&fe, &insn);
	} else if (optype == 0x0100) {
		/* type=4: fsave */
		DPRINTF(("%s: fsave\n", __func__));
		sig = fpu_emul_fsave(&fe, &insn);
	} else if (optype == 0x0140) {
		/* type=5: frestore */
		DPRINTF(("%s: frestore\n", __func__));
		sig = fpu_emul_frestore(&fe, &insn);
	}
	else {
		/* FTRAPcc ?? */
		/* type=6: reserved */
		/* type=7: reserved */
		DPRINTF(("%s: bad opcode type: opcode=0x%x\n", __func__,
			insn.is_opcode));
		sig = SIGILL;
	}

	DUMP_INSN(&insn);

	/*
	 * XXX it is not clear to me, if we should progress the PC always,
	 * for SIGFPE || 0, or only for 0; however, without SIGFPE, we
	 * don't pass the signalling regression  tests.	-is
	 */
	if ((sig == 0) || (sig == SIGFPE))
		frame->f_pc += insn.is_advance;
#if defined(DDB) && defined(DEBUG_FPE)
	else {
		printf("%s: sig=%d, opcode=%x, word1=%x\n", __func__,
		    sig, insn.is_opcode, insn.is_word1);
		kdb_trap(-1, (db_regs_t *)&frame);
	}
#endif
#if 0 /* XXX something is wrong */
	if (frame->f_format == 4) {
		/* XXX Restore PC -- 68{EC,LC}040 only */
		if (insn.is_nextpc)
			frame->f_pc = insn.is_nextpc;
	}
#endif

	DPRINTF(("%s: EXITING: w/FPSR=%08x, FPCR=%08x\n", __func__,
	    fe.fe_fpsr, fe.fe_fpcr));

	if (sig)
		fpe_abort(frame, sig, 0);
	return sig;
}

/* update accrued exception bits and see if there's an FP exception */
int
fpu_upd_excp(struct fpemu *fe)
{
	uint32_t fpsr;
	uint32_t fpcr;

	fpsr = fe->fe_fpsr;
	fpcr = fe->fe_fpcr;
	/*
	 * update fpsr accrued exception bits; each insn doesn't have to
	 * update this
	 */
	if (fpsr & (FPSR_BSUN | FPSR_SNAN | FPSR_OPERR)) {
		fpsr |= FPSR_AIOP;
	}
	if (fpsr & FPSR_OVFL) {
		fpsr |= FPSR_AOVFL;
	}
	if ((fpsr & FPSR_UNFL) && (fpsr & FPSR_INEX2)) {
		fpsr |= FPSR_AUNFL;
	}
	if (fpsr & FPSR_DZ) {
		fpsr |= FPSR_ADZ;
	}
	if (fpsr & (FPSR_INEX1 | FPSR_INEX2 | FPSR_OVFL)) {
		fpsr |= FPSR_AINEX;
	}

	fe->fe_fpframe->fpf_fpsr = fe->fe_fpsr = fpsr;

	return (fpsr & fpcr & FPSR_EXCP) ? SIGFPE : 0;
}

/* update fpsr according to fp (= result of an fp op) */
uint32_t
fpu_upd_fpsr(struct fpemu *fe, struct fpn *fp)
{
	uint32_t fpsr;

	DPRINTF(("%s: previous fpsr=%08x\n", __func__, fe->fe_fpsr));
	/* clear all condition code */
	fpsr = fe->fe_fpsr & ~FPSR_CCB;

	DPRINTF(("%s: result is a ", __func__));
	if (fp->fp_sign) {
		DPRINTF(("negative "));
		fpsr |= FPSR_NEG;
	} else {
		DPRINTF(("positive "));
	}

	switch (fp->fp_class) {
	case FPC_SNAN:
		DPRINTF(("signaling NAN\n"));
		fpsr |= (FPSR_NAN | FPSR_SNAN);
		break;
	case FPC_QNAN:
		DPRINTF(("quiet NAN\n"));
		fpsr |= FPSR_NAN;
		break;
	case FPC_ZERO:
		DPRINTF(("Zero\n"));
		fpsr |= FPSR_ZERO;
		break;
	case FPC_INF:
		DPRINTF(("Inf\n"));
		fpsr |= FPSR_INF;
		break;
	default:
		DPRINTF(("Number\n"));
		/* anything else is treated as if it is a number */
		break;
	}

	fe->fe_fpsr = fe->fe_fpframe->fpf_fpsr = fpsr;

	DPRINTF(("%s: new fpsr=%08x\n", __func__, fe->fe_fpframe->fpf_fpsr));

	return fpsr;
}

static int
fpu_emul_fmovmcr(struct fpemu *fe, struct instruction *insn)
{
	struct frame *frame = fe->fe_frame;
	struct fpframe *fpf = fe->fe_fpframe;
	int sig;
	int reglist;
	int fpu_to_mem;

	/* move to/from control registers */
	reglist = (insn->is_word1 & 0x1c00) >> 10;
	/* Bit 13 selects direction (FPU to/from Mem) */
	fpu_to_mem = insn->is_word1 & 0x2000;

	insn->is_datasize = 4;
	insn->is_advance = 4;
	sig = fpu_decode_ea(frame, insn, &insn->is_ea, insn->is_opcode);
	if (sig)
		return sig;

	if (reglist != 1 && reglist != 2 && reglist != 4 &&
	    (insn->is_ea.ea_flags & EA_DIRECT)) {
		/* attempted to copy more than one FPcr to CPU regs */
		DPRINTF(("%s: tried to copy too many FPcr\n", __func__));
		return SIGILL;
	}

	if (reglist & 4) {
		/* fpcr */
		if ((insn->is_ea.ea_flags & EA_DIRECT) &&
		    insn->is_ea.ea_regnum >= 8 /* address reg */) {
			/* attempted to copy FPCR to An */
			DPRINTF(("%s: tried to copy FPCR from/to A%d\n",
			    __func__, insn->is_ea.ea_regnum & 7));
			return SIGILL;
		}
		if (fpu_to_mem) {
			sig = fpu_store_ea(frame, insn, &insn->is_ea,
			    (char *)&fpf->fpf_fpcr);
		} else {
			sig = fpu_load_ea(frame, insn, &insn->is_ea,
			    (char *)&fpf->fpf_fpcr);
		}
	}
	if (sig)
		return sig;

	if (reglist & 2) {
		/* fpsr */
		if ((insn->is_ea.ea_flags & EA_DIRECT) &&
		    insn->is_ea.ea_regnum >= 8 /* address reg */) {
			/* attempted to copy FPSR to An */
			DPRINTF(("%s: tried to copy FPSR from/to A%d\n",
			    __func__, insn->is_ea.ea_regnum & 7));
			return SIGILL;
		}
		if (fpu_to_mem) {
			sig = fpu_store_ea(frame, insn, &insn->is_ea,
			    (char *)&fpf->fpf_fpsr);
		} else {
			sig = fpu_load_ea(frame, insn, &insn->is_ea,
			    (char *)&fpf->fpf_fpsr);
		}
	}
	if (sig)
		return sig;

	if (reglist & 1) {
		/* fpiar - can be moved to/from An */
		if (fpu_to_mem) {
			sig = fpu_store_ea(frame, insn, &insn->is_ea,
			    (char *)&fpf->fpf_fpiar);
		} else {
			sig = fpu_load_ea(frame, insn, &insn->is_ea,
			    (char *)&fpf->fpf_fpiar);
		}
	}
	return sig;
}


int fpu_emul_fsave_68881(struct fpemu *fe, struct instruction *insn)
{
	static uint8_t buf[28] = {
		0,						//  0: version number
		24,						//  1: frame size
		0xFF, 0xFF,				//  2: reserved
		0,0,					//  4: ccr
		0,0,					//  6: reserved
		0,0,0,0,				//  8: exception operand lo
		0,0,0,0,				// 12: exception operand mid
		0,0,0,0,				// 16: exception operand hi
		0,0,0,0,				// 20: operand register
		0x5C,0x0E,0xFF,0xFF,	// 24: biu flags	
	};

	int sig;
	struct frame *frame = fe->fe_frame;

	insn->is_advance = 2;
	insn->is_datasize = (fpu_state == 0) ? 4 : 28;
	sig = fpu_decode_ea(frame, insn, &insn->is_ea, insn->is_opcode);
	if (sig)
		return sig;

	buf[0] = (fpu_state == 0) ? 0 : 0x1F;		// version number
	sig = fpu_store_ea(frame, insn, &insn->is_ea, (char*)buf);
	return sig;
}

int fpu_emul_frestore_68881(struct fpemu *fe, struct instruction *insn)
{
	int sig;
	struct frame *frame = fe->fe_frame;
	//static uint8_t buf[136];    // busy state = 136
	static uint8_t buf[28];     // idle state = 28

	insn->is_advance = 2;
	insn->is_datasize = 4;
	sig = fpu_decode_ea(frame, insn, &insn->is_ea, insn->is_opcode);
	if (sig)
		return sig;

	sig = fpu_load_ea(frame, insn, &insn->is_ea, (char*)buf);
	if (sig)
		return sig;

    // null state
    if (buf[0] == 0) {
        fpu_state = FPU_STATE_NULL;
        return sig;
    }

    // idle state (0x1F)
	fpu_state = FPU_STATE_IDLE;
    insn->is_datasize = buf[1];
    sig = fpu_load_ea(frame, insn, &insn->is_ea, (char*)&buf[4]);
    if (sig) {
        return sig;
    }
	return sig;
}


int fpu_emul_fsave_68060(struct fpemu *fe, struct instruction *insn)
{
	static uint8_t buf[12] = {
        0, 0,       // excp operand exponent
        0,          // status (0x00 = null, 0x60 = idle, 0xE0 = excp)
        0,          // 00000vvv : excp vector
        0, 0, 0, 0, // excp operand upper 32bit
        0, 0, 0, 0  // excp operand lower 32bit
	};
   
	insn->is_advance = 2;
	insn->is_datasize = 12;
	struct frame *frame = fe->fe_frame;
	int sig = fpu_decode_ea(frame, insn, &insn->is_ea, insn->is_opcode);
	if (sig) { return sig; }
	buf[2] = (fpu_state == FPU_STATE_NULL) ? 0x00 : 0x60;
	sig = fpu_store_ea(frame, insn, &insn->is_ea, (char*)buf);
	return sig;
}

int fpu_emul_frestore_68060(struct fpemu *fe, struct instruction *insn)
{
	static uint8_t buf[12];  // always 4 longs
	insn->is_advance = 2;
	insn->is_datasize = 12;
	struct frame *frame = fe->fe_frame;
	int sig = fpu_decode_ea(frame, insn, &insn->is_ea, insn->is_opcode);
    if (sig) { return sig; }
	sig = fpu_load_ea(frame, insn, &insn->is_ea, (char*)buf);
	if (sig) { return sig; }
    fpu_state = (buf[2] == 0x00) ? FPU_STATE_NULL : FPU_STATE_IDLE;
	return sig;
}



/*
 * type 0: fmovem
 * Separated out of fpu_emul_type0 for efficiency.
 * In this function, we know:
 *   (opcode & 0x01C0) == 0
 *   (word1 & 0x8000) == 0x8000
 *
 * No conversion or rounding is done by this instruction,
 * and the FPSR is not affected.
 */
static int
fpu_emul_fmovm(struct fpemu *fe, struct instruction *insn)
{
	struct frame *frame = fe->fe_frame;
	struct fpframe *fpf = fe->fe_fpframe;
	int word1, sig;
	int reglist, regmask, regnum;
	int fpu_to_mem, order;
	/* int w1_post_incr; */
	int *fpregs;

	insn->is_advance = 4;
	insn->is_datasize = 12;
	word1 = insn->is_word1;

	/* Bit 13 selects direction (FPU to/from Mem) */
	fpu_to_mem = word1 & 0x2000;

	/*
	 * Bits 12,11 select register list mode:
	 * 0,0: Static  reg list, pre-decr.
	 * 0,1: Dynamic reg list, pre-decr.
	 * 1,0: Static  reg list, post-incr.
	 * 1,1: Dynamic reg list, post-incr
	 */
	/* w1_post_incr = word1 & 0x1000; */
	if (word1 & 0x0800) {
		/* dynamic reg list */
		reglist = frame->f_regs[(word1 & 0x70) >> 4];
	} else {
		reglist = word1;
	}
	reglist &= 0xFF;

	/* Get effective address. (modreg=opcode&077) */
	sig = fpu_decode_ea(frame, insn, &insn->is_ea, insn->is_opcode);
	if (sig)
		return sig;

	/* Get address of soft coprocessor regs. */
	fpregs = (int*)&fpf->fpf_regs[0];

	if (insn->is_ea.ea_flags & EA_PREDECR) {
		regnum = 7;
		order = -1;
	} else {
		regnum = 0;
		order = 1;
	}

	regmask = 0x80;
	while ((0 <= regnum) && (regnum < 8)) {
		if (regmask & reglist) {
			if (fpu_to_mem) {
				sig = fpu_store_ea(frame, insn, &insn->is_ea,
				    (char *)&fpregs[regnum * 3]);
				DPRINTF(("%s: FP%d (%08x,%08x,%08x) saved\n",
				    __func__, regnum,
				    fpregs[regnum * 3],
				    fpregs[regnum * 3 + 1],
				    fpregs[regnum * 3 + 2]));
			} else {		/* mem to fpu */
				sig = fpu_load_ea(frame, insn, &insn->is_ea,
				    (char *)&fpregs[regnum * 3]);
				DPRINTF(("%s: FP%d (%08x,%08x,%08x) loaded\n",
				    __func__, regnum,
				    fpregs[regnum * 3],
				    fpregs[regnum * 3 + 1],
				    fpregs[regnum * 3 + 2]));
			}
			if (sig)
				break;
		}
		regnum += order;
		regmask >>= 1;
	}

	return sig;
}

struct fpn *
fpu_cmp(struct fpemu *fe)
{
	struct fpn *x = &fe->fe_f1, *y = &fe->fe_f2;

	/* take care of special cases */
	if (x->fp_class < 0 || y->fp_class < 0) {
		/* if either of two is a SNAN, result is SNAN */
		x->fp_class =
		    (y->fp_class < x->fp_class) ? y->fp_class : x->fp_class;
	} else if (x->fp_class == FPC_INF) {
		if (y->fp_class == FPC_INF) {
			/* both infinities */
			if (x->fp_sign == y->fp_sign) {
				/* return a signed zero */
				x->fp_class = FPC_ZERO;
			} else {
				/* return a faked number w/x's sign */
				x->fp_class = FPC_NUM;
				x->fp_exp = 16383;
				x->fp_mant[0] = FP_1;
			}
		} else {
			/* y is a number */
			/* return a forged number w/x's sign */
			x->fp_class = FPC_NUM;
			x->fp_exp = 16383;
			x->fp_mant[0] = FP_1;
		}
	} else if (y->fp_class == FPC_INF) {
		/* x is a Num but y is an Inf */
		/* return a forged number w/y's sign inverted */
		x->fp_class = FPC_NUM;
		x->fp_sign = !y->fp_sign;
		x->fp_exp = 16383;
		x->fp_mant[0] = FP_1;
	} else {
		/*
		 * x and y are both numbers or zeros,
		 * or pair of a number and a zero
		 */
		y->fp_sign = !y->fp_sign;
		x = fpu_add(fe);	/* (x - y) */
		/*
		 * FCMP does not set Inf bit in CC, so return a forged number
		 * (value doesn't matter) if Inf is the result of fsub.
		 */
		if (x->fp_class == FPC_INF) {
			x->fp_class = FPC_NUM;
			x->fp_exp = 16383;
			x->fp_mant[0] = FP_1;
		}
	}
	return x;
}

/*
 * arithmetic operations
 */
static int
fpu_emul_arith(struct fpemu *fe, struct instruction *insn)
{
	struct frame *frame = fe->fe_frame;
	uint32_t *fpregs = &(fe->fe_fpframe->fpf_regs[0]);
	struct fpn *res;
	int word1, sig = 0;
	int regnum, format;
	int discard_result = 0;
	uint32_t buf[3];
#ifdef DEBUG_FPE
	int flags;
	char regname;
#endif

	fe->fe_fpsr &= ~FPSR_EXCP;

	DUMP_INSN(insn);

	DPRINTF(("%s: FPSR = %08x, FPCR = %08x\n", __func__,
	    fe->fe_fpsr, fe->fe_fpcr));

	word1 = insn->is_word1;
	format = (word1 >> 10) & 7;
	regnum = (word1 >> 7) & 7;

	/* fetch a source operand : may not be used */
	DPRINTF(("%s: dst/src FP%d=%08x,%08x,%08x\n", __func__,
	    regnum, fpregs[regnum * 3], fpregs[regnum * 3 + 1],
	    fpregs[regnum * 3 + 2]));

	fpu_explode(fe, &fe->fe_f1, FTYPE_EXT, &fpregs[regnum * 3]);

	DUMP_INSN(insn);

	/* get the other operand which is always the source */
	if ((word1 & 0x4000) == 0) {
		DPRINTF(("%s: FP%d op FP%d => FP%d\n", __func__,
		    format, regnum, regnum));
		DPRINTF(("%s: src opr FP%d=%08x,%08x,%08x\n", __func__,
		    format, fpregs[format * 3], fpregs[format * 3 + 1],
		    fpregs[format * 3 + 2]));
		fpu_explode(fe, &fe->fe_f2, FTYPE_EXT, &fpregs[format * 3]);
	} else {
		/* the operand is in memory */
		switch (format)
		{
			case FTYPE_DBL:
				insn->is_datasize = 8;
				break;
			case FTYPE_SNG:
			case FTYPE_LNG:
				insn->is_datasize = 4;
				break;
			case FTYPE_WRD:
				insn->is_datasize = 2;
				break;
			case FTYPE_BYT:
				insn->is_datasize = 1;
				break;
			case FTYPE_EXT:
				insn->is_datasize = 12;
				break;
			default:
				/* invalid or unsupported operand format */
				sig = SIGFPE;
				return sig;
		}

		/* Get effective address. (modreg=opcode&077) */
		sig = fpu_decode_ea(frame, insn, &insn->is_ea, insn->is_opcode);
		if (sig) {
			DPRINTF(("%s: error in fpu_decode_ea\n", __func__));
			return sig;
		}

		DUMP_INSN(insn);

#ifdef DEBUG_FPE
		printf("%s: addr mode = ", __func__);
		flags = insn->is_ea.ea_flags;
		regname = (insn->is_ea.ea_regnum & 8) ? 'a' : 'd';

		if (flags & EA_DIRECT) {
			printf("%c%d\n", regname, insn->is_ea.ea_regnum & 7);
		} else if (flags & EA_PC_REL) {
			if (flags & EA_OFFSET) {
				printf("pc@(%d)\n", insn->is_ea.ea_offset);
			} else if (flags & EA_INDEXED) {
				printf("pc@(...)\n");
			}
		} else if (flags & EA_PREDECR) {
			printf("%c%d@-\n", regname, insn->is_ea.ea_regnum & 7);
		} else if (flags & EA_POSTINCR) {
			printf("%c%d@+\n", regname, insn->is_ea.ea_regnum & 7);
		} else if (flags & EA_OFFSET) {
			printf("%c%d@(%d)\n", regname,
			    insn->is_ea.ea_regnum & 7,
			    insn->is_ea.ea_offset);
		} else if (flags & EA_INDEXED) {
			printf("%c%d@(...)\n", regname,
			    insn->is_ea.ea_regnum & 7);
		} else if (flags & EA_ABS) {
			printf("0x%08x\n", insn->is_ea.ea_absaddr);
		} else if (flags & EA_IMMED) {
			printf("#0x%08x,%08x,%08x\n",
			    insn->is_ea.ea_immed[0],
			    insn->is_ea.ea_immed[1],
			    insn->is_ea.ea_immed[2]);
		} else {
			printf("%c%d@\n", regname, insn->is_ea.ea_regnum & 7);
		}
#endif /* DEBUG_FPE */

		fpu_load_ea(frame, insn, &insn->is_ea, (char*)buf);
		if (format == FTYPE_WRD) {
			/* sign-extend */
			buf[0] &= 0xffff;
			if (buf[0] & 0x8000)
				buf[0] |= 0xffff0000;
			format = FTYPE_LNG;
		} else if (format == FTYPE_BYT) {
			/* sign-extend */
			buf[0] &= 0xff;
			if (buf[0] & 0x80)
				buf[0] |= 0xffffff00;
			format = FTYPE_LNG;
		}
		DPRINTF(("%s: src = %08x %08x %08x, siz = %d\n", __func__,
		    buf[0], buf[1], buf[2], insn->is_datasize));
		fpu_explode(fe, &fe->fe_f2, format, buf);
	}

	DUMP_INSN(insn);

	/*
	 * An arithmetic instruction emulate function has a prototype of
	 * struct fpn *fpu_op(struct fpemu *);
	 *
	 * 1) If the instruction is monadic, then fpu_op() must use
	 *    fe->fe_f2 as its operand, and return a pointer to the
	 *    result.
	 *
	 * 2) If the instruction is diadic, then fpu_op() must use
	 *    fe->fe_f1 and fe->fe_f2 as its two operands, and return a
	 *    pointer to the result.
	 *
	 */
	res = NULL;
	switch (word1 & 0x7f) {
	case 0x00:		/* fmove */
		res = &fe->fe_f2;
		break;

	case 0x01:		/* fint */
		res = fpu_int(fe);
		break;

	case 0x02:		/* fsinh */
		res = fpu_sinh(fe);
		break;

	case 0x03:		/* fintrz */
		res = fpu_intrz(fe);
		break;

	case 0x04:		/* fsqrt */
		res = fpu_sqrt(fe);
		break;

	case 0x06:		/* flognp1 */
		res = fpu_lognp1(fe);
		break;

	case 0x08:		/* fetoxm1 */
		res = fpu_etoxm1(fe);
		break;

	case 0x09:		/* ftanh */
		res = fpu_tanh(fe);
		break;

	case 0x0A:		/* fatan */
		res = fpu_atan(fe);
		break;

	case 0x0C:		/* fasin */
		res = fpu_asin(fe);
		break;

	case 0x0D:		/* fatanh */
		res = fpu_atanh(fe);
		break;

	case 0x0E:		/* fsin */
		res = fpu_sin(fe);
		break;

	case 0x0F:		/* ftan */
		res = fpu_tan(fe);
		break;

	case 0x10:		/* fetox */
		res = fpu_etox(fe);
		break;

	case 0x11:		/* ftwotox */
		res = fpu_twotox(fe);
		break;

	case 0x12:		/* ftentox */
		res = fpu_tentox(fe);
		break;

	case 0x14:		/* flogn */
		res = fpu_logn(fe);
		break;

	case 0x15:		/* flog10 */
		res = fpu_log10(fe);
		break;

	case 0x16:		/* flog2 */
		res = fpu_log2(fe);
		break;

	case 0x18:		/* fabs */
		fe->fe_f2.fp_sign = 0;
		res = &fe->fe_f2;
		break;

	case 0x19:		/* fcosh */
		res = fpu_cosh(fe);
		break;

	case 0x1A:		/* fneg */
		fe->fe_f2.fp_sign = !fe->fe_f2.fp_sign;
		res = &fe->fe_f2;
		break;

	case 0x1C:		/* facos */
		res = fpu_acos(fe);
		break;

	case 0x1D:		/* fcos */
		res = fpu_cos(fe);
		break;

	case 0x1E:		/* fgetexp */
		res = fpu_getexp(fe);
		break;

	case 0x1F:		/* fgetman */
		res = fpu_getman(fe);
		break;

	case 0x20:		/* fdiv */
	case 0x24:		/* fsgldiv: cheating - better than nothing */
		res = fpu_div(fe);
		break;

	case 0x21:		/* fmod */
		res = fpu_mod(fe);
		break;

	case 0x28:		/* fsub */
		fe->fe_f2.fp_sign = !fe->fe_f2.fp_sign; /* f2 = -f2 */
		/* FALLTHROUGH */
	case 0x22:		/* fadd */
		res = fpu_add(fe);
		break;

	case 0x23:		/* fmul */
	case 0x27:		/* fsglmul: cheating - better than nothing */
		res = fpu_mul(fe);
		break;

	case 0x25:		/* frem */
		res = fpu_rem(fe);
		break;

	case 0x26:
		/* fscale is handled by a separate function */
		break;

	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x34:
	case 0x35:
	case 0x36:
	case 0x37:		/* fsincos */
		res = fpu_sincos(fe, word1 & 7);
		break;

	case 0x38:		/* fcmp */
		res = fpu_cmp(fe);
		discard_result = 1;
		break;

	case 0x3A:		/* ftst */
		res = &fe->fe_f2;
		discard_result = 1;
		break;

	default:		/* possibly 040/060 instructions */
		DPRINTF(("%s: bad opcode=0x%x, word1=0x%x\n", __func__,
		    insn->is_opcode, insn->is_word1));
		sig = SIGILL;
	}

	/* for sanity */
	if (res == NULL)
		sig = SIGILL;

	if (sig == 0) {
		if (!discard_result)
			fpu_implode(fe, res, FTYPE_EXT, &fpregs[regnum * 3]);

		/* update fpsr according to the result of operation */
		fpu_upd_fpsr(fe, res);
#ifdef DEBUG_FPE
		if (!discard_result) {
			printf("%s: %08x,%08x,%08x stored in FP%d\n", __func__,
			    fpregs[regnum * 3],
			    fpregs[regnum * 3 + 1],
			    fpregs[regnum * 3 + 2],
			    regnum);
		} else {
			static const char *class_name[] =
			    { "SNAN", "QNAN", "ZERO", "NUM", "INF" };
			printf("%s: result(%s,%c,%d,%08x,%08x,%08x) "
			    "discarded\n", __func__,
			    class_name[res->fp_class + 2],
			    res->fp_sign ? '-' : '+', res->fp_exp,
			    res->fp_mant[0], res->fp_mant[1],
			    res->fp_mant[2]);
		}
#endif
	} else {
		DPRINTF(("%s: received signal %d\n", __func__, sig));
	}

	DPRINTF(("%s: FPSR = %08x, FPCR = %08x\n", __func__,
	    fe->fe_fpsr, fe->fe_fpcr));

	DUMP_INSN(insn);

	return sig;
}

/*
 * test condition code according to the predicate in the opcode.
 * returns -1 when the predicate evaluates to true, 0 when false.
 * signal numbers are returned when an error is detected.
 */
static int
test_cc(struct fpemu *fe, int pred)
{
	int result, sig_bsun, invert;
	int fpsr;

	fpsr = fe->fe_fpsr;
	invert = 0;
	fpsr &= ~FPSR_EXCP;		/* clear all exceptions */
	DPRINTF(("%s: fpsr=0x%08x\n", __func__, fpsr));
	pred &= 0x3f;		/* lowest 6 bits */

	DPRINTF(("%s: ", __func__));

	if (pred >= 0x20) {
		DPRINTF(("Illegal condition code\n"));
		return SIGILL;
	} else if (pred & 0x10) {
		/* IEEE nonaware tests */
		sig_bsun = 1;
		pred &= 0x0f;		/* lower 4 bits */
	} else {
		/* IEEE aware tests */
		DPRINTF(("IEEE "));
		sig_bsun = 0;
	}

	if (pred & 0x08) {
		DPRINTF(("Not "));
		/* predicate is "NOT ..." */
		pred ^= 0xf;		/* invert */
		invert = -1;
	}
	switch (pred) {
	case 0:			/* (Signaling) False */
		DPRINTF(("False"));
		result = 0;
		break;
	case 1:			/* (Signaling) Equal */
		DPRINTF(("Equal"));
		result = -((fpsr & FPSR_ZERO) == FPSR_ZERO);
		break;
	case 2:			/* Greater Than */
		DPRINTF(("GT"));
		result = -((fpsr & (FPSR_NAN|FPSR_ZERO|FPSR_NEG)) == 0);
		break;
	case 3:			/* Greater or Equal */
		DPRINTF(("GE"));
		result = -((fpsr & FPSR_ZERO) ||
		    (fpsr & (FPSR_NAN|FPSR_NEG)) == 0);
		break;
	case 4:			/* Less Than */
		DPRINTF(("LT"));
		result = -((fpsr & (FPSR_NAN|FPSR_ZERO|FPSR_NEG)) == FPSR_NEG);
		break;
	case 5:			/* Less or Equal */
		DPRINTF(("LE"));
		result = -((fpsr & FPSR_ZERO) ||
		    ((fpsr & (FPSR_NAN|FPSR_NEG)) == FPSR_NEG));
		break;
	case 6:			/* Greater or Less than */
		DPRINTF(("GLT"));
		result = -((fpsr & (FPSR_NAN|FPSR_ZERO)) == 0);
		break;
	case 7:			/* Greater, Less or Equal */
		DPRINTF(("GLE"));
		result = -((fpsr & FPSR_NAN) == 0);
		break;
	default:
		/* invalid predicate */
		DPRINTF(("Invalid predicate\n"));
		return SIGILL;
	}
	/* if the predicate is "NOT ...", then invert the result */
	result ^= invert;
	DPRINTF(("=> %s (%d)\n", result ? "true" : "false", result));
	/* if it's an IEEE unaware test and NAN is set, BSUN is set */
	if (sig_bsun && (fpsr & FPSR_NAN)) {
		fpsr |= FPSR_BSUN;
	}

	/* put fpsr back */
	fe->fe_fpframe->fpf_fpsr = fe->fe_fpsr = fpsr;

	return result;
}

/*
 * type 1: fdbcc, fscc, ftrapcc
 * In this function, we know:
 *   (opcode & 0x01C0) == 0x0040
 */
static int
fpu_emul_type1(struct fpemu *fe, struct instruction *insn)
{
	struct frame *frame = fe->fe_frame;
	int advance, sig, branch, displ;
	unsigned short sval;

	branch = test_cc(fe, insn->is_word1);
	fe->fe_fpframe->fpf_fpsr = fe->fe_fpsr;

	insn->is_advance = 4;
	sig = 0;

	switch (insn->is_opcode & 070) {
	case 010:			/* fdbcc */
		if (branch == -1) {
			/* advance */
			insn->is_advance = 6;
		} else if (!branch) {
			/* decrement Dn and if (Dn != -1) branch */
			uint16_t count = frame->f_regs[insn->is_opcode & 7];

			if (count-- != 0) {
				if (ufetch_short((void *)(insn->is_pc +
							   insn->is_advance),
						  &sval)) {
					DPRINTF(("%s: fault reading "
					    "displacement\n", __func__));
					return SIGSEGV;
				}
				displ = sval;
				/* sign-extend the displacement */
				displ &= 0xffff;
				if (displ & 0x8000) {
					displ |= 0xffff0000;
				}
				insn->is_advance += displ;
#if 0				/* XXX */
				insn->is_nextpc = insn->is_pc +
				    insn->is_advance;
#endif
			} else {
				insn->is_advance = 6;
			}
			/* write it back */
			frame->f_regs[insn->is_opcode & 7] &= 0xffff0000;
			frame->f_regs[insn->is_opcode & 7] |= (uint32_t)count;
		} else {		/* got a signal */
			sig = SIGFPE;
		}
		break;

	case 070:			/* ftrapcc or fscc */
		advance = 4;
		if ((insn->is_opcode & 07) >= 2) {
			switch (insn->is_opcode & 07) {
			case 3:		/* long opr */
				advance += 2;
			case 2:		/* word opr */
				advance += 2;
			case 4:		/* no opr */
				break;
			default:
				return SIGILL;
				break;
			}

			if (branch == 0) {
				/* no trap */
				insn->is_advance = advance;
				sig = 0;
			} else {
				/* trap */
				sig = SIGFPE;
			}
			break;
		}

		/* FALLTHROUGH */
	default:			/* fscc */
		insn->is_advance = 4;
		insn->is_datasize = 1;	/* always byte */
		sig = fpu_decode_ea(frame, insn, &insn->is_ea, insn->is_opcode);
		if (sig) {
			break;
		}
		if (branch == -1 || branch == 0) {
			/* set result */
			sig = fpu_store_ea(frame, insn, &insn->is_ea,
			    (char *)&branch);
		} else {
			/* got an exception */
			sig = branch;
		}
		break;
	}
	return sig;
}

/*
 * Type 2 or 3: fbcc (also fnop)
 * In this function, we know:
 *   (opcode & 0x0180) == 0x0080
 */
static int
fpu_emul_brcc(struct fpemu *fe, struct instruction *insn)
{
	int displ, word2;
	int sig;
	unsigned short sval;

	/*
	 * Get branch displacement.
	 */
	insn->is_advance = 4;
	displ = insn->is_word1;

	if (insn->is_opcode & 0x40) {
		if (ufetch_short((void *)(insn->is_pc + insn->is_advance),
				  &sval)) {
			DPRINTF(("%s: fault reading word2\n", __func__));
			return SIGSEGV;
		}
		word2 = sval;
		displ <<= 16;
		displ |= word2;
		insn->is_advance += 2;
	} else {
		/* displacement is word sized */
		if (displ & 0x8000)
			displ |= 0xFFFF0000;
	}

	/* XXX: If CC, insn->is_pc += displ */
	sig = test_cc(fe, insn->is_opcode);
	fe->fe_fpframe->fpf_fpsr = fe->fe_fpsr;

	if (fe->fe_fpsr & fe->fe_fpcr & FPSR_EXCP) {
		return SIGFPE;		/* caught an exception */
	}
	if (sig == -1) {
		/*
		 * branch does take place; 2 is the offset to the 1st disp word
		 */
		insn->is_advance = displ + 2;
#if 0		/* XXX */
		insn->is_nextpc = insn->is_pc + insn->is_advance;
#endif
	} else if (sig)
		return SIGILL;		/* got a signal */
	DPRINTF(("%s: %s insn @ %x (%x+%x) (disp=%x)\n", __func__,
	    (sig == -1) ? "BRANCH to" : "NEXT",
	    insn->is_pc + insn->is_advance, insn->is_pc, insn->is_advance,
	    displ));
	return 0;
}
