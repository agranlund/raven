#include <stdio.h>
#include <stdlib.h>
#include "testconfig.h"
/* will be redefined in fpe_atari.h */
#undef LITTLE_ENDIAN
#undef BIG_ENDIAN
#undef PDP_ENDIAN
#undef BYTE_ORDER
#define int8_t xxint8_t
#include "fpe_atari.h"
#undef int8_t
#include <machine/reg.h>
#include "fpu_arith.h"
#include "fpu_emulate.h"

/*
 * When compiling for 68000 only, the FPU opcodes cannot be used
 */
#if defined(__mc68020__) && !defined(__HAVE_68881__)
#undef TEST_OP_F_F
#undef TEST_OP_F_FF
#undef TEST_OP_FF_F
#undef TEST_OP_I_FF
#endif


#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
struct float96 {
	uint16_t exponent;
	uint16_t empty;
	uint32_t mantissa0;
	uint32_t mantissa1;
};
#define HEXCONSTE(v, exp, mant0, mant1) { { (uint32_t)(exp) << 16, mant0, mant1 } }
#endif

union ld_union {
	uint32_t fpe[3];
	struct float96 v;
	long double x;
	double d;
	float s;
};

typedef struct { int line; union ld_union x; union ld_union e; unsigned int flags; } test_f_f_data;
typedef struct { int line; union ld_union x; union ld_union y; union ld_union e; unsigned int flags; } test_f_ff_data;
typedef struct { int line; union ld_union x; union ld_union r1; union ld_union r2; unsigned int flags; } test_ff_f_data;

#define FLAG_INEXACT          0x0001 /* accept slightly inexact values */
#define FLAG_SUBNORM          0x0002 /* test uses subnormal min values */
#define FLAG_MINVAL           0x0004 /* test uses minimum values */
#define FLAG_FAIL_ARANYM      0x0008 /* test currently fails on ARAnyM, but shouldn't */
#define FLAG_FAIL_HATARI      0x0010 /* test currently fails on Hatari, but shouldn't */
#define FLAG_FAIL_STONX       0x0020 /* test currently fails on STonX, but shouldn't */
#define FLAG_FAIL_JIT         0x0040 /* test currently fails when using JIT */
#define FLAG_XFAIL_LINUX      0x0080 /* test currently fails on linux */
#define FLAG_FAIL_X87         0x0100 /* test currently fails when emulation uses x87 host FPU */
#define FLAG_INEXACT2         0x0200 /* accept even more inexact values */
#define FLAG_INEXACT3         0x0400 /* accept even more inexact values */
#define FLAG_INEXACT4         0x0800 /* accept even more inexact values */
#define FLAG_INEXACT5         0x1000 /* accept even more inexact values */
#define FLAG_INEXACT_SUBNORM  0x2000 /* accept inexact subnorm results */

/*
 * exceptions that should be raised (not checked yet)
 */
#define INEXACT_EXCEPTION        0x00010000
#define NO_INEXACT_EXCEPTION     0x00020000
#define UNDERFLOW_EXCEPTION      0x00040000
#define OVERFLOW_EXCEPTION       0x00080000
#define INVALID_EXCEPTION        0x00100000
#define DIVIDE_BY_ZERO_EXCEPTION 0x00200000


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


int numtests;
int testonly;

struct frame frame;
struct fpframe fpframe;
struct fpemu fe = { &frame, &fpframe, 0, 0, { FPC_ZERO, 0, 0, 0, { 0, 0, 0 } }, { FPC_ZERO, 0, 0, 0, { 0, 0, 0 } }, { FPC_ZERO, 0, 0, 0, { 0, 0, 0 } } };
	

#define CHECK(var) \
	this_fail = var; \
	if (var) \
	{ \
		fprintf(stderr, "%s:%d: test %d(%d): failed" CR "\n", __FILE__, __LINE__, numtests, i); \
		status |= this_fail != 0; \
	}

void panic(const char *fmt, ...)
{
	fprintf(stderr, "FPE PANIC [%s]!\n", fmt);
}

int fpe_abort(struct frame *frame, int signo, int code)
{
	fprintf(stderr, "FPE ABORT! pc = %08x\n", frame->f_pc);
	return signo;
}


/*
 * libcmini does not correctly print inf & NaN;
 * mintlib seems to be buggy with printing subnormal & low values
 */
