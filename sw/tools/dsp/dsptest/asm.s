; ------------------------------------------------------------------------
; dsp xbios testbed for faster iteration
; parts of this should normally come with rvbios
; ------------------------------------------------------------------------

DSP_CTRL1		EQU 0x20000010			; uart2:mcr
DSP_CTRL2		EQU	0x20000030			; uart1:mcr

DSP_ICR 		EQU 0x20000400
DSP_CVR 		EQU DSP_ICR+0x04
DSP_ISR 		EQU DSP_ICR+0x08
DSP_IVR 		EQU DSP_ICR+0x0c
DSP_TXH			EQU DSP_ICR+0x14
DSP_TXM			EQU DSP_ICR+0x18
DSP_TXL			EQU DSP_ICR+0x1c
DSP_RXH			EQU DSP_TXH
DSP_RXM			EQU DSP_TXM
DSP_RXL			EQU DSP_TXL


	.text
	.align

	.EXPORT readbuf

readbuf:
	bra.b	.2
.1:	btst.b	#0,DSP_ISR
	beq.b	.1
	move.b	DSP_RXH,(a0)+
	move.b	DSP_RXM,(a0)+
	move.b	DSP_RXL,(a0)+
.2:	dbra.w	d0,.1
	rts

