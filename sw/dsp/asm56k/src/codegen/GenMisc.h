/*

Project:    asm56k
Author:     M.Buras (sqward)


*/

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include <asm_types.h>
#include <CodeUtils.h>

extern uint const jmp_pattern[2];
extern uint const jcc_pattern[2];
extern uint const jsr_pattern[2];
extern uint const jscc_pattern[2];

extern int const jclr_patterns[6];
extern int const jsclr_patterns[6];
extern int const jset_patterns[6];
extern int const jsset_patterns[6];

extern uint const movec_pattern1[2];
extern uint const movec_pattern2[2];
extern uint const movem_pattern1[2];
extern uint const movem_pattern2[2];

void GenOneParamParMove(uint insn_patt, uint reg, bcode *par_move);
void GenBccRelTarger(uint insn_patt, raddr *rel_target);

void GenBraRelTarger(raddr *rel_target);
void GenJccBitRel(const int *insn_patt, int val, int xory, bcode *ea, raddr *rel_target);
void GenJccBitRelReg(int insn_patt, int val, int dest_reg, raddr *rel_target);
void GenBrkCC(int condition);
void GenBscc(uint condition, raddr *rel_target);
void GenBsr(raddr *rel_target);
void GenClb(uint src_reg, uint dest_reg);
void GenDebug(uint inst_patt, uint condition);
void GenDo1(uint xory, bcode *ea , raddr *rel_target);
void GenDo2(int val, raddr *rel_target);
void GenDo3(uint src_reg, raddr *rel_target);
void GenDoForever(raddr *rel_target);
void GenDor1(uint xory, bcode *ea , raddr *rel_target);
void GenDor2(int val, raddr *rel_target);
void GenDor3(uint src_reg, raddr *rel_target);
void GenDorForever(raddr *rel_target);
void GenEnddo(void);
void GenIllegal(void);
void GenJmpJsrJsccJcc(const uint *insn_patt, uint condition, bcode *ea);
void GenJccBitAbs(const int *insn_patt, int val, int xory, bcode *ea, raddr *rel_target);
void GenJccBitAbsReg(int insn_patt, int val, uint dest_reg, raddr *rel_target);
void GenLra(raddr *rel_target, uint dest_reg);
void GenLua1(uint mmrrr, uint dest_reg);
void GenLua2(uint rreg, int val, uint dest_reg);
void GenMerge(uint src_reg, uint dest_reg);
void GenMove(bcode *par_move);
void GenMovec1(const uint *insn_patt, uint dir, uint xory, bcode *ea, uint dest_reg);
void GenMovec2(uint src_reg, uint dest_reg);
void GenMovec3(uint sh, int val, uint dest_reg);
void GenMovem(const uint *insn_patt, uint dir, bcode *ea, uint reg);
void GenMovep(uint src_xory, bcode *src_ea, uint dst_xory, bcode *dst_ea);
void GenMovep2(uint rw, bcode *src_ea, uint dst_xory, bcode *dst_ea);
void GenMovep3(int val, uint dst_xory, bcode *dst_ea);
void GenMovep4(uint rw, uint dory, bcode *ea, uint reg);
void GenNoArgOpcode(uint opcode);
void GenPlockPunloc(uint opcode, bcode *ea);
void GenPlockrPunlockr(uint opcode, raddr *rel_target);
void GenRep1(uint xory, bcode *ea);
void GenRep2(int exp);
void GenRep3(uint reg);
void GenTcc1(uint condition, uint src_reg, uint dst_reg);
void GenTcc2(uint condition, uint src_reg1, uint dst_reg1, uint src_reg2, uint dst_reg2);
void GenTfr(uint src_reg, uint dst_reg, bcode *par_move);
void GenTrapcc(uint condition);
void GenVsl(uint reg, int val, bcode *ea);

bcode GenEA1(uint opcode, uint addr_reg, uint offs_reg);
bcode GenEA2(uint opcode, uint addr_reg);

bcode GenImmLong(uint opcode, uint addr);
bcode GenImmShortIO(uint opcode, uint addr);
bcode GenImmShortAbs(uint opcode, uint addr);

raddr GenRelAddrLong(int addr);
raddr GenRelAddrShort(int addr);
raddr GenRelAddrReg(uint reg);

#endif /* _CODEGEN_H_ */
