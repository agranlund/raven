/*

Project:    asm56k
Author:     M.Buras (sqward)


*/

#ifndef _GENARITH_H_
#define _GENARITH_H_

#include <asm_types.h>
#include <CodeUtils.h>

extern int	const add_patterns[3];
extern int	const sub_patterns[3];
extern int	const cmp_patterns[3];
extern int const and_patterns[3];
extern int const eor_patterns[3];
extern int const or_patterns[3];
extern int const asl_patterns[3];
extern int const asr_patterns[3];
extern int const lsl_patterns[3];
extern int const lsr_patterns[3];

extern int const incdec_patterns[2];

extern uint const mpy_pattern[4];
extern uint const macr_pattern[3];
extern uint const mpyr_pattern[3];
extern uint const max_pattern[2];
extern uint const mac_pattern[4];

void GenDiv(uint src_reg, uint dst_reg);
void GenDMac(uint modifier, uint plusminus, int val, uint dest_reg);
void GenAndEorOr(uint insn_patt, uint src_reg, uint dest_reg, bcode *par_move);
void GenAdcSbc(uint insn_patt, uint src_reg, uint dest_reg, bcode *par_move);
void GenAddSub(uint insn_patt, uint src_reg, uint dest_reg, bcode *par_move);
void GenCmp(uint insn_patt, uint src_reg, uint dest_reg, bcode *par_move);
void GenAddSubEorOrLong(uint insn_patt, int val, uint dest_reg);
void GenAddSubEorOrShort(uint insn_patt, int val, uint dest_reg);
void GenAddxSubx(uint insn_patt, uint src_reg, uint dest_reg, bcode *par_move);
void GenAndiOri(uint insn_patt, int val, uint dest_reg);
void GenAsxImmediate(uint insn_patt, int val, uint src_reg, uint dest_reg);
void GenAsxReg(uint insn_patt, int val_reg, uint src_reg, uint dest_reg);
void GenCmpm(uint src_reg, uint dest_reg, bcode *par_move);
void GenCmpu(uint src_reg, uint dest_reg);
void GenIncDec(uint inst_patt, uint reg);
void GenLsxReg(const int *insn_patt, int src_reg, uint dest_reg);
void GenLsxImmediate(const int *insn_patt, int val, uint dest_reg);
void GenMul1(const uint *insn_patt, uint plusminus, uint reg_pair, uint dest_reg, bcode *par_move);
void GenMul2(const uint *insn_patt, uint plusminus, uint src_reg, int val, uint dest_reg);
void GenMuli(const uint *insn_patt, uint plusminus, int val, uint src_reg, uint dest_reg);
void GenMulxx(const uint *insn_patt, uint code, uint plusminus, uint reg_pair, uint dest_reg);
void GenMax(uint insn_patt, uint src_reg, uint dst_reg, bcode *par_move);
void GenNorm(uint src_reg, uint dst_reg);
void GenNormf(uint src_reg, uint dst_reg);

#endif /* _GENARITH_H_ */
