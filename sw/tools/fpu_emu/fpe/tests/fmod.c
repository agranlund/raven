/*
 * Test:USES_FPU
 */
#include "testconfig.h"
#define TEST_OP_F_FF "fmod"
#define TEST_FUNC_F_FF fpu_mod
#include "testdriver.h"


static test_f_ff_data const fmod_data[] = {
	/* fmod (+0, y) == +0 for y != 0.  */
	{ __LINE__, ZERO_P, HEXCONSTE("3", 0x4000, 0xc0000000L, 0x00000000L), ZERO_P, 0 },
	{ __LINE__, ZERO_P, SUBNORM_P, ZERO_P, 0 },
	{ __LINE__, ZERO_P, SUBNORM_M, ZERO_P, 0 },
	{ __LINE__, ZERO_P, MIN_P, ZERO_P, FLAG_MINVAL },
	{ __LINE__, ZERO_P, MIN_M, ZERO_P, FLAG_MINVAL },
	{ __LINE__, ZERO_P, MAX_P, ZERO_P, 0 },
	{ __LINE__, ZERO_P, MAX_M, ZERO_P, 0 },
	
	/* fmod (-0, y) == -0 for y != 0.  */
	{ __LINE__, ZERO_M, HEXCONSTE("3", 0x4000, 0xc0000000L, 0x00000000L), ZERO_M, FLAG_FAIL_X87 },
	{ __LINE__, ZERO_M, SUBNORM_P, ZERO_M, FLAG_SUBNORM },
	{ __LINE__, ZERO_M, SUBNORM_M, ZERO_M, FLAG_SUBNORM },
	{ __LINE__, ZERO_M, MIN_P, ZERO_M, FLAG_MINVAL },
	{ __LINE__, ZERO_M, MIN_M, ZERO_M, FLAG_MINVAL },
	{ __LINE__, ZERO_M, MAX_P, ZERO_M, FLAG_FAIL_STONX },
	{ __LINE__, ZERO_M, MAX_M, ZERO_M, FLAG_FAIL_STONX },
	
	/* fmod (+inf, y) == qNaN plus invalid exception.  */
	{ __LINE__, INF_P, HEXCONSTE("3", 0x4000, 0xc0000000L, 0x00000000L), QNAN_P, 0 },
	{ __LINE__, INF_P, HEXCONSTE("-1.1", 0xbfff, 0x8cccccccL, 0xcccccccdL), QNAN_P, 0 },
	{ __LINE__, INF_P, ZERO_P, QNAN_P, 0 },
	{ __LINE__, INF_P, ZERO_M, QNAN_P, 0 },
	{ __LINE__, INF_P, SUBNORM_P, QNAN_P, 0 },
	{ __LINE__, INF_P, MIN_P, QNAN_P, 0 },
	{ __LINE__, INF_P, MAX_P, QNAN_P, 0 },
	{ __LINE__, INF_P, INF_P, QNAN_P, 0 },
	{ __LINE__, INF_P, INF_M, QNAN_P, 0 },

	/* fmod (-inf, y) == qNaN plus invalid exception.  */
	{ __LINE__, INF_M, HEXCONSTE("3", 0x4000, 0xc0000000L, 0x00000000L), QNAN_P, 0 },
	{ __LINE__, INF_M, HEXCONSTE("-1.1", 0xbfff, 0x8cccccccL, 0xcccccccdL), QNAN_P, 0 },
	{ __LINE__, INF_M, ZERO_P, QNAN_P, 0 },
	{ __LINE__, INF_M, ZERO_M, QNAN_P, 0 },
	{ __LINE__, INF_M, SUBNORM_P, QNAN_P, 0 },
	{ __LINE__, INF_M, MIN_P, QNAN_P, 0 },
	{ __LINE__, INF_M, MAX_P, QNAN_P, 0 },
	{ __LINE__, INF_M, INF_P, QNAN_P, 0 },
	{ __LINE__, INF_M, INF_M, QNAN_P, 0 },

	/* fmod (x, +0) == qNaN plus invalid exception.  */
	{ __LINE__, HEXCONSTE("3", 0x4000, 0xc0000000L, 0x00000000L), ZERO_P, QNAN_P, 0 },
	{ __LINE__, HEXCONSTE("-1.1", 0xbfff, 0x8cccccccL, 0xcccccccdL), ZERO_P, QNAN_P, 0 },
	{ __LINE__, ZERO_P, ZERO_P, QNAN_P, 0 },
	{ __LINE__, ZERO_M, ZERO_P, QNAN_P, 0 },
	{ __LINE__, SUBNORM_P, ZERO_P, QNAN_P, 0 },
	{ __LINE__, MIN_P, ZERO_P, QNAN_P, 0 },
	{ __LINE__, MAX_P, ZERO_P, QNAN_P, 0 },

	/* fmod (x, -0) == qNaN plus invalid exception.  */
	{ __LINE__, HEXCONSTE("3", 0x4000, 0xc0000000L, 0x00000000L), ZERO_M, QNAN_P, 0 },
	{ __LINE__, HEXCONSTE("-1.1", 0xbfff, 0x8cccccccL, 0xcccccccdL), ZERO_M, QNAN_P, 0 },
	{ __LINE__, ZERO_P, ZERO_M, QNAN_P, 0 },
	{ __LINE__, ZERO_M, ZERO_M, QNAN_P, 0 },
	{ __LINE__, SUBNORM_P, ZERO_M, QNAN_P, 0 },
	{ __LINE__, MIN_P, ZERO_M, QNAN_P, 0 },
	{ __LINE__, MAX_P, ZERO_M, QNAN_P, 0 },
	
	/* fmod (x, +inf) == x for x not infinite.  */
	{ __LINE__, ZERO_P, INF_P, ZERO_P, FLAG_FAIL_ARANYM|FLAG_FAIL_STONX|FLAG_XFAIL_LINUX },
	{ __LINE__, ZERO_M, INF_P, ZERO_M, FLAG_FAIL_ARANYM|FLAG_FAIL_STONX|FLAG_XFAIL_LINUX },
	{ __LINE__, SUBNORM_P, INF_P, SUBNORM_P, FLAG_SUBNORM },
	{ __LINE__, MIN_P, INF_P, MIN_P, FLAG_MINVAL },
	{ __LINE__, MAX_P, INF_P, MAX_P, FLAG_MINVAL },
	{ __LINE__, HEXCONSTE("3.0", 0x4000, 0xc0000000L, 0x00000000L), INF_P, HEXCONSTE("3.0", 0x4000, 0xc0000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX|FLAG_XFAIL_LINUX },

	/* fmod (x, -inf) == x for x not infinite.  */
	{ __LINE__, ZERO_P, INF_M, ZERO_P, FLAG_FAIL_ARANYM|FLAG_FAIL_STONX|FLAG_XFAIL_LINUX },
	{ __LINE__, ZERO_M, INF_M, ZERO_M, FLAG_FAIL_ARANYM|FLAG_FAIL_STONX|FLAG_XFAIL_LINUX },
	{ __LINE__, SUBNORM_P, INF_M, SUBNORM_P, FLAG_SUBNORM },
	{ __LINE__, MIN_P, INF_M, MIN_P, FLAG_MINVAL },
	{ __LINE__, MAX_P, INF_M, MAX_P, FLAG_MINVAL },
	{ __LINE__, HEXCONSTE("3.0", 0x4000, 0xc0000000L, 0x00000000L), INF_M, HEXCONSTE("3.0", 0x4000, 0xc0000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX|FLAG_XFAIL_LINUX },
	
	{ __LINE__, ZERO_P, QNAN_P, QNAN_P, 0 },
	{ __LINE__, ZERO_P, QNAN_M, QNAN_P, 0 },
	{ __LINE__, ZERO_M, QNAN_P, QNAN_P, 0 },
	{ __LINE__, ZERO_M, QNAN_M, QNAN_P, 0 },
	{ __LINE__, HEXCONSTE("1", 0x3fff, 0x80000000L, 0x00000000L), QNAN_P, QNAN_P, 0 },
	{ __LINE__, HEXCONSTE("1", 0x3fff, 0x80000000L, 0x00000000L), QNAN_M, QNAN_P, 0 },
	{ __LINE__, INF_P, QNAN_P, QNAN_P, 0 },
	{ __LINE__, INF_P, QNAN_M, QNAN_P, 0 },
	{ __LINE__, INF_M, QNAN_P, QNAN_P, 0 },
	{ __LINE__, INF_M, QNAN_M, QNAN_P, 0 },
	{ __LINE__, ZERO_P, SNAN_P, QNAN_P, 0 },
	{ __LINE__, ZERO_P, SNAN_M, QNAN_P, 0 },
	{ __LINE__, ZERO_M, SNAN_P, QNAN_P, 0 },
	{ __LINE__, ZERO_M, SNAN_M, QNAN_P, 0 },
	{ __LINE__, HEXCONSTE("1", 0x3fff, 0x80000000L, 0x00000000L), SNAN_P, QNAN_P, 0 },
	{ __LINE__, HEXCONSTE("1", 0x3fff, 0x80000000L, 0x00000000L), SNAN_M, QNAN_P, 0 },
	{ __LINE__, INF_P, SNAN_P, QNAN_P, 0 },
	{ __LINE__, INF_P, SNAN_M, QNAN_P, 0 },
	{ __LINE__, INF_M, SNAN_P, QNAN_P, 0 },
	{ __LINE__, INF_M, SNAN_M, QNAN_P, 0 },
	{ __LINE__, QNAN_P, ZERO_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, ZERO_P, QNAN_P, 0 },
	{ __LINE__, QNAN_P, ZERO_M, QNAN_P, 0 },
	{ __LINE__, QNAN_M, ZERO_M, QNAN_P, 0 },
	{ __LINE__, QNAN_P, HEXCONSTE("1", 0x3fff, 0x80000000L, 0x00000000L), QNAN_P, 0 },
	{ __LINE__, QNAN_M, HEXCONSTE("1", 0x3fff, 0x80000000L, 0x00000000L), QNAN_P, 0 },
	{ __LINE__, QNAN_P, INF_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, INF_P, QNAN_P, 0 },
	{ __LINE__, QNAN_P, INF_M, QNAN_P, 0 },
	{ __LINE__, QNAN_M, INF_M, QNAN_P, 0 },
	{ __LINE__, SNAN_P, ZERO_P, QNAN_P, 0 },
	{ __LINE__, SNAN_M, ZERO_P, QNAN_P, 0 },
	{ __LINE__, SNAN_P, ZERO_M, QNAN_P, 0 },
	{ __LINE__, SNAN_M, ZERO_M, QNAN_P, 0 },
	{ __LINE__, SNAN_P, HEXCONSTE("1", 0x3fff, 0x80000000L, 0x00000000L), QNAN_P, 0 },
	{ __LINE__, SNAN_M, HEXCONSTE("1", 0x3fff, 0x80000000L, 0x00000000L), QNAN_P, 0 },
	{ __LINE__, SNAN_P, INF_P, QNAN_P, 0 },
	{ __LINE__, SNAN_M, INF_P, QNAN_P, 0 },
	{ __LINE__, SNAN_P, INF_M, QNAN_P, 0 },
	{ __LINE__, SNAN_M, INF_M, QNAN_P, 0 },
	{ __LINE__, QNAN_P, QNAN_P, QNAN_P, 0 },
	{ __LINE__, QNAN_P, QNAN_M, QNAN_P, 0 },
	{ __LINE__, QNAN_M, QNAN_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, QNAN_M, QNAN_P, 0 },
	{ __LINE__, QNAN_P, SNAN_P, QNAN_P, 0 },
	{ __LINE__, QNAN_P, SNAN_M, QNAN_P, 0 },
	{ __LINE__, QNAN_M, SNAN_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, SNAN_M, QNAN_P, 0 },
	{ __LINE__, SNAN_P, QNAN_P, QNAN_P, 0 },
	{ __LINE__, SNAN_P, QNAN_M, QNAN_P, 0 },
	{ __LINE__, SNAN_M, QNAN_P, QNAN_P, 0 },
	{ __LINE__, SNAN_M, QNAN_M, QNAN_P, 0 },
	{ __LINE__, SNAN_P, SNAN_P, QNAN_P, 0 },
	{ __LINE__, SNAN_P, SNAN_M, QNAN_P, 0 },
	{ __LINE__, SNAN_M, SNAN_P, QNAN_P, 0 },
	{ __LINE__, SNAN_M, SNAN_M, QNAN_P, 0 },
	
	{ __LINE__, HEXCONSTE("6.5", 0x4001, 0xd0000000L, 0x00000000L), HEXCONSTE("2.25L", 0x4000, 0x90000000L, 0x00000000L), HEXCONSTE("2.0L", 0x4000, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-6.5", 0xc001, 0xd0000000L, 0x00000000L), HEXCONSTE("2.25L", 0x4000, 0x90000000L, 0x00000000L), HEXCONSTE("-2.0L", 0xc000, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("6.5", 0x4001, 0xd0000000L, 0x00000000L), HEXCONSTE("-2.25L", 0xc000, 0x90000000L, 0x00000000L), HEXCONSTE("2.0L", 0x4000, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-6.5", 0xc001, 0xd0000000L, 0x00000000L), HEXCONSTE("-2.25L", 0xc000, 0x90000000L, 0x00000000L), HEXCONSTE("-2.0L", 0xc000, 0x80000000L, 0x00000000L), 0 },
	
	{ __LINE__, MAX_P, MAX_P, ZERO_P, 0 },
	{ __LINE__, MAX_P, MAX_M, ZERO_P, 0 },
	{ __LINE__, MAX_P, MIN_P, ZERO_P, FLAG_MINVAL },
	{ __LINE__, MAX_P, MIN_M, ZERO_P, FLAG_MINVAL },
	{ __LINE__, MAX_P, SUBNORM_P, ZERO_P, FLAG_SUBNORM },
	{ __LINE__, MAX_P, SUBNORM_M, ZERO_P, FLAG_SUBNORM },
	{ __LINE__, MAX_M, MAX_P, ZERO_M, FLAG_FAIL_STONX },
	{ __LINE__, MAX_M, MAX_M, ZERO_M, FLAG_FAIL_STONX },
	{ __LINE__, MAX_M, MIN_P, ZERO_M, FLAG_MINVAL },
	{ __LINE__, MAX_M, MIN_M, ZERO_M, FLAG_MINVAL },
	{ __LINE__, MAX_M, SUBNORM_P, ZERO_M, FLAG_SUBNORM },
	{ __LINE__, MAX_M, SUBNORM_M, ZERO_M, FLAG_SUBNORM },
	
	{ __LINE__, MIN_P, MAX_P, MIN_P, FLAG_MINVAL },
	{ __LINE__, MIN_P, MAX_M, MIN_P, FLAG_MINVAL },
	{ __LINE__, MIN_P, MIN_P, ZERO_P, FLAG_MINVAL },
	{ __LINE__, MIN_P, MIN_M, ZERO_P, FLAG_MINVAL },
	{ __LINE__, MIN_P, SUBNORM_P, ZERO_P, FLAG_MINVAL },
	{ __LINE__, MIN_P, SUBNORM_M, ZERO_P, FLAG_MINVAL },
	{ __LINE__, MIN_M, MAX_P, MIN_M, FLAG_MINVAL },
	{ __LINE__, MIN_M, MAX_M, MIN_M, FLAG_MINVAL },
	{ __LINE__, MIN_M, MIN_P, ZERO_M, FLAG_MINVAL },
	{ __LINE__, MIN_M, MIN_M, ZERO_M, FLAG_MINVAL },
	{ __LINE__, MIN_M, SUBNORM_P, ZERO_M, FLAG_MINVAL },
	{ __LINE__, MIN_M, SUBNORM_M, ZERO_M, FLAG_MINVAL },
	
	{ __LINE__, SUBNORM_P, MAX_P, SUBNORM_P, 0 },
	{ __LINE__, SUBNORM_P, MAX_M, SUBNORM_P, 0 },
	{ __LINE__, SUBNORM_P, MIN_P, SUBNORM_P, FLAG_MINVAL },
	{ __LINE__, SUBNORM_P, MIN_M, SUBNORM_P, FLAG_MINVAL },
	{ __LINE__, SUBNORM_P, SUBNORM_P, ZERO_P, 0 },
	{ __LINE__, SUBNORM_P, SUBNORM_M, ZERO_P, 0 },
	{ __LINE__, SUBNORM_M, MAX_P, SUBNORM_M, 0 },
	{ __LINE__, SUBNORM_M, MAX_M, SUBNORM_M, 0 },
	{ __LINE__, SUBNORM_M, MIN_P, SUBNORM_M, FLAG_MINVAL },
	{ __LINE__, SUBNORM_M, MIN_M, SUBNORM_M, FLAG_MINVAL },
	{ __LINE__, SUBNORM_M, SUBNORM_P, ZERO_M, FLAG_SUBNORM },
	{ __LINE__, SUBNORM_M, SUBNORM_M, ZERO_M, FLAG_SUBNORM },
	
	{ __LINE__, HEXCONSTE("1.70141183460469231732e+38", 0x407e, 0x80000000L, 0x00000000L), HEXCONSTE("4.20389539297445121277e-45", 0x3f6b, 0xc0000000L, 0x00000000L), HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("1.70141183460469231732e+38", 0x407e, 0x80000000L, 0x00000000L), HEXCONSTE("-4.20389539297445121277e-45", 0xbf6b, 0xc0000000L, 0x00000000L), HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("1.70141183460469231732e+38", 0x407e, 0x80000000L, 0x00000000L), HEXCONSTE("8.40779078594890242554e-45", 0x3f6c, 0xc0000000L, 0x00000000L), HEXCONSTE("5.60519385729926828369e-45", 0x3f6c, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("1.70141183460469231732e+38", 0x407e, 0x80000000L, 0x00000000L), HEXCONSTE("-8.40779078594890242554e-45", 0xbf6c, 0xc0000000L, 0x00000000L), HEXCONSTE("5.60519385729926828369e-45", 0x3f6c, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("1.70141183460469231732e+38", 0x407e, 0x80000000L, 0x00000000L), HEXCONSTE("3.52648305246686252391e-38", 0x3f82, 0xc0000000L, 0x00000000L), HEXCONSTE("2.35098870164457501594e-38", 0x3f82, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("1.70141183460469231732e+38", 0x407e, 0x80000000L, 0x00000000L), HEXCONSTE("-3.52648305246686252391e-38", 0xbf82, 0xc0000000L, 0x00000000L), HEXCONSTE("2.35098870164457501594e-38", 0x3f82, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-1.70141183460469231732e+38", 0xc07e, 0x80000000L, 0x00000000L), HEXCONSTE("4.20389539297445121277e-45", 0x3f6b, 0xc0000000L, 0x00000000L), HEXCONSTE("-1.40129846432481707092e-45", 0xbf6a, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-1.70141183460469231732e+38", 0xc07e, 0x80000000L, 0x00000000L), HEXCONSTE("-4.20389539297445121277e-45", 0xbf6b, 0xc0000000L, 0x00000000L), HEXCONSTE("-1.40129846432481707092e-45", 0xbf6a, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-1.70141183460469231732e+38", 0xc07e, 0x80000000L, 0x00000000L), HEXCONSTE("8.40779078594890242554e-45", 0x3f6c, 0xc0000000L, 0x00000000L), HEXCONSTE("-5.60519385729926828369e-45", 0xbf6c, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-1.70141183460469231732e+38", 0xc07e, 0x80000000L, 0x00000000L), HEXCONSTE("-8.40779078594890242554e-45", 0xbf6c, 0xc0000000L, 0x00000000L), HEXCONSTE("-5.60519385729926828369e-45", 0xbf6c, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-1.70141183460469231732e+38", 0xc07e, 0x80000000L, 0x00000000L), HEXCONSTE("3.52648305246686252391e-38", 0x3f82, 0xc0000000L, 0x00000000L), HEXCONSTE("-2.35098870164457501594e-38", 0xbf82, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-1.70141183460469231732e+38", 0xc07e, 0x80000000L, 0x00000000L), HEXCONSTE("-3.52648305246686252391e-38", 0xbf82, 0xc0000000L, 0x00000000L), HEXCONSTE("-2.35098870164457501594e-38", 0xbf82, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("8.98846567431157953865e+307", 0x43fe, 0x80000000L, 0x00000000L), HEXCONSTE("1.48219693752373963253e-323", 0x3bce, 0xc0000000L, 0x00000000L), HEXCONSTE("9.88131291682493088353e-324", 0x3bce, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("8.98846567431157953865e+307", 0x43fe, 0x80000000L, 0x00000000L), HEXCONSTE("-1.48219693752373963253e-323", 0xbbce, 0xc0000000L, 0x00000000L), HEXCONSTE("9.88131291682493088353e-324", 0x3bce, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("8.98846567431157953865e+307", 0x43fe, 0x80000000L, 0x00000000L), HEXCONSTE("2.96439387504747926506e-323", 0x3bcf, 0xc0000000L, 0x00000000L), HEXCONSTE("9.88131291682493088353e-324", 0x3bce, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("8.98846567431157953865e+307", 0x43fe, 0x80000000L, 0x00000000L), HEXCONSTE("-2.96439387504747926506e-323", 0xbbcf, 0xc0000000L, 0x00000000L), HEXCONSTE("9.88131291682493088353e-324", 0x3bce, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("8.98846567431157953865e+307", 0x43fe, 0x80000000L, 0x00000000L), HEXCONSTE("6.67522157552160414927e-308", 0x3c02, 0xc0000000L, 0x00000000L), HEXCONSTE("4.45014771701440276618e-308", 0x3c02, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("8.98846567431157953865e+307", 0x43fe, 0x80000000L, 0x00000000L), HEXCONSTE("-6.67522157552160414927e-308", 0xbc02, 0xc0000000L, 0x00000000L), HEXCONSTE("4.45014771701440276618e-308", 0x3c02, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-8.98846567431157953865e+307", 0xc3fe, 0x80000000L, 0x00000000L), HEXCONSTE("1.48219693752373963253e-323", 0x3bce, 0xc0000000L, 0x00000000L), HEXCONSTE("-9.88131291682493088353e-324", 0xbbce, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-8.98846567431157953865e+307", 0xc3fe, 0x80000000L, 0x00000000L), HEXCONSTE("-1.48219693752373963253e-323", 0xbbce, 0xc0000000L, 0x00000000L), HEXCONSTE("-9.88131291682493088353e-324", 0xbbce, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-8.98846567431157953865e+307", 0xc3fe, 0x80000000L, 0x00000000L), HEXCONSTE("2.96439387504747926506e-323", 0x3bcf, 0xc0000000L, 0x00000000L), HEXCONSTE("-9.88131291682493088353e-324", 0xbbce, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-8.98846567431157953865e+307", 0xc3fe, 0x80000000L, 0x00000000L), HEXCONSTE("-2.96439387504747926506e-323", 0xbbcf, 0xc0000000L, 0x00000000L), HEXCONSTE("-9.88131291682493088353e-324", 0xbbce, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-8.98846567431157953865e+307", 0xc3fe, 0x80000000L, 0x00000000L), HEXCONSTE("6.67522157552160414927e-308", 0x3c02, 0xc0000000L, 0x00000000L), HEXCONSTE("-4.45014771701440276618e-308", 0xbc02, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-8.98846567431157953865e+307", 0xc3fe, 0x80000000L, 0x00000000L), HEXCONSTE("-6.67522157552160414927e-308", 0xbc02, 0xc0000000L, 0x00000000L), HEXCONSTE("-4.45014771701440276618e-308", 0xbc02, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
#if __LDBL_MIN_EXP__ <= (-16382)
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("1.09355985956474238076e-4950", 0x0000, 0x00000000L, 0x00000003L), HEXCONSTE("3.64519953188247460253e-4951", 0x0000, 0x00000000L, 0x00000002L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("-1.09355985956474238076e-4950", 0x8000, 0x00000000L, 0x00000003L), HEXCONSTE("3.64519953188247460253e-4951", 0x0000, 0x00000000L, 0x00000002L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("2.18711971912948476152e-4950", 0x0000, 0x00000000L, 0x00000006L), HEXCONSTE("1.45807981275298984101e-4950", 0x0000, 0x00000000L, 0x00000002L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("-2.18711971912948476152e-4950", 0x8000, 0x00000000L, 0x00000006L), HEXCONSTE("1.45807981275298984101e-4950", 0x0000, 0x00000000L, 0x00000002L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
#else
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("1.09355985956474238076e-4950", 0x0000, 0x00000000L, 0x00000003L), HEXCONSTE("3.64519953188247460253e-4951", 0x0000, 0x00000000L, 0x00000001L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("-1.09355985956474238076e-4950", 0x8000, 0x00000000L, 0x00000003L), HEXCONSTE("3.64519953188247460253e-4951", 0x0000, 0x00000000L, 0x00000001L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("2.18711971912948476152e-4950", 0x0000, 0x00000000L, 0x00000006L), HEXCONSTE("1.45807981275298984101e-4950", 0x0000, 0x00000000L, 0x00000004L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("-2.18711971912948476152e-4950", 0x8000, 0x00000000L, 0x00000006L), HEXCONSTE("1.45807981275298984101e-4950", 0x0000, 0x00000000L, 0x00000004L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
#endif
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("1.00863094293362805188e-4931", 0x0002, 0xc0000000L, 0x00000000L), HEXCONSTE("6.72420628622418701253e-4932", 0x0002, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("-1.00863094293362805188e-4931", 0x8002, 0xc0000000L, 0x00000000L), HEXCONSTE("6.72420628622418701253e-4932", 0x0002, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
#if __LDBL_MIN_EXP__ <= (-16382)
	{ __LINE__, HEXCONSTE("-5.94865747678615882543e+4931", 0xfffe, 0x80000000L, 0x00000000L), HEXCONSTE("1.09355985956474238076e-4950", 0x0000, 0x00000000L, 0x00000003L), HEXCONSTE("-3.64519953188247460253e-4951", 0x8000, 0x00000000L, 0x00000002L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-5.94865747678615882543e+4931", 0xfffe, 0x80000000L, 0x00000000L), HEXCONSTE("-1.09355985956474238076e-4950", 0x8000, 0x00000000L, 0x00000003L), HEXCONSTE("-3.64519953188247460253e-4951", 0x8000, 0x00000000L, 0x00000002L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-5.94865747678615882543e+4931", 0xfffe, 0x80000000L, 0x00000000L), HEXCONSTE("2.18711971912948476152e-4950", 0x0000, 0x00000000L, 0x00000006L), HEXCONSTE("-1.45807981275298984101e-4950", 0x8000, 0x00000000L, 0x00000002L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-5.94865747678615882543e+4931", 0xfffe, 0x80000000L, 0x00000000L), HEXCONSTE("-2.18711971912948476152e-4950", 0x8000, 0x00000000L, 0x00000006L), HEXCONSTE("-1.45807981275298984101e-4950", 0x8000, 0x00000000L, 0x00000002L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
#else
	{ __LINE__, HEXCONSTE("-5.94865747678615882543e+4931", 0xfffe, 0x80000000L, 0x00000000L), HEXCONSTE("1.09355985956474238076e-4950", 0x0000, 0x00000000L, 0x00000003L), HEXCONSTE("-3.64519953188247460253e-4951", 0x8000, 0x00000000L, 0x00000001L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-5.94865747678615882543e+4931", 0xfffe, 0x80000000L, 0x00000000L), HEXCONSTE("-1.09355985956474238076e-4950", 0x8000, 0x00000000L, 0x00000003L), HEXCONSTE("-3.64519953188247460253e-4951", 0x8000, 0x00000000L, 0x00000001L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-5.94865747678615882543e+4931", 0xfffe, 0x80000000L, 0x00000000L), HEXCONSTE("2.18711971912948476152e-4950", 0x0000, 0x00000000L, 0x00000006L), HEXCONSTE("-1.45807981275298984101e-4950", 0x8000, 0x00000000L, 0x00000004L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-5.94865747678615882543e+4931", 0xfffe, 0x80000000L, 0x00000000L), HEXCONSTE("-2.18711971912948476152e-4950", 0x8000, 0x00000000L, 0x00000006L), HEXCONSTE("-1.45807981275298984101e-4950", 0x8000, 0x00000000L, 0x00000004L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
#endif
	{ __LINE__, HEXCONSTE("-5.94865747678615882543e+4931", 0xfffe, 0x80000000L, 0x00000000L), HEXCONSTE("1.00863094293362805188e-4931", 0x0002, 0xc0000000L, 0x00000000L), HEXCONSTE("-6.72420628622418701253e-4932", 0x8002, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	{ __LINE__, HEXCONSTE("-5.94865747678615882543e+4931", 0xfffe, 0x80000000L, 0x00000000L), HEXCONSTE("-1.00863094293362805188e-4931", 0x8002, 0xc0000000L, 0x00000000L), HEXCONSTE("-6.72420628622418701253e-4932", 0x8002, 0x80000000L, 0x00000000L), FLAG_FAIL_ARANYM|FLAG_FAIL_STONX },
	
	{ __LINE__, HEXCONSTE("1.17549421069244107549e-38", 0x3f80, 0xfffffe00L, 0x00000000L), HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), ZERO_P, 0 },
	{ __LINE__, HEXCONSTE("2.22507385850720088902e-308", 0x3c00, 0xffffffffL, 0xfffff000L), HEXCONSTE("4.94065645841246544177e-324", 0x3bcd, 0x80000000L, 0x00000000L), ZERO_P, 0 },
	{ __LINE__, HEXCONSTE("3.36210314311209350590e-4932", 0x0000, 0x7fffffffL, 0xffffffffL), HEXCONSTE("3.64519953188247460253e-4951", 0x0000, 0x00000000L, 0x00000001L), ZERO_P, 0 },
	{ __LINE__, HEXCONSTE("-1.00000000000000005551e+00", 0xbfff, 0x80000000L, 0x00000200L), HEXCONSTE("9.99999999999999944489e-01", 0x3ffe, 0xffffffffL, 0xfffffc00L), HEXCONSTE("-1.11022302462515654042e-16", 0xbfca, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("9.99999999999999958367e-01", 0x3ffe, 0xffffffffL, 0xfffffd00L), HEXCONSTE("9.99999999999999944489e-01", 0x3ffe, 0xffffffffL, 0xfffffc00L), HEXCONSTE("1.38777878078144567553e-17", 0x3fc7, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-9.99999999999999958367e-01", 0xbffe, 0xffffffffL, 0xfffffd00L), HEXCONSTE("9.99999999999999944489e-01", 0x3ffe, 0xffffffffL, 0xfffffc00L), HEXCONSTE("-1.38777878078144567553e-17", 0xbfc7, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("9.99999999999999958367e-01", 0x3ffe, 0xffffffffL, 0xfffffd00L), HEXCONSTE("-9.99999999999999944489e-01", 0xbffe, 0xffffffffL, 0xfffffc00L), HEXCONSTE("1.38777878078144567553e-17", 0x3fc7, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-9.99999999999999958367e-01", 0xbffe, 0xffffffffL, 0xfffffd00L), HEXCONSTE("-9.99999999999999944489e-01", 0xbffe, 0xffffffffL, 0xfffffc00L), HEXCONSTE("-1.38777878078144567553e-17", 0xbfc7, 0x80000000L, 0x00000000L), 0 },

	{ __LINE__, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), HEXCONSTE("3.0", 0x4000, 0xc0000000L, 0x00000000L), HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("3.0", 0x3fff, 0x80000000L, 0x00000000L), HEXCONSTE("1.0", 0x4000, 0xc0000000L, 0x00000000L), HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("2.0", 0x4000, 0x80000000L, 0x00000000L), HEXCONSTE("3.0", 0x4000, 0xc0000000L, 0x00000000L), HEXCONSTE("2.0", 0x4000, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("3.0", 0x4000, 0xc0000000L, 0x00000000L), HEXCONSTE("2.0", 0x4000, 0x80000000L, 0x00000000L), HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), 0 },
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
	status |= test_table_f_ff_op(fmod_data, ARRAY_SIZE(fmod_data), __FILE__);
#endif
#ifdef TEST_FUNC_F_FF
	status |= test_table_f_ff_func(fmod_data, ARRAY_SIZE(fmod_data), __FILE__);
#endif
	
	return status ? EXIT_FAILURE : EXIT_SUCCESS;
}