static const char *print_ld(const union ld_union *x)
{
	int sign = (x->v.exponent & 0x8000) != 0;
	static int nbuf = 0;
	static char buf[2][8000];		/* yes, it has to be that large; mintlib sometimes prints lots of digits */
	char *p;
	union ld_union r;
	
	if ((x->v.exponent & 0x7fff) == 0)
	{
		/*
		 * zero, or subnormal
		 */
		if (x->v.mantissa0 == 0 && x->v.mantissa1 == 0)
			return sign ? "-zero" : "+zero";
		return sign ? "-denorm" : "+denorm";
	} else if ((x->v.exponent & 0x7fff) == 0x7fff)
	{
		/* inf or nan */
 		if ((x->v.mantissa0 & 0x7fffffff) == 0 && x->v.mantissa1 == 0)
			return sign ? "-inf" : "+inf";
		if ((x->v.mantissa0 & 0x40000000L) == 0)
			return sign ? "-sNaN" : "+sNaN";
		return sign ? "-NaN" : "+NaN";
	} else if ((x->v.exponent & 0x7fff) <= 0x0001 && !(x->v.mantissa0 & 0x80000000))
	{
		/*
		 * subnormal
		 */
		return sign ? "-denorm" : "+denorm";
	}
	/*
	 * hopefully some printable value now
	 */
	p = buf[nbuf];
	nbuf++;
	if (nbuf >= 2)
		nbuf = 0;
	/*
	 * avoid problems with impossible patterns
	 */
	r = *x;
	r.v.mantissa0 |= 0x80000000L;
#ifdef __AMIGA__
	sprintf(p, "%.22e", (double)r.x);
#else
	sprintf(p, "%.22Le", r.x);
#endif
	return p;
}

__attribute__((__noinline__, __unused__))
static int check_fp(uint16_t exponent, uint32_t mant0, uint32_t mant1, const union ld_union *f, int flags, int i, const char *file, int line)
{
	if (exponent == f->v.exponent && mant0 == f->v.mantissa0 && mant1 == f->v.mantissa1)
		return 0;
	/*
	 * if we expect a nan, accept *any* value that would be interpreted as NaN
	 * (that is, any non-zero bit pattern in the mantissa)
	 */
	if ((exponent & 0x7fff) == 0x7fff && (f->v.exponent & 0x7fff) == 0x7fff)
	{
		if (((mant0 & 0x7fffffff) != 0 || mant1 != 0) && ((f->v.mantissa0 & 0x7fffffff) != 0 || f->v.mantissa1 != 0))
			return 0;
		if (((mant0 & 0x7fffffff) == 0 && mant1 == 0) && ((f->v.mantissa0 & 0x7fffffff) == 0 && f->v.mantissa1 == 0))
			return 0;
	}
	if (flags & (FLAG_INEXACT|FLAG_INEXACT2|FLAG_INEXACT3|FLAG_INEXACT4|FLAG_INEXACT5|FLAG_INEXACT_SUBNORM))
	{
#if ALLOW_INEXACT
		/*
		 * allow for inexactness in least-significant 2 bits
		 */
		int expdiff = (exponent & 0x7fff) - (f->v.exponent & 0x7fff);

		if ((flags & FLAG_INEXACT_SUBNORM) && (exponent & 0x7fff) == 0 && exponent == f->v.exponent)
		{
			if ((f->v.mantissa0 & 0x7fffffff) == (mant0 & 0x7fffffff))
				return 0;
		} else if ((flags & FLAG_INEXACT_SUBNORM) && (exponent & 0x7fff) == 1 && exponent == f->v.exponent)
		{
			if ((f->v.mantissa0 & 0x7fffffff) == (mant0 & 0x7fffffff) && (f->v.mantissa1 & 0xffff0000) == (mant1 & 0xffff0000))
				return 0;
		} else if ((flags & FLAG_INEXACT4) && expdiff == 0)
		{
			union ld_union diffval, t;

			/*
			 * used for cos(x) with x near pi, which should yield values close to zero
			 */
			t = *f;
			diffval.v.mantissa0 = mant0;
			diffval.v.mantissa1 = mant1;
			diffval.v.exponent = 0x3fff;
			t.v.exponent = 0x3fff - expdiff;
			diffval.x -= t.x;
			diffval.v.exponent &= 0x7fff;
			expdiff = 0x3fff - diffval.v.exponent;
			if (expdiff >= 2)
				return 0;
		} else if (expdiff == 1 || expdiff == -1)
		{
			union ld_union diffval, t;

			t = *f;
			diffval.v.mantissa0 = mant0;
			diffval.v.mantissa1 = mant1;
			diffval.v.exponent = 0x3fff;
			t.v.exponent = 0x3fff - expdiff;
			diffval.x -= t.x;
			diffval.v.exponent &= 0x7fff;
			expdiff = 0x3fff - diffval.v.exponent;
			if (expdiff >= 60)
				return 0;
			if (flags & FLAG_INEXACT2)
				if (expdiff >= 57)
					return 0;
			if (flags & FLAG_INEXACT3)
				if (expdiff >= 54)
					return 0;
			if (flags & FLAG_INEXACT5)
				if (expdiff >= 51)
					return 0;
		} else if (expdiff == 0)
		{
			uint32_t diff;

			if (mant1 > f->v.mantissa1)
			{
				diff = mant1 - f->v.mantissa1;
			} else
			{
				diff = f->v.mantissa1 - mant1;
			}
			if (mant0 == (f->v.mantissa0 + 1))
				diff = -diff;
			else if (mant0 != f->v.mantissa0)
				diff = 0x7fffffff;
			if (diff <= 4)
				return 0;
			if (flags & FLAG_INEXACT2)
				if (diff <= 32)
					return 0;
			if (flags & FLAG_INEXACT3)
				if (diff <= 128)
					return 0;
			if (flags & FLAG_INEXACT5)
				if (diff <= 1280)
					return 0;
		}
#endif
	}
	if (flags & FLAG_SUBNORM)
	{
		if (!ALLOW_SUBNORM_MIN)
			return 0;
	}
	if (flags & FLAG_MINVAL)
	{
		if (!ALLOW_MINVAL)
			return 0;
	}
	if (flags & FLAG_XFAIL_LINUX)
	{
#ifdef __linux__
		return 0;
#endif
	}
	if (flags & FLAG_FAIL_X87)
	{
#if defined(__i386__) || defined(__x86_64__)
		return 0;
#endif
	}
	if (flags & (FLAG_FAIL_STONX|FLAG_FAIL_X87))
	{
#if defined(IN_STONX) && IN_STONX
		return 0;
#endif
	}
	if (flags & (FLAG_FAIL_ARANYM|FLAG_FAIL_X87))
	{
#if defined(IN_ARANYM) && IN_ARANYM
		return 0;
#endif
	}
	if (flags & FLAG_FAIL_HATARI)
	{
#if defined(IN_HATARI) && IN_HATARI
		return 0;
#endif
	}
	if (JIT_LOOPS > 1)
		fprintf(stderr, "%s:%d: test %d(%d): expected %04x:%08lx:%08lx got %04x:%08lx:%08lx (%s)" CR "\n", 
			file, line, numtests, i,
			exponent, (unsigned long)mant0, (unsigned long)mant1,
			f->v.exponent, (unsigned long)f->v.mantissa0, (unsigned long)f->v.mantissa1,
			print_ld(f));
	else
		fprintf(stderr, "%s:%d: test %d): expected %04x:%08lx:%08lx got %04x:%08lx:%08lx (%s)" CR "\n", 
			file, line, numtests,
			exponent, (unsigned long)mant0, (unsigned long)mant1,
			f->v.exponent, (unsigned long)f->v.mantissa0, (unsigned long)f->v.mantissa1,
			print_ld(f));
	return 1;
}



