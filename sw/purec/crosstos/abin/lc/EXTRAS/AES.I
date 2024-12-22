**
** $Header: e:/dercs\aes.i,v 1.2 1993/11/08 17:25:50 AGK Exp $
**
** AES macro definitions for DERCS & Devpac
**
** This file is designed for use with the VDI macros and library supplied
** as part of Devpac. The file is usable either by Devpac or by ASM
**
** (c) Copyright 1991, 1993 HiSoft
**

	IFND	__AES_I
__AES_I	SET	1

IBM		equ	3
SMALL		equ	5

G_BOX		equ	20
G_TEXT		equ	21
G_BOXTEXT	equ	22
G_IMAGE		equ	23
G_PROGDEF	equ	24
G_IBOX		equ	25
G_BUTTON	equ	26
G_BOXCHAR	equ	27
G_STRING	equ	28
G_FTEXT		equ	29
G_FBOXTEXT	equ	30
G_ICON		equ	31
G_TITLE		equ	32
G_CICON		equ	33

TE_LEFT		equ	0
TE_RIGHT	equ	1
TE_CNTR		equ	2

		IFD	__G2
_TEDINFO	MACRO
		dc.l	\1
		dc.l	\2
		dc.l	\3
		dc.w	\4
		dc.w	\5
		dc.w	\6
		dc.w	\7
		dc.w	\8
		dc.w	\9
		dc.w	\A
		dc.w	\B
		ENDM
		ELSEIF
		MACRO
	_TEDINFO	te_ptext,te_ptmplt,te_pvalid,te_font,te_junk1,te_just,te_color,te_junk2,te_thickness,te_txtlen,te_tmplen
		dc.l	te_ptext
		dc.l	te_ptmplt
		dc.l	te_pvalid
		dc.w	te_font
		dc.w	te_junk1
		dc.w	te_just
		dc.w	te_color
		dc.w	te_junk2
		dc.w	te_thickness
		dc.w	te_txtlen
		dc.w	te_tmplen
		ENDM
		ENDC

		IFD	__G2
_BITBLK		MACRO
		dc.l	\1
		dc.w	\2
		dc.w	\3
		dc.w	\4
		dc.w	\5
		dc.w	\6
		ENDM
		ELSEIF
		MACRO
	_BITBLK	bi_pdata,bi_wb,bi_hl,bi_x,bi_y,bi_color
		dc.l	bi_pdata
		dc.w	bi_wb
		dc.w	bi_hl
		dc.w	bi_x
		dc.w	bi_y
		dc.w	bi_color
		ENDM
		ENDC

		IFD	__G2
_ICONBLK	MACRO
		dc.l	\1
		dc.l	\2
		dc.l	\3
		dc.w	\4
		dc.w	\5
		dc.w	\6
		dc.w	\7
		dc.w	\8
		dc.w	\9
		dc.w	\A
		dc.w	\B
		dc.w	\C
		dc.w	\D
		dc.w	\E
		ENDM
		ELSEIF
		MACRO
	_ICONBLK	ib_pmask,ib_pdata,ib_ptext,ib_char,ib_xchar,ib_ychar,ib_xicon,ib_yicon,ib_wicon,ib_hicon,ib_xtext,ib_ytext,ib_wtext,ib_htext
		dc.l	ib_pmask
		dc.l	ib_pdata
		dc.l	ib_ptext
		dc.w	ib_char
		dc.w	ib_xchar
		dc.w	ib_ychar
		dc.w	ib_xicon
		dc.w	ib_yicon
		dc.w	ib_wicon
		dc.w	ib_hicon
		dc.w	ib_xtext
		dc.w	ib_ytext
		dc.w	ib_wtext
		dc.w	ib_htext
		ENDM
		ENDC

		IFD	__G2
_OBJECT		MACRO
		dc.w	\1
		dc.w	\2
		dc.w	\3
		dc.w	\4
		dc.w	\5
		dc.w	\6
		dc.l	\7
		dc.w	\8
		dc.w	\9
		dc.w	\A
		dc.w	\B
		ENDM
		ELSEIF
		MACRO
	_OBJECT	ob_next,ob_head,ob_tail,ob_type,ob_flags,ob_state,ob_spec,ob_x,ob_y,ob_width,ob_height
		dc.w	ob_next
		dc.w	ob_head
		dc.w	ob_tail
		dc.w	ob_type
		dc.w	ob_flags
		dc.w	ob_state
		dc.l	ob_spec
		dc.w	ob_x
		dc.w	ob_y
		dc.w	ob_width
		dc.w	ob_height
		ENDM
		ENDC

		IFD	__G2
_CICON		MACRO
		dc.w	\1
		dc.l	\2
		dc.l	\3
		dc.l	\4
		dc.l	\5
		dc.l	\6
		ENDM
		ELSEIF
		MACRO
	_CICON	num_planes,col_data,col_mask,sel_data,sel_mask,next_res
		dc.w	num_planes
		dc.l	col_data
		dc.l	col_mask
		dc.l	sel_data
		dc.l	sel_mask
		dc.l	next_res
		ENDM
		ENDC

		IFD	__G2
_CICONBLK	MACRO
		_ICONBLK	\1,\2,\3,\4,\5,\6,\7,\8,\9,\A,\B,\C,\D,\E
		dc.l	\F
		ENDM
		ELSEIF
		MACRO
	_CICONBLK	ib_pmask,ib_pdata,ib_ptext,ib_char,ib_xchar,ib_ychar,ib_xicon,ib_yicon,ib_wicon,ib_hicon,ib_xtext,ib_ytext,ib_wtext,ib_htext,mainlist
		_ICONBLK	ib_pmask,ib_pdata,ib_ptext,ib_char,ib_xchar,ib_ychar,ib_xicon,ib_yicon,ib_wicon,ib_hicon,ib_xtext,ib_ytext,ib_wtext,ib_htext
		dc.l	mainlist
		ENDM
		ENDC
	ENDC
