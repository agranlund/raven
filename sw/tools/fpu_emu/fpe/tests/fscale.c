/*
 * Test:USES_FPU
 */
#include "testconfig.h"
#define TEST_OP_F_FF "fscale"
#define TEST_FUNC_F_FF fpu_scale
struct fpemu;
struct fpn *fpu_scale(struct fpemu *fe);
#include "testdriver.h"

/* TODO: fpu_emul_fscale needs an instruction */
#undef TEST_FUNC_F_FF
struct fpn *fpu_scale(struct fpemu *fe)
{
	return &fe->fe_f3;
}

#define ONE_P HEXCONSTE("1", 0x3fff, 0x80000000L, 0x00000000L)
#define ONE_M HEXCONSTE("-1", 0xbfff, 0x80000000L, 0x00000000L)


#if __LDBL_MIN_EXP__ <= (-16382)
#define MIN_HALF_P HEXCONSTE("+8.40525785778023376566e-4933", 0x0000, 0x40000000L, 0x00000000L) /* min_value / 2 */
#define MIN_HALF_M HEXCONSTE("-8.40525785778023376566e-4933", 0x8000, 0x40000000L, 0x00000000L) /* -min_value / 2 */
#define MIN_P_2    HEXCONSTE("+3.36205184148942639058e-4932", 0x0000, 0x7fff8000L, 0x00000000L) /* min_value * 0x0.ffffp0 */
#define MIN_M_2    HEXCONSTE("-3.36205184148942639058e-4932", 0x8000, 0x7fff8000L, 0x00000000L) /* -min_value * 0x0.ffffp0 */
#define MIN_P_15   HEXCONSTE("+2.52157735733407012970e-4932", 0x0000, 0xc0000000L, 0x00000000L) /* min_value * 1.5 */
#define MIN_M_15   HEXCONSTE("-2.52157735733407012970e-4932", 0x8000, 0xc0000000L, 0x00000000L) /* -min_value * 1.5 */
#define MIN_P_125  HEXCONSTE("+2.10131446444505844141e-4932", 0x0000, 0xa0000000L, 0x00000000L) /* min_value * 1.25 */
#define MIN_M_125  HEXCONSTE("-2.10131446444505844141e-4932", 0x8000, 0xa0000000L, 0x00000000L) /* -min_value * 1.25 */
#define MIN_4TH_P  HEXCONSTE("+4.20262892889011688283e-4933", 0x0000, 0x20000000L, 0x00000000L) /* min_value / 4 */
#define MIN_4TH_M  HEXCONSTE("-4.20262892889011688283e-4933", 0x8000, 0x20000000L, 0x00000000L) /* -min_value / 4 */
#define SUBNORM2_P HEXCONSTE("+7.29039906376494920506e-4951", 0x0000, 0x00000000L, 0x00000002L) /* subnorm_value * 2 */
#define SUBNORM2_M HEXCONSTE("-7.29039906376494920506e-4951", 0x8000, 0x00000000L, 0x00000002L) /* -subnorm_value * 2 */
#else
#define MIN_HALF_P HEXCONSTE("+1.68105157155604675313e-4932", 0x0000, 0x40000000L, 0x00000000L) /* min_value / 2 */
#define MIN_HALF_M HEXCONSTE("-1.68105157155604675313e-4932", 0x8000, 0x40000000L, 0x00000000L) /* -min_value / 2 */
#define MIN_P_2    HEXCONSTE("+3.36205184148942639058e-4932", 0x0000, 0x7fff8000L, 0x00000000L) /* min_value * 0x0.ffffp0 */
#define MIN_M_2    HEXCONSTE("-3.36205184148942639058e-4932", 0x8000, 0x7fff8000L, 0x00000000L) /* -min_value * 0x0.ffffp0 */
#define MIN_P_15   HEXCONSTE("+5.04315471466814025939e-4932", 0x0001, 0xc0000000L, 0x00000000L) /* min_value * 1.5 */
#define MIN_M_15   HEXCONSTE("-5.04315471466814025939e-4932", 0x8001, 0xc0000000L, 0x00000000L) /* -min_value * 1.5 */
#define MIN_P_125  HEXCONSTE("+4.20262892889011688283e-4932", 0x0001, 0xa0000000L, 0x00000000L) /* min_value * 1.25 */
#define MIN_M_125  HEXCONSTE("-4.20262892889011688283e-4932", 0x8001, 0xa0000000L, 0x00000000L) /* -min_value * 1.25 */
#define MIN_4TH_P  HEXCONSTE("+8.40525785778023376566e-4933", 0x0000, 0x20000000L, 0x00000000L) /* min_value / 4 */
#define MIN_4TH_M  HEXCONSTE("-8.40525785778023376566e-4933", 0x8000, 0x20000000L, 0x00000000L) /* -min_value / 4 */
#define SUBNORM2_P HEXCONSTE("+7.29039906376494920506e-4951", 0x0000, 0x00000000L, 0x00000002L) /* subnorm_value * 2 */
#define SUBNORM2_M HEXCONSTE("-7.29039906376494920506e-4951", 0x8000, 0x00000000L, 0x00000002L) /* -subnorm_value * 2 */
#endif