#define EXPECT_FP_FLAGS(f, exp, mant0, mant1, flags) \
	this_fail = check_fp(exp, mant0, mant1, &f, flags, i, __FILE__, __LINE__); \
	status |= this_fail != 0

#define EXPECT_FP(f, exp, mant0, mant1) EXPECT_FP_FLAGS(f, exp, mant0, mant1, 0)

#define EXPECT_FP_CONST(f, c) EXPECT_FP(f, c.v.exponent, c.v.mantissa0, c.v.mantissa1)

#define EXPECT_FP_CONST_FLAGS(f, c, flags) EXPECT_FP_FLAGS(f, c.v.exponent, c.v.mantissa0, c.v.mantissa1, flags)

#define EXPECT(r, v) \
	this_fail = ((r) != (v)); \
	if (this_fail) \
	{ \
		fprintf(stderr, "%s:%d: test %d(%d): expected 0x%08lx, got 0x%08lx" CR "\n", __FILE__, __LINE__, numtests, i, v, r); \
		status |= this_fail != 0; \
	}

#define EXPECT_FLAGS(r, v, flags) \
	this_fail = ((r) != (v)); \
	if (((flags) & (FLAG_FAIL_STONX|FLAG_FAIL_X87)) && IN_STONX) this_fail = 0; \
	if (((flags) & (FLAG_FAIL_ARANYM|FLAG_FAIL_X87)) && IN_ARANYM) this_fail = 0; \
	if (this_fail) \
	{ \
		fprintf(stderr, "%s:%d: test %d(%d): expected 0x%08lx, got 0x%08lx" CR "\n", __FILE__, __LINE__, numtests, i, v, r); \
		status |= this_fail != 0; \
	}





