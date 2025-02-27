/*
 * Test:USES_FPU
 */
#include "testconfig.h"
#define TEST_OP_F_FF "fsub"
#define TEST_FUNC_F_FF fpu_sub
struct fpemu;
struct fpn *fpu_sub(struct fpemu *fe);
#include "testdriver.h"

/* not available as separate function in emulation */
struct fpn *fpu_sub(struct fpemu *fe)
{
	fe->fe_f2.fp_sign = !fe->fe_f2.fp_sign;
	return fpu_add(fe);
}




static test_f_ff_data const fsub_data[] = {
	{ __LINE__, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), HEXCONSTE("2.0", 0x4000, 0x80000000L, 0x00000000L),  HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), HEXCONSTE("-2.0", 0xc000, 0x80000000L, 0x00000000L), HEXCONSTE("3.0", 0x4000, 0xc0000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), HEXCONSTE("2.0", 0x4000, 0x80000000L, 0x00000000L), HEXCONSTE("-3.0", 0xc000, 0xc0000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), HEXCONSTE("-2.0", 0xc000, 0x80000000L, 0x00000000L), HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), ZERO_P, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), ZERO_M, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), ZERO_P, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), ZERO_M, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), INF_P, INF_P, FLAG_XFAIL_LINUX },
	{ __LINE__, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), INF_M, INF_M, FLAG_XFAIL_LINUX },
	{ __LINE__, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), INF_P, INF_P, FLAG_XFAIL_LINUX },
	{ __LINE__, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), INF_M, INF_M, FLAG_XFAIL_LINUX },
	{ __LINE__, ZERO_P, HEXCONSTE("2.0", 0x4000, 0x80000000L, 0x00000000L),  HEXCONSTE("-2.0", 0xc000, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, ZERO_P, HEXCONSTE("-2.0", 0xc000, 0x80000000L, 0x00000000L), HEXCONSTE("2.0", 0x4000, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, ZERO_M, HEXCONSTE("2.0", 0x4000, 0x80000000L, 0x00000000L), HEXCONSTE("-2.0", 0xc000, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, ZERO_M, HEXCONSTE("-2.0", 0xc000, 0x80000000L, 0x00000000L), HEXCONSTE("2.0", 0x4000, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, ZERO_P, ZERO_P, ZERO_P, 0 },
	{ __LINE__, ZERO_P, ZERO_M, ZERO_P, 0 },
	{ __LINE__, ZERO_M, ZERO_P, ZERO_M, 0 },
	{ __LINE__, ZERO_M, ZERO_M, ZERO_P, 0 },
	{ __LINE__, ZERO_P, INF_P, INF_P, FLAG_XFAIL_LINUX },
	{ __LINE__, ZERO_P, INF_M, INF_M, FLAG_XFAIL_LINUX },
	{ __LINE__, ZERO_M, INF_P, INF_P, FLAG_XFAIL_LINUX },
	{ __LINE__, ZERO_M, INF_M, INF_M, FLAG_XFAIL_LINUX },
	{ __LINE__, INF_P, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), INF_P, FLAG_XFAIL_LINUX },
	{ __LINE__, INF_P, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), INF_P, FLAG_XFAIL_LINUX },
	{ __LINE__, INF_M, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), INF_M, FLAG_XFAIL_LINUX },
	{ __LINE__, INF_M, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), INF_M, FLAG_XFAIL_LINUX },
	{ __LINE__, INF_P, ZERO_P, INF_P, FLAG_XFAIL_LINUX },
	{ __LINE__, INF_P, ZERO_M, INF_P, FLAG_XFAIL_LINUX },
	{ __LINE__, INF_M, ZERO_P, INF_M, FLAG_XFAIL_LINUX },
	{ __LINE__, INF_M, ZERO_M, INF_M, FLAG_XFAIL_LINUX },
	{ __LINE__, INF_P, INF_P, QNAN_P, 0 },
	{ __LINE__, INF_P, INF_M, INF_P, 0 }, /* documentation says -inf, but is wrong */
	{ __LINE__, INF_M, INF_P, INF_M, 0 },
	{ __LINE__, INF_M, INF_M, QNAN_P, 0 },

	{ __LINE__, ZERO_P, QNAN_P, QNAN_P, 0 },
	{ __LINE__, ZERO_P, QNAN_M, QNAN_P, 0 },
	{ __LINE__, ZERO_M, QNAN_P, QNAN_P, 0 },
	{ __LINE__, ZERO_M, QNAN_M, QNAN_P, 0 },
	{ __LINE__, INF_P, QNAN_P, QNAN_P, 0 },
	{ __LINE__, INF_P, QNAN_M, QNAN_M, 0 },
	{ __LINE__, INF_M, QNAN_P, QNAN_M, 0 },
	{ __LINE__, INF_M, QNAN_M, QNAN_P, 0 },
	{ __LINE__, QNAN_P, ZERO_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, ZERO_P, QNAN_P, 0 },
	{ __LINE__, QNAN_P, ZERO_M, QNAN_P, 0 },
	{ __LINE__, QNAN_M, ZERO_M, QNAN_P, 0 },
	{ __LINE__, QNAN_P, INF_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, INF_P, QNAN_M, 0 },
	{ __LINE__, QNAN_P, INF_M, QNAN_M, 0 },
	{ __LINE__, QNAN_M, INF_M, QNAN_P, 0 },
	{ __LINE__, QNAN_P, QNAN_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, QNAN_P, QNAN_P, 0 },
	{ __LINE__, QNAN_P, QNAN_M, QNAN_P, 0 },
	{ __LINE__, QNAN_M, QNAN_M, QNAN_P, 0 },
	{ __LINE__, QNAN_P, QNAN_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, QNAN_P, QNAN_M, 0 },
	{ __LINE__, QNAN_P, QNAN_M, QNAN_M, 0 },
	{ __LINE__, QNAN_M, QNAN_M, QNAN_P, 0 },
	{ __LINE__, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), QNAN_P, QNAN_P, 0 },
	{ __LINE__, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), QNAN_M, QNAN_P, 0 },
	{ __LINE__, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), QNAN_P, QNAN_P, 0 },
	{ __LINE__, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), QNAN_M, QNAN_P, 0 },
	{ __LINE__, QNAN_P, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), QNAN_P, 0 },
	{ __LINE__, QNAN_M, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), QNAN_P, 0 },
	{ __LINE__, QNAN_P, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), QNAN_P, 0 },
	{ __LINE__, QNAN_M, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), QNAN_P, 0 },

	{ __LINE__, HEXCONST_PI, HEXCONSTE("3.0", 0x4000, 0xc0000000L, 0x00000000L), HEXCONSTE("0.141592653589793238462643383279502884", 0x3ffc, 0x90fdaa22L, 0x168c2350L), 0 },
};


int main(int argc, char **argv)
{
	int status;
	
	status = 0;
	
	if (argc > 1)
		testonly = (int)strtol(argv[1], NULL, 0);
	
	set_precision(FPCR_PRECISION_EXTENDED);
	set_rounding(FPCR_ROUND_NEAR);
	
#ifdef TEST_OP_F_FF
	status |= test_table_f_ff_op(fsub_data, ARRAY_SIZE(fsub_data), __FILE__);
#endif
#ifdef TEST_FUNC_F_FF
	status |= test_table_f_ff_func(fsub_data, ARRAY_SIZE(fsub_data), __FILE__);
#endif
	
	return status ? EXIT_FAILURE : EXIT_SUCCESS;
}