#define INT_MAX HEXCONSTE("2147483647", 0x401d, 0xfffffffeL, 0x00000000L)
#define INT_MIN HEXCONSTE("-2147483648", 0xc01e, 0x80000000L, 0x00000000L)

#define MANTDIG_P_M1 HEXCONSTE("63", 0x4004, 0xfc000000L, 0x00000000L)  /* LDBL_MANT_DIG - 1 */
#define MANTDIG_M_P1 HEXCONSTE("-63", 0xc004, 0xfc000000L, 0x00000000L) /* -LDBL_MANT_DIG + 1 */
#define MANTDIG_P    HEXCONSTE("64", 0x4005, 0x80000000L, 0x00000000L)  /* LDBL_MANT_DIG */
#define MANTDIG_M    HEXCONSTE("-64", 0xc005, 0x80000000L, 0x00000000L) /* -LDBL_MANT_DIG */
#define MANTDIG_M_M1 HEXCONSTE("-65", 0xc005, 0x82000000L, 0x00000000L) /* -LDBL_MANT_DIG - 1 */

#define plus_uflow ZERO_P
#define minus_uflow ZERO_M
#define plus_oflow INF_P
#define minus_oflow INF_M


static test_f_ff_data const fscale_data[] = {
	{ __LINE__, ZERO_P, ZERO_P, ZERO_P, 0 },
	{ __LINE__, ZERO_M, ZERO_P, ZERO_M, 0 },
	
	{ __LINE__, INF_P, ONE_P, INF_P, FLAG_XFAIL_LINUX },
	{ __LINE__, INF_M, ONE_P, INF_M, FLAG_XFAIL_LINUX },
	{ __LINE__, QNAN_P, ONE_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, ONE_P, QNAN_P, 0 },
	{ __LINE__, SNAN_P, ONE_P, QNAN_P, 0 },
	{ __LINE__, SNAN_M, ONE_P, QNAN_P, 0 },
	
	{ __LINE__, HEXCONSTE("0.8L", 0x3ffe, 0xccccccccL, 0xcccccccdL), HEXCONSTE("4", 0x4001, 0x80000000L, 0x00000000L), HEXCONSTE("12.8L", 0x4002, 0xccccccccL, 0xcccccccdL), 0 },
	{ __LINE__, HEXCONSTE("-0.854375L", 0xbffe, 0xdab851ebL, 0x851eb852L), HEXCONSTE("5", 0x4001, 0xa0000000L, 0x00000000L), HEXCONSTE("-27.34L", 0xc003, 0xdab851ebL, 0x851eb852L), 0 },
	
	{ __LINE__, ONE_P, ZERO_P, ONE_P, 0 },
	
	{ __LINE__, MIN_HALF_P, ZERO_P, MIN_HALF_P, 0 },
	{ __LINE__, MIN_HALF_M, ZERO_P, MIN_HALF_M, 0 },
	{ __LINE__, MIN_HALF_P, ONE_P, MIN_P, FLAG_MINVAL|FLAG_SUBNORM },
	{ __LINE__, MIN_HALF_M, ONE_P, MIN_M, FLAG_MINVAL|FLAG_SUBNORM },
	{ __LINE__, MIN_P_2, ZERO_P, MIN_P_2, 0 },
	{ __LINE__, MIN_M_2, ZERO_P, MIN_M_2, 0 },
	{ __LINE__, SUBNORM_P, ZERO_P, SUBNORM_P, 0 },
	{ __LINE__, SUBNORM_M, ZERO_P, SUBNORM_M, 0 },
	{ __LINE__, SUBNORM_P, MANTDIG_P_M1, MIN_P, FLAG_SUBNORM },
	{ __LINE__, SUBNORM_M, MANTDIG_P_M1, MIN_M, FLAG_SUBNORM },
	
	{ __LINE__, MIN_P, MANTDIG_M_P1, SUBNORM_P, FLAG_SUBNORM },
	{ __LINE__, MIN_M, MANTDIG_M_P1, SUBNORM_M, FLAG_SUBNORM },
	{ __LINE__, MIN_P, MANTDIG_M, plus_uflow, FLAG_MINVAL|FLAG_SUBNORM },
	{ __LINE__, MIN_M, MANTDIG_M, minus_uflow, FLAG_MINVAL|FLAG_SUBNORM },
	{ __LINE__, MIN_P, MANTDIG_M_M1, plus_uflow, FLAG_MINVAL|FLAG_SUBNORM },
	{ __LINE__, MIN_M, MANTDIG_M_M1, minus_uflow, FLAG_MINVAL|FLAG_SUBNORM },
	{ __LINE__, MIN_P_15, MANTDIG_M_P1, SUBNORM2_P, FLAG_SUBNORM },
	{ __LINE__, MIN_M_15, MANTDIG_M_P1, SUBNORM2_M, FLAG_MINVAL|FLAG_SUBNORM },
	{ __LINE__, MIN_P_15, MANTDIG_M, SUBNORM_P, FLAG_SUBNORM },
	{ __LINE__, MIN_M_15, MANTDIG_M, SUBNORM_M, FLAG_SUBNORM },
	{ __LINE__, MIN_P_15, MANTDIG_M_M1, plus_uflow, 0 },
	{ __LINE__, MIN_M_15, MANTDIG_M_M1, minus_uflow, 0 },
	{ __LINE__, MIN_P_125, MANTDIG_M_P1, SUBNORM_P, FLAG_SUBNORM }, /* works in STonX, but fails in ARAnyM */
	{ __LINE__, MIN_M_125, MANTDIG_M_P1, SUBNORM_M, FLAG_XFAIL_LINUX|FLAG_SUBNORM },
	{ __LINE__, MIN_P_125, MANTDIG_M, SUBNORM_P, FLAG_XFAIL_LINUX|FLAG_SUBNORM },
	{ __LINE__, MIN_M_125, MANTDIG_M, SUBNORM_M, FLAG_SUBNORM }, /* works in STonX, but fails in ARAnyM */
	{ __LINE__, MIN_P_125, MANTDIG_M_M1, plus_uflow, 0 },
	{ __LINE__, MIN_M_125, MANTDIG_M_M1, minus_uflow, 0 },
	
	{ __LINE__, ONE_P, INT_MAX, plus_oflow, 0 },
	{ __LINE__, ONE_P, INT_MIN, plus_uflow, 0 },
	{ __LINE__, MAX_P, INT_MAX, plus_oflow, 0 },
	{ __LINE__, MAX_P, INT_MIN, plus_uflow, 0 },
	{ __LINE__, MIN_P, INT_MAX, plus_oflow, 0 },
	{ __LINE__, MIN_P, INT_MIN, plus_uflow, 0 },
	{ __LINE__, MIN_4TH_P, INT_MAX, plus_oflow, 0 },
	{ __LINE__, MIN_4TH_P, INT_MIN, plus_uflow, 0 },
	{ __LINE__, SUBNORM_P, INT_MAX, plus_oflow, 0 },
	{ __LINE__, SUBNORM_P, INT_MIN, plus_uflow, 0 },
	
	{ __LINE__, ONE_M, INT_MAX, minus_oflow, 0 },
	{ __LINE__, ONE_M, INT_MIN, minus_uflow, 0 },
	{ __LINE__, MAX_M, INT_MAX, minus_oflow, 0 },
	{ __LINE__, MAX_M, INT_MIN, minus_uflow, 0 },
	{ __LINE__, MIN_M, INT_MAX, minus_oflow, 0 },
	{ __LINE__, MIN_M, INT_MIN, minus_uflow, 0 },
	{ __LINE__, MIN_4TH_M, INT_MAX, minus_oflow, 0 },
	{ __LINE__, MIN_4TH_M, INT_MIN, minus_uflow, FLAG_XFAIL_LINUX|FLAG_SUBNORM },
	{ __LINE__, SUBNORM_M, INT_MAX, minus_oflow, 0 },
	{ __LINE__, SUBNORM_M, INT_MIN, minus_uflow, 0 },

	{ __LINE__, HEXCONSTE("5.0", 0x4001, 0xa0000000L, 0x00000000L), ONE_P, HEXCONSTE("10.0", 0x4002, 0xa0000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("5.0", 0x4001, 0xa0000000L, 0x00000000L), ONE_M, HEXCONSTE("2.5", 0x4000, 0xa0000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("0x1p16382", 0x7ffd, 0x80000000L, 0x0L), ONE_P, HEXCONSTE("0x1p16383", 0x7ffe, 0x80000000L, 0x0L), 0 },
	{ __LINE__, HEXCONSTE("0x1p16383", 0x7ffe, 0x80000000L, 0x0L), ONE_P, INF_P, 0 },
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
	status |= test_table_f_ff_op(fscale_data, ARRAY_SIZE(fscale_data), __FILE__);
#endif
#ifdef TEST_FUNC_F_FF
	status |= test_table_f_ff_func(fscale_data, ARRAY_SIZE(fscale_data), __FILE__);
#endif
	
	return status ? EXIT_FAILURE : EXIT_SUCCESS;
}