/*
 * body for monadic operations:
 * fabs, facos, fasin, fatan, fatanh, fcos, fcosh,
 * fetox, fetoxm1, fgetexp, fgetman, fint, fintrz,
 * flog10, flog2, flogn, flognp1, fneg,
 * fsinh, fsqrt, ftan, ftanh, ftentox, ftst, ftwotox
 */

#if defined(TEST_OP_F_F)

#define TEST_OP_BODY_F_F(_x) \
	__asm__ __volatile__( \
		"\t" TEST_OP_F_F ".x %[x],%[fp0]\n" \
	: [fp0]"=f"(fp0.x) \
	: [x]"m"(_x) \
	: "cc", "memory")

__attribute__((__noinline__, __unused__))
static int test_table_f_f_op(const test_f_f_data *data, size_t n, const char *file)
{
	int status = 0;
	size_t l;
	int i;
	int this_fail;
	union ld_union fp0;

	for (l = 0; l < n; l++)
	{
		numtests++;
		if (testonly == 0 || testonly == numtests)
		{
			for (i = 0; i < JIT_LOOPS; i++)
			{
				TEST_OP_BODY_F_F(data[l].x);
				this_fail = check_fp(data[l].e.v.exponent, data[l].e.v.mantissa0, data[l].e.v.mantissa1, &fp0, data[l].flags, i, file, data[l].line);
				if (this_fail)
				{
					status |= this_fail != 0;
					break;
				}
			}
		}
	}
	
	return status;
}

#endif

#if defined(TEST_FUNC_F_F)

__attribute__((__noinline__, __unused__))
static int test_table_f_f_func(const test_f_f_data *data, size_t n, const char *file)
{
	int status = 0;
	size_t l;
	int i;
	int this_fail;
	union ld_union fp0;
	struct fpn res;

	for (l = 0; l < n; l++)
	{
		numtests++;
		if (testonly == 0 || testonly == numtests)
		{
			for (i = 0; i < JIT_LOOPS; i++)
			{
				fpu_explode(&fe, &fe.fe_f2, FTYPE_EXT, data[l].x.fpe);
				res = *TEST_FUNC_F_F(&fe);
				fpu_implode(&fe, &res, FTYPE_EXT, fp0.fpe);
				this_fail = check_fp(data[l].e.v.exponent, data[l].e.v.mantissa0, data[l].e.v.mantissa1, &fp0, data[l].flags, i, file, data[l].line);
				if (this_fail)
				{
					status |= this_fail != 0;
					break;
				}
			}
		}
	}
	
	return status;
}

#endif


/*
 * body for dyadic operations:
 * fadd, fcmp, fdiv, fmod, fmul, frem,
 * fscale, fsgldiv, fsglmul, fsub
 */
#if defined(TEST_OP_F_FF)

#define TEST_OP_BODY_F_FF(_x, _y) \
	__asm__ __volatile__( \
		"\tfmove.x %[x],%[fp0]\n" \
		"\t" TEST_OP_F_FF ".x %[y],%[fp0]\n" \
	: [fp0]"=&f"(fp0.x) \
	: [x]"m"(_x), [y]"m"(_y) \
	: "cc", "memory")

__attribute__((__unused__))
static int test_table_f_ff_op(const test_f_ff_data *data, size_t n, const char *file)
{
	size_t l;
	int i;
	int status = 0, this_fail;
	union ld_union fp0;
	
	for (l = 0; l < n; l++)
	{
		numtests++;
		if (testonly == 0 || testonly == numtests)
		{
			for (i = 0; i < JIT_LOOPS; i++)
			{
				TEST_OP_BODY_F_FF(data[l].x, data[l].y);
				this_fail = check_fp(data[l].e.v.exponent, data[l].e.v.mantissa0, data[l].e.v.mantissa1, &fp0, data[l].flags, i, file, data[l].line);
				if (this_fail)
				{
					status |= this_fail != 0;
					break;
				}
			}
		}
	}
	
	return status;
}

#endif

#if defined(TEST_FUNC_F_FF)

