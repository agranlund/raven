/*
 * Test:USES_FPU
 */
#include "testconfig.h"
#define TEST_OP_F_F "fabs"
#define TEST_FUNC_F_F fpu_fabs
struct fpemu;
struct fpn *fpu_fabs(struct fpemu *fe);
#include "testdriver.h"

/* not available as separate function in emulation */
struct fpn *fpu_fabs(struct fpemu *fe)
{
	fe->fe_f2.fp_sign = 0;
	return &fe->fe_f2;
}


static test_f_f_data const fabs_data[] = {
	{ __LINE__, ZERO_P, ZERO_P, 0 },
	{ __LINE__, ZERO_M, ZERO_P, 0 },
	{ __LINE__, INF_P, INF_P, 0 },
	{ __LINE__, INF_M, INF_P, 0 },
	{ __LINE__, QNAN_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, QNAN_P, 0 },
	{ __LINE__, SNAN_P, QNAN_P, 0 },
	{ __LINE__, SNAN_M, QNAN_P, 0 },
	{ __LINE__, SUBNORM_P, SUBNORM_P, 0 },
	{ __LINE__, SUBNORM_M, SUBNORM_P, 0 },
	{ __LINE__, MIN_M, MIN_P, 0 },
	{ __LINE__, MIN_P, MIN_P, 0 },
	{ __LINE__, MAX_M, MAX_P, 0 },
	{ __LINE__, MAX_P, MAX_P, 0 },

	{ __LINE__, HEXCONSTE("38.0", 0x4004, 0x98000000L, 0x00000000L), HEXCONSTE("38.0", 0x4004, 0x98000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("2.718281828459045235360287471352662498", 0x4000, 0xadf85458L, 0xa2bb4a9aL), HEXCONSTE("2.718281828459045235360287471352662498", 0x4000, 0xadf85458L, 0xa2bb4a9aL), 0 },
	{ __LINE__, HEXCONSTE("-2.718281828459045235360287471352662498", 0xc000, 0xadf85458L, 0xa2bb4a9aL), HEXCONSTE("2.718281828459045235360287471352662498", 0x4000, 0xadf85458L, 0xa2bb4a9aL), 0 },
	{ __LINE__, HEXCONSTE("-55", 0xc004, 0xdc000000L, 0x00000000L), HEXCONSTE("55", 0x4004, 0xdc000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-1024", 0xc009, 0x80000000L, 0x00000000L), HEXCONSTE("1024", 0x4009, 0x80000000L, 0x00000000L), 0 },
};

int main(int argc, char **argv)
{
	int status;
	
	status = 0;
	
	if (argc > 1)
		testonly = (int)strtol(argv[1], NULL, 0);
	
	set_precision(FPCR_PRECISION_EXTENDED);
	set_rounding(FPCR_ROUND_NEAR);
	
#ifdef TEST_OP_F_F
	status |= test_table_f_f_op(fabs_data, ARRAY_SIZE(fabs_data), __FILE__);
#endif
#ifdef TEST_FUNC_F_F
	status |= test_table_f_f_func(fabs_data, ARRAY_SIZE(fabs_data), __FILE__);
#endif
	
	return status ? EXIT_FAILURE : EXIT_SUCCESS;
}
