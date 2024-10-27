				XREF	_aes
				XREF	_aes1
				XREF	_GemParBlk
				XREF	_VdiCtrl
				XREF	_VdiCtrl2
				XREF	_VdiParBlk
				XREF	vdi
				XREF	vdipb
				XREF	aespb
				
				XREF	_mt_vdi
				XREF	_mt_vdi_end

;
; sizes of the various arrays
; these MUST match the definitions in mt_gem.h/aes.h
;
AES_CTRLMAX		equ		6	; actually 5; use 6 to make it long aligned
AES_GLOBMAX		equ		16
AES_INTINMAX 	equ		16
AES_INTOUTMAX	equ		16
AES_ADDRINMAX	equ		16
AES_ADDROUTMAX	equ		16

VDI_CNTRLMAX    equ		16  ; actually 15; use 16 to make it long aligned
VDI_INTINMAX    equ		1024
VDI_INTOUTMAX   equ		256
VDI_PTSINMAX    equ		256
VDI_PTSOUTMAX   equ		256

				OFFSET	0

control:		ds.w	VDI_CNTRLMAX
global:			ds.w	AES_GLOBMAX
intin:			ds.w	VDI_INTINMAX
intout:			ds.w	VDI_INTOUTMAX
ptsout:			ds.w	VDI_PTSOUTMAX
addrin:			ds.l	AES_ADDRINMAX
addrout:		ds.l	AES_ADDROUTMAX
ptsin:			ds.w	VDI_PTSINMAX
acontrl:		ds.w	AES_CTRLMAX
aintin:			ds.w	AES_INTINMAX
aintout:		ds.w	AES_INTOUTMAX
_GemParBSize	equ		(*)

				OFFSET	0

vdi_pb:			ds.l	5
vdi_control:	ds.w	VDI_CNTRLMAX
vdi_intin:		ds.w	VDI_INTINMAX
vdi_intout:		ds.w	VDI_INTOUTMAX
vdi_ptsout:		ds.w	VDI_PTSOUTMAX
vdi_ptsin:		ds.w	VDI_PTSINMAX
_VdiParBSize	equ		(*)


; offsets of VDI control array
				OFFSET	control
v_opcode:		ds.w	1
v_nptsin:		ds.w	1
v_nptsout:		ds.w	1
v_nintin:		ds.w	1
v_nintout:		ds.w	1
v_opcode2:		ds.w	1
v_handle:		ds.w	1
v_param:		ds.l	4

; offsets of AES control array
				OFFSET	acontrl
a_opcode:		ds.w	1
a_nintin:		ds.w	1
a_nintout:		ds.w	1
a_naddrin:		ds.w	1
a_naddrout:		ds.w	1

; offsets of AES parameter block
				OFFSET	0
a_control:		ds.l	1
a_global:		ds.l	1
a_intin:		ds.l	1
a_intout:		ds.l	1
a_addrin:		ds.l	1
a_addrout:		ds.l	1

; offsets of VDI parameter block
				OFFSET	0
v_control:		ds.l	1
v_intin:		ds.l	1
v_ptsin:		ds.l	1
v_intout:		ds.l	1
v_ptsout:		ds.l	1


				MACRO  VDI_SET_PARAMS handle, opcode, subop, nptsin, nintin
					move.l sp,a0
					move.l v_control(a0),a1
					move.w opcode,(a1)+
					move.w nptsin,(a1)+
					clr.w (a1)+ ; nptsout
					move.w nintin,(a1)+
					clr.w (a1)+ ; nintout
					move.w subop,(a1)+
					move.w handle,(a1)+
				ENDM
				
				MACRO  VDI_CALL_ESC handle, opcode, subop, nptsin, nintin
					VDI_SET_PARAMS handle, opcode, subop, nptsin, nintin
					bsr _mt_vdi
				ENDM
				
				MACRO  VDI_CALL handle, opcode, nptsin, nintin
					VDI_CALL_ESC handle, opcode, #0, nptsin, nintin
				ENDM
				
				MACRO  VDI_JUMP_ESC handle, opcode, subop, nptsin, nintin
					VDI_SET_PARAMS handle, opcode, subop, nptsin, nintin
					bra _mt_vdi_end
				ENDM
				
				MACRO  VDI_JUMP handle, opcode, nptsin, nintin
					VDI_JUMP_ESC handle, opcode, #0, nptsin, nintin
				ENDM
				
				MACRO VDIPB extra
					link a6,#-_VdiParBSize-extra
					lea vdi_control(sp),a0
					move.l a0,(sp)
					lea vdi_intin(sp),a0
					move.l a0,v_intin(sp)
					lea vdi_intout(sp),a0
					move.l a0,v_intout(sp)
					lea vdi_ptsout(sp),a0
					move.l a0,v_ptsout(sp)
					lea vdi_ptsin(sp),a0
					move.l a0,v_ptsin(sp)
				ENDM

				MACRO VDI_END
					unlk a6
				ENDM
				
				TEXT

