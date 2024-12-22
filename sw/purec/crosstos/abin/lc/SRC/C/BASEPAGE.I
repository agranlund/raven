**
** $Header: e:/src/i\basepage.i,v 1.1 1993/11/09 09:56:48 AGK Exp $
**
** Format of GEMDOS basepage
**
** (c) Copyright 1992 HiSoft
**
		IFND	__BASEPAGE_I
__BASEPAGE_I	EQU 1

		OFFSET	0
p_lowtpa	DS.L	1
p_hitpa		DS.L	1
p_tbase		DS.L	1
p_tlen		DS.L	1
p_dbase		DS.L	1
p_dlen		DS.L	1
p_bbase		DS.L	1
p_blen		DS.L	1
p_dta		DS.L	1
p_parent	DS.L	1
p_reserved	DS.L	1
p_env		DS.L	1
		DS.L	20
p_cmdlin	DS.B	128
p_sizeof	DS.B	0
		ENDC