__attribute__((__unused__))
static int test_table_f_ff_func(const test_f_ff_data *data, size_t n, const char *file)
{
	size_t l;
	int i;
	int status = 0, this_fail;
	union ld_union fp0;
	struct fpn res;
	
	for (l = 0; l < n; l++)
	{
		numtests++;
		if (testonly == 0 || testonly == numtests)
		{
			for (i = 0; i < JIT_LOOPS; i++)
			{
				fpu_explode(&fe, &fe.fe_f1, FTYPE_EXT, data[l].x.fpe);
				fpu_explode(&fe, &fe.fe_f2, FTYPE_EXT, data[l].y.fpe);
				res = *TEST_FUNC_F_FF(&fe);
				fpu_implode(&fe, &res, FTYPE_EXT, fp0.fpe);
				
				this_fail = check_fp(data[l].e.v.exponent, data[l].e.v.mantissa0, data[l].e.v.mantissa1, &fp0, data[l].flags, i, file, data[l].line);
				if (this_fail)
				{
					status |= this_fail != 0;
					break;
				}
			}
		}
	}
	
	return status;
}

#endif
	

/*
 * body for fsincos which returns two values
 */
#if defined(TEST_OP_FF_F)

#define TEST_OP_BODY_FF_F(_x, _r1, _r2) \
	__asm__ __volatile__( \
		"\tfmove.x %[x],%[fp0]\n" \
		"\t" TEST_OP_FF_F ".x %[fp0],%[r2],%[r1]\n" \
	: [fp0]"=&f"(fp0.x), [r1]"=&f"(_r1.x), [r2]"=&f"(_r2.x) \
	: [x]"m"(_x) \
	: "cc", "memory")

static __inline int test_table_ff_f_op(const test_ff_f_data *data, size_t n, const char *file)
{
	size_t l;
	int i;
	int status = 0, this_fail;
	union ld_union fp0;
	union ld_union fp1;
	union ld_union fp2;
	
	for (l = 0; l < n; l++)
	{
		numtests++;
		if (testonly == 0 || testonly == numtests)
		{
			for (i = 0; i < JIT_LOOPS; i++)
			{
				TEST_OP_BODY_FF_F(data[l].x, fp1, fp2);
				
				this_fail = check_fp(data[l].r1.v.exponent, data[l].r1.v.mantissa0, data[l].r1.v.mantissa1, &fp1, data[l].flags, i, file, data[l].line);
				this_fail |= check_fp(data[l].r2.v.exponent, data[l].r2.v.mantissa0, data[l].r2.v.mantissa1, &fp2, data[l].flags, i, file, data[l].line);
				if (this_fail)
				{
					status |= this_fail != 0;
					break;
				}
			}
		}
	}
	
	return status;
}

#endif

#if defined(TEST_FUNC_FF_F)

static __inline int test_table_ff_f_func(const test_ff_f_data *data, size_t n, const char *file)
{
	size_t l;
	int i;
	int status = 0, this_fail;
	union ld_union fp1;
	union ld_union fp2;
	struct fpn res;
	
	for (l = 0; l < n; l++)
	{
		numtests++;
		if (testonly == 0 || testonly == numtests)
		{
			for (i = 0; i < JIT_LOOPS; i++)
			{
				fpu_explode(&fe, &fe.fe_f2, FTYPE_EXT, data[l].x.fpe);
				res = *TEST_FUNC_FF_F(&fe, 7);
				fpu_implode(&fe, &res, FTYPE_EXT, fp1.fpe);
				/* cos result already imploded to fpframe */
				fp2.fpe[0] = fpframe.fpf_regs[7 * 3 + 0];
				fp2.fpe[1] = fpframe.fpf_regs[7 * 3 + 1];
				fp2.fpe[2] = fpframe.fpf_regs[7 * 3 + 2];
				
				this_fail = check_fp(data[l].r1.v.exponent, data[l].r1.v.mantissa0, data[l].r1.v.mantissa1, &fp1, data[l].flags, i, file, data[l].line);
				this_fail |= check_fp(data[l].r2.v.exponent, data[l].r2.v.mantissa0, data[l].r2.v.mantissa1, &fp2, data[l].flags, i, file, data[l].line);
				if (this_fail)
				{
					status |= this_fail != 0;
					break;
				}
			}
		}
	}
	
	return status;
}

#endif


/* Exception Enable Byte */
#define		FPCR_EXCEPTION_ENABLE	0x0000ff00
#define		FPCR_EXCEPTION_BSUN		0x00008000
#define		FPCR_EXCEPTION_SNAN		0x00004000
#define		FPCR_EXCEPTION_OPERR	0x00002000
#define		FPCR_EXCEPTION_OVFL		0x00001000
#define		FPCR_EXCEPTION_UNFL		0x00000800
#define		FPCR_EXCEPTION_DZ		0x00000400
#define		FPCR_EXCEPTION_INEX2	0x00000200
#define		FPCR_EXCEPTION_INEX1	0x00000100

