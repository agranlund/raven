/*
 * Test:USES_FPU
 */
#include "testconfig.h"
#define TEST_OP_F_F "ftanh"
#define TEST_FUNC_F_F fpu_tanh
#include "testdriver.h"



static test_f_f_data const ftanh_data[] = {
	{ __LINE__, ZERO_P, ZERO_P, 0 },
	{ __LINE__, ZERO_M, ZERO_M, 0 },
	{ __LINE__, INF_P, HEXCONSTE("1.0", 0x3fff, 0x80000000L, 0x00000000L), FLAG_XFAIL_LINUX },
	{ __LINE__, INF_M, HEXCONSTE("-1.0", 0xbfff, 0x80000000L, 0x00000000L), FLAG_XFAIL_LINUX },
	{ __LINE__, QNAN_P, QNAN_P, 0 },
	{ __LINE__, QNAN_M, QNAN_P, 0 },
	{ __LINE__, SNAN_P, QNAN_P, 0 },
	{ __LINE__, SNAN_M, QNAN_P, 0 },

	{ __LINE__, HEXCONSTE("7.50000000000000000000e-01", 0x3ffe, 0xc0000000L, 0x00000000L), HEXCONSTE("6.35148952387287319223e-01", 0x3ffe, 0xa2991f2aL, 0x9791413aL), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("-7.50000000000000000000e-01", 0xbffe, 0xc0000000L, 0x00000000L), HEXCONSTE("-6.35148952387287319223e-01", 0xbffe, 0xa2991f2aL, 0x9791413aL), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), HEXCONSTE("7.61594155955764888109e-01", 0x3ffe, 0xc2f7d5a8L, 0xa79ca2acL), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("-1.00000000000000000000e+00", 0xbfff, 0x80000000L, 0x00000000L), HEXCONSTE("-7.61594155955764888109e-01", 0xbffe, 0xc2f7d5a8L, 0xa79ca2acL), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("2.00000000000000000000e+00", 0x4000, 0x80000000L, 0x00000000L), HEXCONSTE("9.64027580075816883953e-01", 0x3ffe, 0xf6ca82f0L, 0xde1e9e9aL), 0 },
	{ __LINE__, HEXCONSTE("-2.00000000000000000000e+00", 0xc000, 0x80000000L, 0x00000000L), HEXCONSTE("-9.64027580075816883953e-01", 0xbffe, 0xf6ca82f0L, 0xde1e9e9aL), 0 },
	{ __LINE__, HEXCONSTE("3.00000000000000000000e+00", 0x4000, 0xc0000000L, 0x00000000L), HEXCONSTE("9.95054753686730451313e-01", 0x3ffe, 0xfebbe888L, 0xd057ff10L), 0 },
	{ __LINE__, HEXCONSTE("-3.00000000000000000000e+00", 0xc000, 0xc0000000L, 0x00000000L), HEXCONSTE("-9.95054753686730451313e-01", 0xbffe, 0xfebbe888L, 0xd057ff10L), 0 },
	{ __LINE__, HEXCONSTE("4.00000000000000000000e+00", 0x4001, 0x80000000L, 0x00000000L), HEXCONSTE("9.99329299739067043783e-01", 0x3ffe, 0xffd40b84L, 0x505a10b4L), 0 },
	{ __LINE__, HEXCONSTE("-4.00000000000000000000e+00", 0xc001, 0x80000000L, 0x00000000L), HEXCONSTE("-9.99329299739067043783e-01", 0xbffe, 0xffd40b84L, 0x505a10b4L), 0 },
	{ __LINE__, HEXCONSTE("5.00000000000000000000e+00", 0x4001, 0xa0000000L, 0x00000000L), HEXCONSTE("9.99909204262595131211e-01", 0x3ffe, 0xfffa0cb3L, 0x46f889a8L), 0 },
	{ __LINE__, HEXCONSTE("-5.00000000000000000000e+00", 0xc001, 0xa0000000L, 0x00000000L), HEXCONSTE("-9.99909204262595131211e-01", 0xbffe, 0xfffa0cb3L, 0x46f889a8L), 0 },
	{ __LINE__, HEXCONSTE("6.00000000000000000000e+00", 0x4001, 0xc0000000L, 0x00000000L), HEXCONSTE("9.99987711650795570574e-01", 0x3ffe, 0xffff31d5L, 0xf129deeeL), 0 },
	{ __LINE__, HEXCONSTE("-6.00000000000000000000e+00", 0xc001, 0xc0000000L, 0x00000000L), HEXCONSTE("-9.99987711650795570574e-01", 0xbffe, 0xffff31d5L, 0xf129deeeL), 0 },
	{ __LINE__, HEXCONSTE("7.00000000000000000000e+00", 0x4001, 0xe0000000L, 0x00000000L), HEXCONSTE("9.99998336943944671760e-01", 0x3ffe, 0xffffe419L, 0x3a878ed7L), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("-7.00000000000000000000e+00", 0xc001, 0xe0000000L, 0x00000000L), HEXCONSTE("-9.99998336943944671760e-01", 0xbffe, 0xffffe419L, 0x3a878ed7L), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("8.00000000000000000000e+00", 0x4002, 0x80000000L, 0x00000000L), HEXCONSTE("9.99999774929675889785e-01", 0x3ffe, 0xfffffc39L, 0x548fc348L), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("-8.00000000000000000000e+00", 0xc002, 0x80000000L, 0x00000000L), HEXCONSTE("-9.99999774929675889785e-01", 0xbffe, 0xfffffc39L, 0x548fc348L), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("9.00000000000000000000e+00", 0x4002, 0x90000000L, 0x00000000L), HEXCONSTE("9.99999969540040974504e-01", 0x3ffe, 0xffffff7dL, 0x2cebbe21L), 0 },
	{ __LINE__, HEXCONSTE("-9.00000000000000000000e+00", 0xc002, 0x90000000L, 0x00000000L), HEXCONSTE("-9.99999969540040974504e-01", 0xbffe, 0xffffff7dL, 0x2cebbe21L), 0 },
	{ __LINE__, HEXCONSTE("1.00000000000000000000e+01", 0x4002, 0xa0000000L, 0x00000000L), HEXCONSTE("9.99999995877692763604e-01", 0x3ffe, 0xffffffeeL, 0x4b79aaa9L), 0 },
	{ __LINE__, HEXCONSTE("-1.00000000000000000000e+01", 0xc002, 0xa0000000L, 0x00000000L), HEXCONSTE("-9.99999995877692763604e-01", 0xbffe, 0xffffffeeL, 0x4b79aaa9L), 0 },
	{ __LINE__, HEXCONSTE("1.50000000000000000000e+01", 0x4002, 0xf0000000L, 0x00000000L), HEXCONSTE("9.99999999999812847515e-01", 0x3ffe, 0xffffffffL, 0xffcb523eL), 0 },
	{ __LINE__, HEXCONSTE("-1.50000000000000000000e+01", 0xc002, 0xf0000000L, 0x00000000L), HEXCONSTE("-9.99999999999812847515e-01", 0xbffe, 0xffffffffL, 0xffcb523eL), 0 },
	{ __LINE__, HEXCONSTE("2.00000000000000000000e+01", 0x4003, 0xa0000000L, 0x00000000L), HEXCONSTE("9.99999999999999991489e-01", 0x3ffe, 0xffffffffL, 0xffffff63L), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("-2.00000000000000000000e+01", 0xc003, 0xa0000000L, 0x00000000L), HEXCONSTE("-9.99999999999999991489e-01", 0xbffe, 0xffffffffL, 0xffffff63L), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("2.20000000000000000000e+01", 0x4003, 0xb0000000L, 0x00000000L), HEXCONSTE("9.99999999999999999837e-01", 0x3ffe, 0xffffffffL, 0xfffffffdL), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("-2.20000000000000000000e+01", 0xc003, 0xb0000000L, 0x00000000L), HEXCONSTE("-9.99999999999999999837e-01", 0xbffe, 0xffffffffL, 0xfffffffdL), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("2.50000000000000000000e+01", 0x4003, 0xc8000000L, 0x00000000L), HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-2.50000000000000000000e+01", 0xc003, 0xc8000000L, 0x00000000L), HEXCONSTE("-1.00000000000000000000e+00", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("3.00000000000000000000e+01", 0x4003, 0xf0000000L, 0x00000000L), HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-3.00000000000000000000e+01", 0xc003, 0xf0000000L, 0x00000000L), HEXCONSTE("-1.00000000000000000000e+00", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("3.50000000000000000000e+01", 0x4004, 0x8c000000L, 0x00000000L), HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-3.50000000000000000000e+01", 0xc004, 0x8c000000L, 0x00000000L), HEXCONSTE("-1.00000000000000000000e+00", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("4.00000000000000000000e+01", 0x4004, 0xa0000000L, 0x00000000L), HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-4.00000000000000000000e+01", 0xc004, 0xa0000000L, 0x00000000L), HEXCONSTE("-1.00000000000000000000e+00", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("4.50000000000000000000e+01", 0x4004, 0xb4000000L, 0x00000000L), HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-4.50000000000000000000e+01", 0xc004, 0xb4000000L, 0x00000000L), HEXCONSTE("-1.00000000000000000000e+00", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("5.00000000000000000000e+01", 0x4004, 0xc8000000L, 0x00000000L), HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-5.00000000000000000000e+01", 0xc004, 0xc8000000L, 0x00000000L), HEXCONSTE("-1.00000000000000000000e+00", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("6.93889390390722837765e-18", 0x3fc6, 0x80000000L, 0x00000000L), HEXCONSTE("6.93889390390722837765e-18", 0x3fc6, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("9.01464045047760009766e-01", 0x3ffe, 0xe6c65900L, 0x00000000L), HEXCONSTE("7.17009991844966219671e-01", 0x3ffe, 0xb78df781L, 0xe11d83e5L), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("5.47448992729187011719e-01", 0x3ffe, 0x8c259e00L, 0x00000000L), HEXCONSTE("4.98605843319011067717e-01", 0x3ffd, 0xff4943ddL, 0x4c9f4514L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("3.96516621112823486328e-01", 0x3ffd, 0xcb043a00L, 0x00000000L), HEXCONSTE("3.76964510303130499882e-01", 0x3ffd, 0xc1017e07L, 0x025b009bL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("5.30424416065216064453e-01", 0x3ffe, 0x87c9e500L, 0x00000000L), HEXCONSTE("4.85705449612686392059e-01", 0x3ffd, 0xf8ae627bL, 0x26a334feL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("-2.31946453452110290527e-01", 0xbffc, 0xed835f00L, 0x00000000L), HEXCONSTE("-2.27874549155812908719e-01", 0xbffc, 0xe957f220L, 0xdc1eb288L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("4.82811272144317626953e-01", 0x3ffd, 0xf7330a00L, 0x00000000L), HEXCONSTE("4.48492237471829255813e-01", 0x3ffd, 0xe5a0c648L, 0xe71a47aaL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("4.76942539215087890625e-01", 0x3ffd, 0xf431d000L, 0x00000000L), HEXCONSTE("4.43791654161363958097e-01", 0x3ffd, 0xe338a8e1L, 0xb8bad812L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("-1.48562371730804443359e-01", 0xbffc, 0x9820bc00L, 0x00000000L), HEXCONSTE("-1.47478972918774734710e-01", 0xbffc, 0x9704ba56L, 0x229e576eL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("2.13921783142723143101e-04", 0x3ff2, 0xe0503100L, 0x00000000L), HEXCONSTE("2.13921779879522579631e-04", 0x3ff2, 0xe05030c6L, 0x97d9e583L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("2.36342117190361022949e-01", 0x3ffc, 0xf203ab00L, 0x00000000L), HEXCONSTE("2.32037763159495026577e-01", 0x3ffc, 0xed9b4eb0L, 0xd3fe4d34L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("2.36342102289199829102e-01", 0x3ffc, 0xf203aa00L, 0x00000000L), HEXCONSTE("2.32037749060635005059e-01", 0x3ffc, 0xed9b4dbeL, 0x9c8d1de4L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("2.36342111527542142468e-01", 0x3ffc, 0xf203aa9eL, 0xb6a8c000L), HEXCONSTE("2.32037757801570935064e-01", 0x3ffc, 0xed9b4e54L, 0xc79810eaL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("1.25237971544265747070e-01", 0x3ffc, 0x803e6200L, 0x00000000L), HEXCONSTE("1.24587286465171515829e-01", 0x3ffb, 0xff279e86L, 0xec1fd6d8L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("1.25237956643104553223e-01", 0x3ffc, 0x803e6100L, 0x00000000L), HEXCONSTE("1.24587271795305998827e-01", 0x3ffb, 0xff279c8eL, 0xde9f66b9L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("1.25237962086789667016e-01", 0x3ffc, 0x803e615dL, 0x85949000L), HEXCONSTE("1.24587277154494292374e-01", 0x3ffb, 0xff279d47L, 0x028a9a28L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-1.46172380447387695312e+01", 0xc002, 0xe9e03500L, 0x00000000L), HEXCONSTE("-9.99999999999597599665e-01", 0xbffe, 0xffffffffL, 0xff8ebc00L), 0 },
	{ __LINE__, HEXCONSTE("-2.34581664204597473145e-01", 0xbffc, 0xf0362d00L, 0x00000000L), HEXCONSTE("-2.30371416677539751056e-01", 0xbffc, 0xebe67c12L, 0x40bd5298L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("-2.34581679105758666992e-01", 0xbffc, 0xf0362e00L, 0x00000000L), HEXCONSTE("-2.30371430787881525379e-01", 0xbffc, 0xebe67d04L, 0xaaadcfd6L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("-2.34581669040965334894e-01", 0xbffc, 0xf0362d53L, 0x16922000L), HEXCONSTE("-2.30371421257236779103e-01", 0xbffc, 0xebe67c60L, 0xee75c7a6L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("-1.99059486389160156250e-01", 0xbffc, 0xcbd64000L, 0x00000000L), HEXCONSTE("-1.96471278664374300404e-01", 0xbffc, 0xc92fc451L, 0xddfe3ebeL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("1.81386277079582214355e-01", 0x3ffc, 0xb9bd5300L, 0x00000000L), HEXCONSTE("1.79422850321223587014e-01", 0x3ffc, 0xb7ba9fa9L, 0x2397b4cdL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("2.01537311077117919922e-01", 0x3ffc, 0xce5fcc00L, 0x00000000L), HEXCONSTE("1.98852292930891770209e-01", 0x3ffc, 0xcb9fef7bL, 0x7dbd391aL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("2.01537296175956726074e-01", 0x3ffc, 0xce5fcb00L, 0x00000000L), HEXCONSTE("1.98852278618955742768e-01", 0x3ffc, 0xcb9fee85L, 0x9d2dc874L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("2.01537308518033669857e-01", 0x3ffc, 0xce5fcbd4L, 0x09074800L), HEXCONSTE("1.98852290472999428174e-01", 0x3ffc, 0xcb9fef51L, 0x43d076d1L), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("2.01537308518033642102e-01", 0x3ffc, 0xce5fcbd4L, 0x09074000L), HEXCONSTE("1.98852290472999401503e-01", 0x3ffc, 0xcb9fef51L, 0x43d06f21L), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("2.01537308518033644758e-01", 0x3ffc, 0xce5fcbd4L, 0x090740c4L), HEXCONSTE("1.98852290472999404064e-01", 0x3ffc, 0xcb9fef51L, 0x43d06fdeL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("5.15659987926483154297e-01", 0x3ffe, 0x84024b00L, 0x00000000L), HEXCONSTE("4.74343441290915907840e-01", 0x3ffd, 0xf2dd24beL, 0xd549f41eL), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("5.15659928321838378906e-01", 0x3ffe, 0x84024a00L, 0x00000000L), HEXCONSTE("4.74343395097416246404e-01", 0x3ffd, 0xf2dd2332L, 0x08b57887L), FLAG_INEXACT },
	{ __LINE__, HEXCONSTE("5.15659932445832080994e-01", 0x3ffe, 0x84024a11L, 0xb6610800L), HEXCONSTE("4.74343398293504527996e-01", 0x3ffd, 0xf2dd234dL, 0x7cfb3856L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("5.15659932445831969972e-01", 0x3ffe, 0x84024a11L, 0xb6610000L), HEXCONSTE("4.74343398293504441964e-01", 0x3ffd, 0xf2dd234dL, 0x7cfb2bf0L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("5.15659932445832059472e-01", 0x3ffe, 0x84024a11L, 0xb6610673L), HEXCONSTE("4.74343398293504511326e-01", 0x3ffd, 0xf2dd234dL, 0x7cfb35efL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("5.15659932445832059418e-01", 0x3ffe, 0x84024a11L, 0xb6610672L), HEXCONSTE("4.74343398293504511272e-01", 0x3ffd, 0xf2dd234dL, 0x7cfb35edL), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("3.12500000000000000000e-02", 0x3ffa, 0x80000000L, 0x00000000L), HEXCONSTE("3.12398314460312567690e-02", 0x3ff9, 0xffeaacccL, 0x958ef142L), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("9.76562500000000000000e-04", 0x3ff5, 0x80000000L, 0x00000000L), HEXCONSTE("9.76562189559260218587e-04", 0x3ff4, 0xfffffaaaL, 0xaaccccccL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("3.05175781250000000000e-05", 0x3ff0, 0x80000000L, 0x00000000L), HEXCONSTE("3.05175781155260968604e-05", 0x3fef, 0xfffffffeL, 0xaaaaaaadL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("9.53674316406250000000e-07", 0x3feb, 0x80000000L, 0x00000000L), HEXCONSTE("9.53674316405960879438e-07", 0x3fea, 0xffffffffL, 0xffaaaaabL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("2.98023223876953125000e-08", 0x3fe6, 0x80000000L, 0x00000000L), HEXCONSTE("2.98023223876953036773e-08", 0x3fe5, 0xffffffffL, 0xffffeaabL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("9.31322574615478515625e-10", 0x3fe1, 0x80000000L, 0x00000000L), HEXCONSTE("9.31322574615478515373e-10", 0x3fe0, 0xffffffffL, 0xfffffffbL), FLAG_INEXACT3 },
	{ __LINE__, HEXCONSTE("2.91038304567337036133e-11", 0x3fdc, 0x80000000L, 0x00000000L), HEXCONSTE("2.91038304567337036133e-11", 0x3fdc, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("9.09494701772928237915e-13", 0x3fd7, 0x80000000L, 0x00000000L), HEXCONSTE("9.09494701772928237915e-13", 0x3fd7, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("2.84217094304040074348e-14", 0x3fd2, 0x80000000L, 0x00000000L), HEXCONSTE("2.84217094304040074348e-14", 0x3fd2, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("8.88178419700125232339e-16", 0x3fcd, 0x80000000L, 0x00000000L), HEXCONSTE("8.88178419700125232339e-16", 0x3fcd, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("2.77555756156289135106e-17", 0x3fc8, 0x80000000L, 0x00000000L), HEXCONSTE("2.77555756156289135106e-17", 0x3fc8, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("8.67361737988403547206e-19", 0x3fc3, 0x80000000L, 0x00000000L), HEXCONSTE("8.67361737988403547206e-19", 0x3fc2, 0xffffffffL, 0xfffffff0L), FLAG_INEXACT2 },
	{ __LINE__, HEXCONSTE("7.88860905221011805412e-31", 0x3f9b, 0x80000000L, 0x00000000L), HEXCONSTE("7.88860905221011805412e-31", 0x3f9b, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("0.00000000000000000000e+00", 0x0000, 0x00000000L, 0x00000000L), HEXCONSTE("0.00000000000000000000e+00", 0x0000, 0x00000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("2.40991986510288411774e-181", 0x3da7, 0x80000000L, 0x00000000L), HEXCONSTE("2.40991986510288411774e-181", 0x3da7, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), HEXCONSTE("1.40129846432481707092e-45", 0x3f6a, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("0.00000000000000000000e+00", 0x0000, 0x00000000L, 0x00000000L), HEXCONSTE("0.00000000000000000000e+00", 0x0000, 0x00000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("4.94065645841246544177e-324", 0x3bcd, 0x80000000L, 0x00000000L), HEXCONSTE("4.94065645841246544177e-324", 0x3bcd, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("5.01237274920645200930e-3011", 0x18ef, 0x80000000L, 0x00000000L), HEXCONSTE("5.01237274920645200930e-3011", 0x18ef, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("3.40282346638528859812e+38", 0x407e, 0xffffff00L, 0x00000000L), HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.79769313486231570815e+308", 0x43fe, 0xffffffffL, 0xfffff800L), HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("1.18973149535723176502e+4932", 0x7ffe, 0xffffffffL, 0xffffffffL), HEXCONSTE("1.00000000000000000000e+00", 0x3fff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-3.40282346638528859812e+38", 0xc07e, 0xffffff00L, 0x00000000L), HEXCONSTE("-1.00000000000000000000e+00", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-1.79769313486231570815e+308", 0xc3fe, 0xffffffffL, 0xfffff800L), HEXCONSTE("-1.00000000000000000000e+00", 0xbfff, 0x80000000L, 0x00000000L), 0 },
	{ __LINE__, HEXCONSTE("-1.18973149535723176502e+4932", 0xfffe, 0xffffffffL, 0xffffffffL), HEXCONSTE("-1.00000000000000000000e+00", 0xbfff, 0x80000000L, 0x00000000L), 0 },
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
	status |= test_table_f_f_op(ftanh_data, ARRAY_SIZE(ftanh_data), __FILE__);
#endif
#ifdef TEST_FUNC_F_F
	status |= test_table_f_f_func(ftanh_data, ARRAY_SIZE(ftanh_data), __FILE__);
#endif
	
	return status ? EXIT_FAILURE : EXIT_SUCCESS;
}
