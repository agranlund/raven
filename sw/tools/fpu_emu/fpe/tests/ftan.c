/*
 * Test:USES_FPU
 */
#include "testconfig.h"
#define TEST_OP_F_F "ftan"
#define TEST_FUNC_F_F fpu_tan
#include "testdriver.h"



static test_f_f_data const ftan_data[] = {
	{ __LINE__, ZERO_P, ZERO_P, 0 },
	{ __LINE__, ZERO_M, ZERO_M, 0 },
	{ __LINE__, INF_P, QNAN_P, 0 },
	{ __LINE__, INF_M, QNAN_P, 0 },
	{ __LINE__, QNAN_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, QNAN_P, 0 },
	{ __LINE__, SNAN_P, QNAN_P, 0 },
	{ __LINE__, SNAN_M, QNAN_P, 0 },
	{ __LINE__, SUBNORM_M, SUBNORM_M, 0 },
	{ __LINE__, SUBNORM_P, SUBNORM_P, 0 },

	{ __LINE__, HEXCONSTE("7.85398185253143310547e-01", 0x3ffe, 0xc90fdb00L, 0x00000000L), HEXCONSTE("1.00000004371139095724e+00", 0x3fff, 0x8000005dL, 0xde976037L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85398125648498535156e-01", 0x3ffe, 0xc90fda00L, 0x00000000L), HEXCONSTE("9.99999924502103301044e-01", 0x3ffe, 0xfffffebbL, 0xbd2f48f3L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85398163397448390022e-01", 0x3ffe, 0xc90fdaa2L, 0x2168c800L), HEXCONSTE("1.00000000000000016079e+00", 0x3fff, 0x80000000L, 0x000005cbL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85398163397448278999e-01", 0x3ffe, 0xc90fdaa2L, 0x2168c000L), HEXCONSTE("9.99999999999999938743e-01", 0x3ffe, 0xffffffffL, 0xfffffb96L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85398163397448309628e-01", 0x3ffe, 0xc90fdaa2L, 0x2168c235L), HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("7.85398163397448309574e-01", 0x3ffe, 0xc90fdaa2L, 0x2168c234L), HEXCONSTE("9.99999999999999999892e-01", 0x3ffe, 0xffffffffL, 0xfffffffeL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("1.57079637050628662109e+00", 0x3fff, 0xc90fdb00L, 0x00000000L), HEXCONSTE("-2.28773324288564598737e+07", 0xc017, 0xae8a4a36L, 0xe4bbda07L), FLAG_INEXACT4 }, /* c017:ae8a4a36:e4c4baeb */
	{ __LINE__, HEXCONSTE("1.57079625129699707031e+00", 0x3fff, 0xc90fda00L, 0x00000000L), HEXCONSTE("1.32454016068625678863e+07", 0x4016, 0xca1bd99bL, 0x5b5e560aL), FLAG_INEXACT4 }, /* 4016:ca1bd99b:5b58623d */
	{ __LINE__, HEXCONSTE("1.57079632679489678004e+00", 0x3fff, 0xc90fdaa2L, 0x2168c800L), HEXCONSTE("-6.21843116382373801758e+15", 0xc033, 0xb0bc78fdL, 0x05d3363dL), FLAG_INEXACT4 }, /* c033:b0bd0aa4:a3b3d024 */
	{ __LINE__, HEXCONSTE("1.57079632679489655800e+00", 0x3fff, 0xc90fdaa2L, 0x2168c000L), HEXCONSTE("1.63312393531953697559e+16", 0x4034, 0xe816aa36L, 0x9f558f5bL), FLAG_INEXACT4 }, /* 4034:e814b3e1:8e6da706 */
	{ __LINE__, HEXCONSTE("1.57079632679489661926e+00", 0x3fff, 0xc90fdaa2L, 0x2168c235L), HEXCONSTE("-3.98679762981171070680e+19", 0xc040, 0x80000000L, 0x00000000L), FLAG_INEXACT4 }, /* c040:8a51e04d:aabda35f */
	{ __LINE__, HEXCONSTE("1.57079632679489661915e+00", 0x3fff, 0xc90fdaa2L, 0x2168c234L), HEXCONSTE("1.19994102263174429330e+19", 0x403e, 0xaaaaaaaaL, 0xaaaaaaabL), FLAG_INEXACT4 }, /* 403e:a6867806:75d73f75 */
	{ __LINE__, HEXCONSTE("-1.57079637050628662109e+00", 0xbfff, 0xc90fdb00L, 0x00000000L), HEXCONSTE("2.28773324288564598737e+07", 0x4017, 0xae8a4a36L, 0xe4bbda07L), FLAG_INEXACT4 }, /* 4017:ae8a4a36:e4c4baeb */
	{ __LINE__, HEXCONSTE("-1.57079625129699707031e+00", 0xbfff, 0xc90fda00L, 0x00000000L), HEXCONSTE("-1.32454016068625678863e+07", 0xc016, 0xca1bd99bL, 0x5b58623dL), FLAG_INEXACT4 }, /* c016:ca1bd99b:5b58623d */
	{ __LINE__, HEXCONSTE("-1.57079632679489678004e+00", 0xbfff, 0xc90fdaa2L, 0x2168c800L), HEXCONSTE("6.21843116382373801758e+15", 0x4033, 0xb0bc78fdL, 0x05d3363dL), FLAG_INEXACT4 }, /* 4033:b0bd0aa4:a3b3d024 */
	{ __LINE__, HEXCONSTE("-1.57079632679489655800e+00", 0xbfff, 0xc90fdaa2L, 0x2168c000L), HEXCONSTE("-1.63312393531953697559e+16", 0xc034, 0xe816aa36L, 0x9f558f5bL), FLAG_INEXACT4 }, /* c034:e814b3e1:8e6da706 */
	{ __LINE__, HEXCONSTE("-1.57079632679489661926e+00", 0xbfff, 0xc90fdaa2L, 0x2168c235L), HEXCONSTE("3.98679762981171070680e+19", 0x4040, 0x80000000L, 0x00000000L), FLAG_INEXACT4 }, /* 4040:8a51e04d:aabda35f */
	{ __LINE__, HEXCONSTE("-1.57079632679489661915e+00", 0xbfff, 0xc90fdaa2L, 0x2168c234L), HEXCONSTE("-1.19994102263174429330e+19", 0xc03e, 0xaaaaaaaaL, 0xaaaaaaabL), FLAG_INEXACT4 }, /* c03e:a6867806:75d73f75 */
	{ __LINE__, HEXCONSTE("7.50000000000000000000e-01", 0x3ffe, 0xc0000000L, 0x00000000L), HEXCONSTE("9.31596459944072461160e-01", 0x3ffe, 0xee7d1b08L, 0x87775f05L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("3.68934881474191032320e+19", 0x4040, 0x80000000L, 0x00000000L), HEXCONSTE("-4.72364872359047946782e-02", 0x3ff3, 0x9b17d8d7L, 0xc01ece1bL), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* bffa:c17b0bfd:b2b8061e */
	{ __LINE__, HEXCONSTE("-3.68934881474191032320e+19", 0xc040, 0x80000000L, 0x00000000L), HEXCONSTE("4.72364872359047946782e-02", 0xbff3, 0x9b17d8d7L, 0xc01ece1bL), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 3ffa:c17b0bfd:b2b8061e */
	{ __LINE__, HEXCONSTE("7.45058059692382812500e-09", 0x3fe4, 0x80000000L, 0x00000000L), HEXCONSTE("7.45058059692382826313e-09", 0x3fe4, 0x80000000L, 0x000000abL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("-7.45058059692382812500e-09", 0xbfe4, 0x80000000L, 0x00000000L), HEXCONSTE("-7.45058059692382826313e-09", 0xbfe4, 0x80000000L, 0x000000abL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("7.85156250000000000000e-01", 0x3ffe, 0xc9000000L, 0x00000000L), HEXCONSTE("9.99516290211545781826e-01", 0x3ffe, 0xffe04cb2L, 0x472021f1L), FLAG_INEXACT3 }, /* 3ffe:ffe04cb2:472021f2 */
	{ __LINE__, HEXCONSTE("7.85278320312500000000e-01", 0x3ffe, 0xc9080000L, 0x00000000L), HEXCONSTE("9.99760342550244141089e-01", 0x3ffe, 0xfff04b37L, 0x174f6f34L), FLAG_INEXACT3 }, /* 3ffe:fff04b37:174f6f35 */
	{ __LINE__, HEXCONSTE("7.85339355468750000000e-01", 0x3ffe, 0xc90c0000L, 0x00000000L), HEXCONSTE("9.99882391058806030285e-01", 0x3ffe, 0xfff84ad9L, 0x71a07663L), FLAG_INEXACT3 }, /* 3ffe:fff84ad9:71a07664 */
	{ __LINE__, HEXCONSTE("7.85369873046875000000e-01", 0x3ffe, 0xc90e0000L, 0x00000000L), HEXCONSTE("9.99943420899480875331e-01", 0x3ffe, 0xfffc4ac2L, 0x9d1711ccL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85385131835937500000e-01", 0x3ffe, 0xc90f0000L, 0x00000000L), HEXCONSTE("9.99973937216615670228e-01", 0x3ffe, 0xfffe4abdL, 0x329de183L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85392761230468750000e-01", 0x3ffe, 0xc90f8000L, 0x00000000L), HEXCONSTE("9.99989195724407276499e-01", 0x3ffe, 0xffff4abbL, 0xfd5b29a3L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85396575927734375000e-01", 0x3ffe, 0xc90fc000L, 0x00000000L), HEXCONSTE("9.99996825065612240295e-01", 0x3ffe, 0xffffcabbL, 0xc2b925c1L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85397529602050781250e-01", 0x3ffe, 0xc90fd000L, 0x00000000L), HEXCONSTE("9.99998732410008335807e-01", 0x3ffe, 0xffffeabbL, 0xbe109e1fL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85398006439208984375e-01", 0x3ffe, 0xc90fd800L, 0x00000000L), HEXCONSTE("9.99999686083570621278e-01", 0x3ffe, 0xfffffabbL, 0xbd3c59feL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85398125648498535156e-01", 0x3ffe, 0xc90fda00L, 0x00000000L), HEXCONSTE("9.99999924502103301044e-01", 0x3ffe, 0xfffffebbL, 0xbd2f48f3L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.89062500000000000000e-01", 0x3ffe, 0xca000000L, 0x00000000L), HEXCONSTE("1.00735565974072721650e+00", 0x3fff, 0x80f107bfL, 0x03725a03L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.87109375000000000000e-01", 0x3ffe, 0xc9800000L, 0x00000000L), HEXCONSTE("1.00342829308630446543e+00", 0x3fff, 0x8070569bL, 0x57e86d1bL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("7.86132812500000000000e-01", 0x3ffe, 0xc9400000L, 0x00000000L), HEXCONSTE("1.00147037868200822371e+00", 0x3fff, 0x80302e6eL, 0x2d0d0e0fL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("7.85644531250000000000e-01", 0x3ffe, 0xc9200000L, 0x00000000L), HEXCONSTE("1.00049285713923005714e+00", 0x3fff, 0x80102662L, 0xa5b53afeL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("7.85400390625000000000e-01", 0x3ffe, 0xc9100000L, 0x00000000L), HEXCONSTE("1.00000445446502449535e+00", 0x3fff, 0x8000255dL, 0xe40b839eL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("7.85398483276367187500e-01", 0x3ffe, 0xc90fe000L, 0x00000000L), HEXCONSTE("1.00000063975804240086e+00", 0x3fff, 0x8000055dL, 0xdeb40ae9L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("7.85398244857788085938e-01", 0x3ffe, 0xc90fdc00L, 0x00000000L), HEXCONSTE("1.00000016292069282423e+00", 0x3fff, 0x8000015dL, 0xde991bf4L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.85398185253143310547e-01", 0x3ffe, 0xc90fdb00L, 0x00000000L), HEXCONSTE("1.00000004371139095724e+00", 0x3fff, 0x8000005dL, 0xde976037L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85156250000000000000e-01", 0xbffe, 0xc9000000L, 0x00000000L), HEXCONSTE("-9.99516290211545781826e-01", 0xbffe, 0xffe04cb2L, 0x472021f2L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85278320312500000000e-01", 0xbffe, 0xc9080000L, 0x00000000L), HEXCONSTE("-9.99760342550244141089e-01", 0xbffe, 0xfff04b37L, 0x174f6f35L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85339355468750000000e-01", 0xbffe, 0xc90c0000L, 0x00000000L), HEXCONSTE("-9.99882391058806030285e-01", 0xbffe, 0xfff84ad9L, 0x71a07664L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85369873046875000000e-01", 0xbffe, 0xc90e0000L, 0x00000000L), HEXCONSTE("-9.99943420899480875331e-01", 0xbffe, 0xfffc4ac2L, 0x9d1711ccL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85385131835937500000e-01", 0xbffe, 0xc90f0000L, 0x00000000L), HEXCONSTE("-9.99973937216615670228e-01", 0xbffe, 0xfffe4abdL, 0x329de183L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85392761230468750000e-01", 0xbffe, 0xc90f8000L, 0x00000000L), HEXCONSTE("-9.99989195724407276499e-01", 0xbffe, 0xffff4abbL, 0xfd5b29a3L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85396575927734375000e-01", 0xbffe, 0xc90fc000L, 0x00000000L), HEXCONSTE("-9.99996825065612240295e-01", 0xbffe, 0xffffcabbL, 0xc2b925c1L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85397529602050781250e-01", 0xbffe, 0xc90fd000L, 0x00000000L), HEXCONSTE("-9.99998732410008335807e-01", 0xbffe, 0xffffeabbL, 0xbe109e1fL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85398006439208984375e-01", 0xbffe, 0xc90fd800L, 0x00000000L), HEXCONSTE("-9.99999686083570621278e-01", 0xbffe, 0xfffffabbL, 0xbd3c59feL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85398125648498535156e-01", 0xbffe, 0xc90fda00L, 0x00000000L), HEXCONSTE("-9.99999924502103301044e-01", 0xbffe, 0xfffffebbL, 0xbd2f48f3L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.89062500000000000000e-01", 0xbffe, 0xca000000L, 0x00000000L), HEXCONSTE("-1.00735565974072721650e+00", 0xbfff, 0x80f107bfL, 0x03725a03L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.87109375000000000000e-01", 0xbffe, 0xc9800000L, 0x00000000L), HEXCONSTE("-1.00342829308630446543e+00", 0xbfff, 0x8070569bL, 0x57e86d1bL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.86132812500000000000e-01", 0xbffe, 0xc9400000L, 0x00000000L), HEXCONSTE("-1.00147037868200822371e+00", 0xbfff, 0x80302e6eL, 0x2d0d0e0fL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85644531250000000000e-01", 0xbffe, 0xc9200000L, 0x00000000L), HEXCONSTE("-1.00049285713923005714e+00", 0xbfff, 0x80102662L, 0xa5b53afeL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85400390625000000000e-01", 0xbffe, 0xc9100000L, 0x00000000L), HEXCONSTE("-1.00000445446502449535e+00", 0xbfff, 0x8000255dL, 0xe40b839eL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85398483276367187500e-01", 0xbffe, 0xc90fe000L, 0x00000000L), HEXCONSTE("-1.00000063975804240086e+00", 0xbfff, 0x8000055dL, 0xdeb40ae9L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85398244857788085938e-01", 0xbffe, 0xc90fdc00L, 0x00000000L), HEXCONSTE("-1.00000016292069282423e+00", 0xbfff, 0x8000015dL, 0xde991bf4L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-7.85398185253143310547e-01", 0xbffe, 0xc90fdb00L, 0x00000000L), HEXCONSTE("-1.00000004371139095724e+00", 0xbfff, 0x8000005dL, 0xde976037L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("1.00000009040962152038e+22", 0x4048, 0x87867900L, 0x00000000L), HEXCONSTE("-3.14680716922883491765e-01", 0x3ff6, 0xbe838f2fL, 0x91a2bb15L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* bffd:a11dd4b7:bb32541a */
	{ __LINE__, HEXCONSTE("9.99999977819630836122e+21", 0x4048, 0x87867800L, 0x00000000L), HEXCONSTE("-1.08102390048654640640e+00", 0xbffe, 0x9166ad50L, 0xc4461ff7L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* bfff:8a5efdbd:645c945d */
	{ __LINE__, HEXCONSTE("1.00000000000000000000e+22", 0x4048, 0x87867832L, 0x6eac9000L), HEXCONSTE("-1.62877822560689887856e+00", 0xbffe, 0xdd1c4380L, 0xfe2e929eL), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* bfff:d07bce0d:b592bba5 */
	{ __LINE__, HEXCONSTE("3.40282346638528859812e+38", 0x407e, 0xffffff00L, 0x00000000L), HEXCONSTE("-6.11797949834248061182e-01", 0xc000, 0xc8c34476L, 0x95a35cddL), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* bffe:9c9eca5a:4c460f93 */
	{ __LINE__, HEXCONSTE("8.98846567431157953865e+307", 0x43fe, 0x80000000L, 0x00000000L), HEXCONSTE("-6.81447647606621501287e-01", 0xc000, 0x9a0006baL, 0x40ac7102L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* bffe:ae735a60:68151a9e */
	{ __LINE__, HEXCONSTE("3.40282346638528859812e+38", 0x407e, 0xffffff00L, 0x00000000L), HEXCONSTE("-6.11797949834248061182e-01", 0xc000, 0xc8c34476L, 0x95a35cddL), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* bffe:9c9eca5a:4c460f93 */
	{ __LINE__, HEXCONSTE("1.79769313486231570815e+308", 0x43fe, 0xffffffffL, 0xfffff800L), HEXCONSTE("-4.96201587444489490058e-03", 0x3fff, 0xa820a48bL, 0xbe4ad518L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* bff7:a29867f3:94a41dc7 */
	{ __LINE__, HEXCONSTE("5.94865747678615882543e+4931", 0x7ffe, 0x80000000L, 0x00000000L), HEXCONSTE("4.22722393732022337789e-01", 0x4001, 0x9b2555fdL, 0x81e76442L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 3ffd:d86f11d0:bb537c9f */
	{ __LINE__, HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), HEXCONSTE("1.55740772465490223046e+00", 0x3fff, 0xc75922e5L, 0xf71d2dc5L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("2.00000000000000000000e+00", 0x4000, 0x80000000L, 0x00000000L), HEXCONSTE("-2.18503986326151899175e+00", 0xc000, 0x8bd7b170L, 0x4a87c1dbL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("3.00000000000000000000e+00", 0x4000, 0xc0000000L, 0x00000000L), HEXCONSTE("-1.42546543074277805291e-01", 0xbffc, 0x91f7b892L, 0xa5c37866L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("4.00000000000000000000e+00", 0x4001, 0x80000000L, 0x00000000L), HEXCONSTE("1.15782128234957758315e+00", 0x3fff, 0x94337cdfL, 0x26f09b87L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("5.00000000000000000000e+00", 0x4001, 0xa0000000L, 0x00000000L), HEXCONSTE("-3.38051500624658563692e+00", 0xc000, 0xd85a5b9cL, 0xddd83421L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("6.00000000000000000000e+00", 0x4001, 0xc0000000L, 0x00000000L), HEXCONSTE("-2.91006191384749157063e-01", 0xbffd, 0x94fec375L, 0xdcadf182L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("7.00000000000000000000e+00", 0x4001, 0xe0000000L, 0x00000000L), HEXCONSTE("8.71447982724318736451e-01", 0x3ffe, 0xdf173709L, 0xf753c4c1L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("8.00000000000000000000e+00", 0x4002, 0x80000000L, 0x00000000L), HEXCONSTE("-6.79971145522037869998e+00", 0xc001, 0xd9973c7aL, 0x4d0f1d39L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("9.00000000000000000000e+00", 0x4002, 0x90000000L, 0x00000000L), HEXCONSTE("-4.52315659441809840596e-01", 0xbffd, 0xe795eb09L, 0x8ae0df01L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("1.00000000000000000000e+01", 0x4002, 0xa0000000L, 0x00000000L), HEXCONSTE("6.48360827459086671264e-01", 0x3ffe, 0xa5faf9a5L, 0xf1bc12f0L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-1.02408266067504882812e+00", 0xbfff, 0x83152400L, 0x00000000L), HEXCONSTE("-1.64313524714176713022e+00", 0xbfff, 0xd252417aL, 0xb07b7267L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-1.31020736694335937500e+00", 0xbfff, 0xa7b4e000L, 0x00000000L), HEXCONSTE("-3.75020232812174444817e+00", 0xc000, 0xf00350a0L, 0x2a9efb7aL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("1.42443704605102539062e+00", 0x3fff, 0xb653f400L, 0x00000000L), HEXCONSTE("6.78364522457327409144e+00", 0x4001, 0xd9139f26L, 0x66ae63b5L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-1.70864486694335937500e+00", 0xbfff, 0xdab4e000L, 0x00000000L), HEXCONSTE("7.20833087491041078158e+00", 0x4001, 0xe6aaa582L, 0xcf97ced2L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-2.07316565513610839844e+00", 0xc000, 0x84aebf00L, 0x00000000L), HEXCONSTE("1.82022401449273296551e+00", 0x3fff, 0xe8fd19baL, 0xd1ef63f4L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("2.03560495376586914062e+00", 0x4000, 0x82475a00L, 0x00000000L), HEXCONSTE("-1.99420835595249289825e+00", 0xbfff, 0xff42382bL, 0x1ce9c3b9L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-5.18815183639526367188e+00", 0xc001, 0xa6055700L, 0x00000000L), HEXCONSTE("1.94085400773404880253e+00", 0x3fff, 0xf86de774L, 0xc39e5f97L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-5.18815231323242187500e+00", 0xc001, 0xa6055800L, 0x00000000L), HEXCONSTE("1.94085173469429407838e+00", 0x3fff, 0xf86dd463L, 0x72cc5c99L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-5.18815193733241741825e+00", 0xc001, 0xa6055736L, 0x30b26000L), HEXCONSTE("1.94085352657538355440e+00", 0x3fff, 0xf86de36bL, 0x7bd85373L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-5.18815193733241830643e+00", 0xc001, 0xa6055736L, 0x30b26800L), HEXCONSTE("1.94085352657537932048e+00", 0x3fff, 0xf86de36bL, 0x7bd7bae8L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-5.18815193733241794907e+00", 0xc001, 0xa6055736L, 0x30b264c8L), HEXCONSTE("1.94085352657538102398e+00", 0x3fff, 0xf86de36bL, 0x7bd7f848L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("1.10478901863098144531e+00", 0x3fff, 0x8d69ba00L, 0x00000000L), HEXCONSTE("1.98825690274209654069e+00", 0x3fff, 0xfe7f33c2L, 0xa96a53e6L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-1.05208921432495117188e+00", 0xbfff, 0x86aadc00L, 0x00000000L), HEXCONSTE("-1.75178481602246248624e+00", 0xbfff, 0xe03a7c1fL, 0x3911b78dL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("1.57079708576202392578e+00", 0x3fff, 0xc90fe100L, 0x00000000L), HEXCONSTE("-1.31758012174867739395e+06", 0xc013, 0xa0d660f9L, 0x575e6548L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* c013:a0d660f9:575edde */
	{ __LINE__, HEXCONSTE("1.57079696655273437500e+00", 0x3fff, 0xc90fe000L, 0x00000000L), HEXCONSTE("-1.56309144020462858066e+06", 0xc013, 0xbece9b85L, 0x8a0070a5L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* c013:bece9b85:8a011a6 */
	{ __LINE__, HEXCONSTE("1.57079697000000018114e+00", 0x3fff, 0xc90fe007L, 0x67278800L), HEXCONSTE("-1.55471403205930036495e+06", 0xc013, 0xbdc8d041L, 0xa84dccd8L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* c013:bdc8d041:a84e74c */
	{ __LINE__, HEXCONSTE("1.57079696999999995910e+00", 0x3fff, 0xc90fe007L, 0x67278000L), HEXCONSTE("-1.55471403259601231139e+06", 0xc013, 0xbdc8d042L, 0xc1b20ee8L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* c013:bdc8d042:c1b2b6dc */
	{ __LINE__, HEXCONSTE("1.57079697000000000008e+00", 0x3fff, 0xc90fe007L, 0x6727817aL), HEXCONSTE("-1.55471403249695121974e+06", 0xc013, 0xbdc8d042L, 0x8dc24db6L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* c013:bdc8d042:8dc2f5ab */
	{ __LINE__, HEXCONSTE("1.57079696999999999997e+00", 0x3fff, 0xc90fe007L, 0x67278179L), HEXCONSTE("-1.55471403249721328609e+06", 0xc013, 0xbdc8d042L, 0x8de57a3eL), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* c013:bdc8d042:8de62233 */
	{ __LINE__, HEXCONSTE("-1.57079696655273437500e+00", 0xbfff, 0xc90fe000L, 0x00000000L), HEXCONSTE("1.56309144020462858066e+06", 0x4013, 0xbece9b85L, 0x8a0070a5L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 4013:bece9b85:8a011a6a */
	{ __LINE__, HEXCONSTE("-1.57079708576202392578e+00", 0xbfff, 0xc90fe100L, 0x00000000L), HEXCONSTE("1.31758012174867739395e+06", 0x4013, 0xa0d660f9L, 0x575e6548L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 4013:a0d660f9:575edde8 */
	{ __LINE__, HEXCONSTE("-1.57079696999999995910e+00", 0xbfff, 0xc90fe007L, 0x67278000L), HEXCONSTE("1.55471403259601231139e+06", 0x4013, 0xbdc8d042L, 0xc1b20ee8L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 4013:bdc8d042:c1b2b6dc */
	{ __LINE__, HEXCONSTE("-1.57079697000000018114e+00", 0xbfff, 0xc90fe007L, 0x67278800L), HEXCONSTE("1.55471403205930036495e+06", 0x4013, 0xbdc8d041L, 0xa84dccd8L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 4013:bdc8d041:a84e74cd */
	{ __LINE__, HEXCONSTE("-1.57079696999999999997e+00", 0xbfff, 0xc90fe007L, 0x67278179L), HEXCONSTE("1.55471403249721328609e+06", 0x4013, 0xbdc8d042L, 0x8de57a3eL), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 4013:bdc8d042:8de62233 */
	{ __LINE__, HEXCONSTE("-1.57079697000000000008e+00", 0xbfff, 0xc90fe007L, 0x6727817aL), HEXCONSTE("1.55471403249695121974e+06", 0x4013, 0xbdc8d042L, 0x8dc24db6L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 4013:bdc8d042:8dc2f5ab */
	{ __LINE__, HEXCONSTE("3.12500000000000000000e-02", 0x3ffa, 0x80000000L, 0x00000000L), HEXCONSTE("3.12601765012559564221e-02", 0x3ffa, 0x800aabbbL, 0xd76042bfL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("9.76562500000000000000e-04", 0x3ff5, 0x80000000L, 0x00000000L), HEXCONSTE("9.76562810440976628977e-04", 0x3ff5, 0x800002aaL, 0xaabbbbbcL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("3.05175781250000000000e-05", 0x3ff0, 0x80000000L, 0x00000000L), HEXCONSTE("3.05175781344739031479e-05", 0x3ff0, 0x80000000L, 0xaaaaaaacL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("9.53674316406250000000e-07", 0x3feb, 0x80000000L, 0x00000000L), HEXCONSTE("9.53674316406539120614e-07", 0x3feb, 0x80000000L, 0x002aaaabL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("2.98023223876953125000e-08", 0x3fe6, 0x80000000L, 0x00000000L), HEXCONSTE("2.98023223876953213243e-08", 0x3fe6, 0x80000000L, 0x00000aabL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("9.31322574615478515625e-10", 0x3fe1, 0x80000000L, 0x00000000L), HEXCONSTE("9.31322574615478515928e-10", 0x3fe1, 0x80000000L, 0x00000003L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("2.91038304567337036133e-11", 0x3fdc, 0x80000000L, 0x00000000L), HEXCONSTE("2.91038304567337036133e-11", 0x3fdc, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("9.09494701772928237915e-13", 0x3fd7, 0x80000000L, 0x00000000L), HEXCONSTE("9.09494701772928237915e-13", 0x3fd7, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("2.84217094304040074348e-14", 0x3fd2, 0x80000000L, 0x00000000L), HEXCONSTE("2.84217094304040074348e-14", 0x3fd2, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("8.88178419700125232339e-16", 0x3fcd, 0x80000000L, 0x00000000L), HEXCONSTE("8.88178419700125232339e-16", 0x3fcd, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("2.77555756156289135106e-17", 0x3fc8, 0x80000000L, 0x00000000L), HEXCONSTE("2.77555756156289135106e-17", 0x3fc8, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("8.67361737988403547206e-19", 0x3fc3, 0x80000000L, 0x00000000L), HEXCONSTE("8.67361737988403547206e-19", 0x3fc3, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("7.88860905221011805412e-31", 0x3f9b, 0x80000000L, 0x00000000L), HEXCONSTE("7.88860905221011805412e-31", 0x3f9b, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("2.40991986510288411774e-181", 0x3da7, 0x80000000L, 0x00000000L), HEXCONSTE("2.40991986510288411774e-181", 0x3da7, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("0.00000000000000000000e+00", 0x0000, 0x00000000L, 0x00000000L), HEXCONSTE("0.00000000000000000000e+00", 0x0000, 0x00000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("4.94065645841246544177e-324", 0x3bcd, 0x80000000L, 0x00000000L), HEXCONSTE("4.94065645841246544177e-324", 0x3bcd, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("5.01237274920645200930e-3011", 0x18ef, 0x80000000L, 0x00000000L), HEXCONSTE("5.01237274920645200930e-3011", 0x18ef, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("3.40282346638528859812e+38", 0x407e, 0xffffff00L, 0x00000000L), HEXCONSTE("-6.11797949834248061182e-01", 0xc000, 0xc8c34476L, 0x95a35cddL), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* bffe:9c9eca5a:4c460f93 */
	{ __LINE__, HEXCONSTE("1.79769313486231570815e+308", 0x43fe, 0xffffffffL, 0xfffff800L), HEXCONSTE("-4.96201587444489490058e-03", 0x3fff, 0xa820a48bL, 0xbe4ad518L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* bff7:a29867f3:94a41dc7 */
	{ __LINE__, HEXCONSTE("1.18973149535723176502e+4932", 0x7ffe, 0xffffffffL, 0xffffffffL), HEXCONSTE("-7.93436651174057592855e+00", 0xbffb, 0xd67de6dcL, 0xf70ec995L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* c001:fde65499:4ce86fdb */
	{ __LINE__, HEXCONSTE("-3.40282346638528859812e+38", 0xc07e, 0xffffff00L, 0x00000000L), HEXCONSTE("6.11797949834248061182e-01", 0x4000, 0xc8c34476L, 0x95a35cddL), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 3ffe:9c9eca5a:4c460f93 */
	{ __LINE__, HEXCONSTE("-1.79769313486231570815e+308", 0xc3fe, 0xffffffffL, 0xfffff800L), HEXCONSTE("4.96201587444489490058e-03", 0xbfff, 0xa820a48bL, 0xbe4ad518L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 3ff7:a29867f3:94a41dc7 */
	{ __LINE__, HEXCONSTE("-1.18973149535723176502e+4932", 0xfffe, 0xffffffffL, 0xffffffffL), HEXCONSTE("7.93436651174057592855e+00", 0x3ffb, 0xd67de6dcL, 0xf70ec995L), FLAG_INEXACT3|FLAG_FAIL_X87 }, /* 4001:fde65499:4ce86fdb */
	{ __LINE__, HEXCONSTE("1.17549435082228750797e-38", 0x3f81, 0x80000000L, 0x00000000L), HEXCONSTE("1.17549435082228750797e-38", 0x3f81, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("2.22507385850720138309e-308", 0x3c01, 0x80000000L, 0x00000000L), HEXCONSTE("2.22507385850720138309e-308", 0x3c01, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("3.36210314311209350626e-4932", 0x0001, 0x80000000L, 0x00000000L), HEXCONSTE("3.36210314311209350626e-4932", 0x0001, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.68105157155604675313e-4932", 0x0000, 0x40000000L, 0x00000000L), HEXCONSTE("1.68105157155604675313e-4932", 0x0000, 0x40000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("2.00416836000897277800e-292", 0x3c36, 0x80000000L, 0x00000000L), HEXCONSTE("2.00416836000897277800e-292", 0x3c36, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-1.17549435082228750797e-38", 0xbf81, 0x80000000L, 0x00000000L), HEXCONSTE("-1.17549435082228750797e-38", 0xbf81, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-2.22507385850720138309e-308", 0xbc01, 0x80000000L, 0x00000000L), HEXCONSTE("-2.22507385850720138309e-308", 0xbc01, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-3.36210314311209350626e-4932", 0x8001, 0x80000000L, 0x00000000L), HEXCONSTE("-3.36210314311209350626e-4932", 0x8001, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-1.68105157155604675313e-4932", 0x8000, 0x40000000L, 0x00000000L), HEXCONSTE("-1.68105157155604675313e-4932", 0x8000, 0x40000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-2.00416836000897277800e-292", 0xbc36, 0x80000000L, 0x00000000L), HEXCONSTE("-2.00416836000897277800e-292", 0xbc36, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("4.94065645841246544177e-324", 0x3bcd, 0x80000000L, 0x00000000L), HEXCONSTE("4.94065645841246544177e-324", 0x3bcd, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("3.64519953188247460253e-4951", 0x0000, 0x00000000L, 0x00000001L), HEXCONSTE("3.64519953188247460253e-4951", 0x0000, 0x00000000L, 0x00000001L), 0 },
	{ __LINE__, HEXCONSTE("0.00000000000000000000e+00", 0x0000, 0x00000000L, 0x00000000L), HEXCONSTE("0.00000000000000000000e+00", 0x0000, 0x00000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-1.40129846432481707092e-45", 0xbf6a, 0x80000000L, 0x00000000L), HEXCONSTE("-1.40129846432481707092e-45", 0xbf6a, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-4.94065645841246544177e-324", 0xbbcd, 0x80000000L, 0x00000000L), HEXCONSTE("-4.94065645841246544177e-324", 0xbbcd, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-3.64519953188247460253e-4951", 0x8000, 0x00000000L, 0x00000001L), HEXCONSTE("-3.64519953188247460253e-4951", 0x8000, 0x00000000L, 0x00000001L), 0 },
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
	status |= test_table_f_f_op(ftan_data, ARRAY_SIZE(ftan_data), __FILE__);
#endif
#ifdef TEST_FUNC_F_F
	status |= test_table_f_f_func(ftan_data, ARRAY_SIZE(ftan_data), __FILE__);
#endif

	return status ? EXIT_FAILURE : EXIT_SUCCESS;
}