/* Rounding precision */
#define		FPCR_ROUNDING_PRECISION	0x000000c0
#define		FPCR_PRECISION_SINGLE	0x00000040
#define		FPCR_PRECISION_DOUBLE	0x00000080
#define		FPCR_PRECISION_EXTENDED	0x00000000

/* Rounding mode */
#define		FPCR_ROUNDING_MODE		0x00000030
#define		FPCR_ROUND_NEAR			0x00000000
#define		FPCR_ROUND_ZERO			0x00000010
#define		FPCR_ROUND_MINF			0x00000020
#define		FPCR_ROUND_PINF			0x00000030
	
/* Floating-Point Condition Code Byte */
#undef FPSR_CCB
#define		FPSR_CCB				0x0f000000
#define		FPSR_CCB_NEGATIVE		0x08000000
#define		FPSR_CCB_ZERO			0x04000000
#define		FPSR_CCB_INFINITY		0x02000000
#define		FPSR_CCB_NAN			0x01000000

/* Quotient Byte */
#define		FPSR_QUOTIENT			0x00ff0000
#define		FPSR_QUOTIENT_SIGN		0x00800000
#define		FPSR_QUOTIENT_VALUE		0x007f0000

/* Exception Status Byte */
#define		FPSR_EXCEPTION			FPCR_EXCEPTION_ENABLE
#define		FPSR_EXCEPTION_BSUN		FPCR_EXCEPTION_BSUN
#define		FPSR_EXCEPTION_SNAN		FPCR_EXCEPTION_SNAN
#define		FPSR_EXCEPTION_OPERR	FPCR_EXCEPTION_OPERR
#define		FPSR_EXCEPTION_OVFL		FPCR_EXCEPTION_OVFL
#define		FPSR_EXCEPTION_UNFL		FPCR_EXCEPTION_UNFL
#define		FPSR_EXCEPTION_DZ		FPCR_EXCEPTION_DZ
#define		FPSR_EXCEPTION_INEX2	FPCR_EXCEPTION_INEX2
#define		FPSR_EXCEPTION_INEX1	FPCR_EXCEPTION_INEX1

/* Accrued Exception Byte */
#define		FPSR_ACCRUED_EXCEPTION	0x000000f8
#define		FPSR_ACCR_IOP			0x00000080
#define		FPSR_ACCR_OVFL			0x00000040
#define		FPSR_ACCR_UNFL			0x00000020
#define		FPSR_ACCR_DZ			0x00000010
#define		FPSR_ACCR_INEX			0x00000008



#ifdef __mc68000__

static __inline uint32_t get_fpcr(void)
{
#ifdef __HAVE_68881__
	uint32_t fpcr;
	
	__asm__ __volatile__(
		"\tfmove.l %%fpcr,%[fpcr]\n"
		: [fpcr]"=d"(fpcr)
		:
		: "cc", "memory");
	return fpcr;
#else
	register uint32_t fpcr asm("d0");
	
	__asm__ __volatile__(
		"\t.dc.w 0xf200,0xa800\n"
		: "=d"(fpcr)
		:
		: "cc", "memory");
	return fpcr;
#endif
}

static __inline void set_fpcr(uint32_t fpcr)
{
#ifdef __HAVE_68881__
	__asm__ __volatile__(
		"\tfmove.l %[fpcr],%%fpcr\n"
		:
		: [fpcr]"d"(fpcr)
		: "cc", "memory");
#else
	register uint32_t __fpcr asm("d0") = fpcr;
	__asm__ __volatile__(
		"\t.dc.w 0xf200,0x8800\n"
		:
		: "d"(__fpcr)
		: "cc", "memory");
#endif
}

#else

#include <fpu_control.h>
#include <fenv.h>
#include <stdlib.h>

static __inline void set_fpcr(uint32_t fpcr)
{
	fpu_control_t cw;
	
	_FPU_GETCW(cw);
	cw &= ~(_FPU_RC_NEAREST|_FPU_RC_DOWN|_FPU_RC_UP|_FPU_RC_ZERO);
	cw &= ~(_FPU_EXTENDED|_FPU_DOUBLE|_FPU_SINGLE);
	switch (fpcr & FPCR_ROUNDING_MODE)
	{
	case FPCR_ROUND_NEAR:
		fesetround(1);
		cw |= _FPU_RC_NEAREST;
		break;
	case FPCR_ROUND_ZERO:
		fesetround(0);
		cw |= _FPU_RC_ZERO;
		break;
	case FPCR_ROUND_MINF:
		fesetround(3);
		cw |= _FPU_RC_DOWN;
		break;
	case FPCR_ROUND_PINF:
		fesetround(2);
		cw |= _FPU_RC_UP;
		break;
	default:
		abort();
	}
	switch (fpcr & FPCR_ROUNDING_PRECISION)
	{
		case FPCR_PRECISION_SINGLE: cw |= _FPU_SINGLE; break;
		case FPCR_PRECISION_DOUBLE: cw |= _FPU_DOUBLE; break;
		case FPCR_PRECISION_EXTENDED: cw |= _FPU_EXTENDED; break;
		default: abort();
	}
	_FPU_SETCW(cw);
}

