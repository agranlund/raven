#ifndef __dsp_unasm_h__
#define __dsp_unasm_h__ 1

/* from acon56k.c */
int dsptl_acon_56k (uint32_t op, uint32_t kinsl, uint32_t seql, uint32_t dev_flag);

/* from core56k.c */
void dsptl_corergwf_56k (int regnum);
void dsptl_bwf_56k (int regnum, int bus);
void dsptl_mbwf_56k (int regnum, int bus);
void dsptl_arf_56k (int regnum, int bus);

/* from dalu56k.c */
void dsptl_norm_56k (uint32_t *hr);
void dsptl_dalu_56k (uint32_t *hr);

/* from decod56k.c */
int dsptl_dcd1_56k (uint32_t op, uint32_t devnum);
int dsptl_illeg_56k (uint32_t op, uint32_t devnum);

/* from execa56k.c */
int dsptl_decp_56k (uint32_t op, uint32_t *ip);

/* from ibs56k.c */
struct dsp_ibsout56k
{
	int xs;
	int xd;
	int ys;
	int yd;
	int rw;
	int imm;
	int seq;
	int rsm;
};
int dsptl_ibs_56k (uint32_t devnum, uint32_t op, uint32_t kinsl, uint32_t seql, struct dsp_ibsout56k **retptr);

/* from mem56k.c */
#if ADSx
int dsptl_rmem_56k (enum memory_map memt, uint32_t add, uint32_t count, uint32_t *to_buf);
int dsptl_wmem_56k (enum memory_map memt, uint32_t add, uint32_t count, uint32_t *valuep);
#else
int dsptl_rmem_56k (enum memory_map memt, uint32_t add, uint32_t *to_buf);
int dsptl_wmem_56k (enum memory_map memt, uint32_t add, uint32_t *valuep);
#endif /* ADSx */
int dsptl_mattr_56k (enum memory_map memt, uint32_t add);

/* from pag56k.c */
struct dsp_pagout56k
{
	int oh;
	int ol;
	int os;
};
int dsptl_pag_56k (uint32_t op, uint32_t kinsl, uint32_t seql, struct dsp_pagout56k **retptr, uint32_t dev_flag);

/* from sas56000.c */
void dsptl_sasm_56k(char *s, uint32_t asmaddr);

/* from simmasm.c */
int dspt_masm_56k (char *asm_string, uint32_t *ops, char **error_ptr);

/* from small56k.c */
int dsptl_small_56k (uint32_t op, uint32_t kins, uint32_t *retvp, uint32_t dev_flag);

/* from unasm56k.c */
int dspt_unasm_56004 (uint32_t *ops, char *strp, uint32_t srflag, uint32_t omrflag, struct sdb_dasm *sdb_retp);
int dspt_unasm_56002 (uint32_t *ops, char *strp, uint32_t srflag, uint32_t omrflag, struct sdb_dasm *sdb_retp);
int dspt_unasm_56k (uint32_t *ops, char *strp, uint32_t srflag, uint32_t omrflag, struct sdb_dasm *sdb_retp);
int dspt_unasm_56x (uint32_t devtype, uint32_t *ops, char *strp, uint32_t srflag, uint32_t omrflag, struct sdb_dasm *sdb_retp);

/* from nda56k.c */
int dspl_unl56k(const char *devname, const char *devpasswd);

#endif
