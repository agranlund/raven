/*

Project:    asm56k
Author:     M.Buras (sqward)


*/
#ifndef _CONVERTFIELDS_H_
#define _CONVERTFIELDS_H_

#include "asm_types.h"

int	ddddd_2_ff(unsigned int code);
int	ddddd_2_ddddd(unsigned int code);
int	ddddd_2_yff(unsigned int code);
int	ddddd_2_LLL(unsigned int code);
int	ddddd_2_sss(unsigned int code);
int	ddddd_2_d(unsigned int code);
int	ddddd_2_d_src(unsigned int code);
int	ddddd_2_d_dst(unsigned int code);
int	ddddd_2_JJJ(unsigned int code);
int ddddd_2_JJJ2(unsigned int code);
int	ddddd_2_EE(unsigned int code);
int	ddddd_2_JJ(unsigned int code);
int ddddd_2_JJ_src(unsigned int code);
int	ddddd_2_DDDDDD(unsigned int code);
int	ddddd_2_ggg(unsigned int code);
int	ddddd_2_dddd(unsigned int code);
int	ddddd_2_QQ(unsigned int code);
int	ddddd_2_qq(unsigned int code);
int	ddddd_2_ddddd2(unsigned int code);
int ddddd_2_eeeeee(unsigned int code);
int	ddddd_2_RRR(unsigned int code);
int	ea_2_MMRRR(unsigned int code);
int	ea_2_mmrr(unsigned int code, unsigned int code2);
int	exp_2_ssss(int value);

int GetQQQ(int first, int second, int dont_iterate);
int GetQQQQXregXreg(int first, int second);
int GetQQQQXregYreg(int first, int second);
int GetQQQQYregXreg(int first, int second);
int GetQQQQYregYreg(int first, int second);

bool isAguReg(uint reg);
const char *getRegName(uint reg);

#endif /* _CONVERTFIELDS_H_ */