static __inline uint32_t get_fpcr(void)
{
	uint32_t fpcr = 0;
	fpu_control_t cw;
	
	_FPU_GETCW(cw);
	switch (cw & (_FPU_RC_NEAREST|_FPU_RC_DOWN|_FPU_RC_UP|_FPU_RC_ZERO))
	{
		case _FPU_RC_NEAREST: fpcr |= FPCR_ROUND_NEAR; break;
		case _FPU_RC_ZERO: fpcr |= FPCR_ROUND_ZERO; break;
		case _FPU_RC_DOWN: fpcr |= FPCR_ROUND_MINF; break;
		case _FPU_RC_UP: fpcr |= FPCR_ROUND_PINF; break;
	}
	switch (cw & (_FPU_EXTENDED|_FPU_DOUBLE|_FPU_SINGLE))
	{
		case _FPU_SINGLE: fpcr |= FPCR_PRECISION_SINGLE; break;
		case _FPU_DOUBLE: fpcr |= FPCR_PRECISION_DOUBLE; break;
		case _FPU_EXTENDED: fpcr |= FPCR_PRECISION_EXTENDED; break;
	}
	return fpcr;
}

#endif

static __inline void set_precision(uint32_t prec)
{
	uint32_t fpcr;
	
	fpcr = get_fpcr();
	fpcr &= ~FPCR_ROUNDING_PRECISION;
	fpcr |= prec;
	set_fpcr(fpcr);
}


static __inline void set_rounding(uint32_t rounding)
{
	uint32_t fpcr;
	
	fpcr = get_fpcr();
	fpcr &= ~FPCR_ROUNDING_MODE;
	fpcr |= rounding;
	set_fpcr(fpcr);
}


/*
 * some constants
 */
#define ZERO_P HEXCONSTE("+0.0", 0x0000, 0x00000000L, 0x00000000L)
#define ZERO_M HEXCONSTE("-0.0", 0x8000, 0x00000000L, 0x00000000L)
#define INF_P HEXCONSTE("+inf", 0x7fff, 0x00000000L, 0x00000000L)
#define INF_M HEXCONSTE("-inf", 0xffff, 0x00000000L, 0x00000000L)
#define QNAN_P HEXCONSTE("nan", 0x7fff, 0xffffffffL, 0xffffffffL)
#define QNAN_M HEXCONSTE("-nan", 0xffff, 0xffffffffL, 0xffffffffL)
#define SNAN_P HEXCONSTE("snan", 0x7fff, 0xbfffffffL, 0xffffffffL)
#define SNAN_M HEXCONSTE("-snan", 0xffff, 0xbfffffffL, 0xffffffffL)

#ifndef __LDBL_MIN_EXP__
#ifdef __mc68000__
#define __LDBL_MIN_EXP__ (-16382)
#else
#define __LDBL_MIN_EXP__ (-16381)
#endif
#endif

#if __LDBL_MIN_EXP__ <= (-16382)
#define SUBNORM_P HEXCONSTE("+1.82259976594123730126e-4951" /* LDBL_DENORM_MIN */, 0x0000, 0x00000000L, 0x00000001L)
#define SUBNORM_M HEXCONSTE("-1.82259976594123730126e-4951" /* -LDBL_DENORM_MIN */, 0x8000, 0x00000000L, 0x00000001L)
#define MIN_P HEXCONSTE("+1.68105157155604675313e-4932" /* LDBL_MIN */, 0x0000, 0x80000000L, 0x00000000L)
#define MIN_M HEXCONSTE("-1.68105157155604675313e-4932" /* -LDBL_MIN */, 0x8000, 0x80000000L, 0x00000000L)
#else
#define SUBNORM_P HEXCONSTE("+3.64519953188247460253e-4951" /* LDBL_DENORM_MIN */, 0x0000, 0x00000000L, 0x00000001L)
#define SUBNORM_M HEXCONSTE("-3.64519953188247460253e-4951" /* -LDBL_DENORM_MIN */, 0x8000, 0x00000000L, 0x00000001L)
#define MIN_P HEXCONSTE("+3.36210314311209350626e-4932" /* LDBL_MIN */, 0x0001, 0x80000000L, 0x00000000L)
#define MIN_M HEXCONSTE("-3.36210314311209350626e-4932" /* -LDBL_MIN */, 0x8001, 0x80000000L, 0x00000000L)
#endif
#define MAX_P HEXCONSTE("+1.18973149535723176502e+4932" /* LDBL_MAX */, 0x7ffe, 0xffffffffL, 0xffffffffL)
#define MAX_M HEXCONSTE("-1.18973149535723176502e+4932" /* -LDBL_MAX */, 0xfffe, 0xffffffffL, 0xffffffffL)

static union ld_union const zero_p = ZERO_P;
static union ld_union const zero_m = ZERO_M;
static union ld_union const inf_p = INF_P;
static union ld_union const inf_m = INF_M;
static union ld_union const qnan_p = QNAN_P;
static union ld_union const qnan_m = QNAN_M;
static union ld_union const snan_p = SNAN_P;
static union ld_union const snan_m = SNAN_M;
static union ld_union const subnorm_p = SUBNORM_P;
static union ld_union const subnorm_m = SUBNORM_M;
static union ld_union const min_p = MIN_P;
static union ld_union const min_m = MIN_M;
static union ld_union const max_p = MAX_P;
static union ld_union const max_m = MAX_M;

#define HEXCONST_E        HEXCONSTE("2.718281828459045235360287471352662498", 0x4000, 0xadf85458L, 0xa2bb4a9aL) /* e */
#define HEXCONST_LOG2E    HEXCONSTE("1.442695040888963407359924681001892137", 0x3fff, 0xb8aa3b29L, 0x5c17f0bcL) /* log2(e) */
#define HEXCONST_LOG10E   HEXCONSTE("0.434294481903251827651128918916605082", 0x3ffd, 0xde5bd8a9L, 0x37287195L) /* log10(e) */
#define HEXCONST_LN2      HEXCONSTE("0.693147180559945309417232121458176568", 0x3ffe, 0xb17217f7L, 0xd1cf79acL) /* ln(2) */
#define HEXCONST_LN10     HEXCONSTE("2.302585092994045684017991454684364208", 0x4000, 0x935d8dddL, 0xaaa8ac17L) /* ln(10) */
#define HEXCONST_PI       HEXCONSTE("3.141592653589793238462643383279502884", 0x4000, 0xc90fdaa2L, 0x2168c235L) /* pi */
#define HEXCONST_PI_2     HEXCONSTE("1.570796326794896619231321691639751442", 0x3fff, 0xc90fdaa2L, 0x2168c235L) /* pi/2 */
#define HEXCONST_PI_4     HEXCONSTE("0.785398163397448309615660845819875721", 0x3ffe, 0xc90fdaa2L, 0x2168c235L) /* pi/4 */
#define HEXCONST_1_PI     HEXCONSTE("0.318309886183790671537767526745028724", 0x3ffd, 0xa2f9836eL, 0x4e44152aL) /* 1/pi */
#define HEXCONST_2_PI     HEXCONSTE("0.636619772367581343075535053490057448", 0x3ffe, 0xa2f9836eL, 0x4e44152aL) /* 2/pi */
#define HEXCONST_2_SQRTPI HEXCONSTE("1.128379167095512573896158903121545172", 0x3fff, 0x906eba82L, 0x14db688dL) /* 2/sqrt(pi) */
#define HEXCONST_SQRT2    HEXCONSTE("1.414213562373095048801688724209698079", 0x3fff, 0xb504f333L, 0xf9de6484L) /* sqrt(2) */
#define HEXCONST_SQRT1_2  HEXCONSTE("0.707106781186547524400844362104849039", 0x3ffe, 0xb504f333L, 0xf9de6484L) /* 1/sqrt(2) */

#define HEXCONST_PI_M     HEXCONSTE("3.141592653589793238462643383279502884", 0xc000, 0xc90fdaa2L, 0x2168c235L) /* -pi */
#define HEXCONST_PI_2_M   HEXCONSTE("-1.570796326794896619231321691639751442", 0xbfff, 0xc90fdaa2L, 0x2168c235L) /* -pi/2 */
#define HEXCONST_PI_4_M   HEXCONSTE("-0.785398163397448309615660845819875721", 0xbffe, 0xc90fdaa2L, 0x2168c235L) /* -pi/4 */
