;-------------------------------------------------------------------------------
; fpu_off
; (c) 2024 Anders Granlund
;
; simulate 68LC060
;
; - disables FPU in pcr register
; - clears _FPU cookie
; - removes FPSP exception handlers
;
;-------------------------------------------------------------------------------
;
; This file is free software  you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation  either version 2, or (at your option)
; any later version.
;
; This file is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY  without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program  if not, write to the Free Software
; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;
;-------------------------------------------------------------------------------

	BSS
gstackb:	ds.l	256
gstackt:	ds.l	4

	TEXT
init:
	move.l	4(sp),a0
	lea		gstackt(pc),sp
	move.l	#0x1100,d0
	add.l	0x0c(a0),d0
	add.l	0x14(a0),d0
	add.l	0x1c(a0),d0

	move.l	d0,-(sp)			; Mshrink
	move.l	a0,-(sp)
	clr.w	-(sp)
	move.w	#0x4a,-(sp)
	trap	#1
	add.l	#12,sp

	pea		main				; Supexec(main)
	move.w	#38,-(sp)
	trap	#14
	addq.l	#6,sp
	
	clr.w	-(sp)				; Pterm(0)
	trap	#1


main:
	move.w	sr,-(sp)
	move.w	#$2700,sr

	; check revision and bail out if already LC/EC
	dc.l	$4e7a0808			; movec pcr,d0
	swap	d0
	cmp.w	#$0431,d0			; lc/ec?
	beq.s	done
	swap	d0
	btst	#$1,d0				; fpu already disabled?
	bne.s	done

	; kill _FPU cookie
	move.l	0x5a0.w,d1			; cookie jar
	beq.s	.3
	move.l	d1,a0
.1:	move.l	(a0)+,d1			; id
	beq.s	.3
	cmp.l	#$5F465055,d1		; '_FPU'
	bne.b	.2
	move.l	#0,(a0)				; clear value
.2:	addq.l	#4,a0				; next cookie
	bra.b	.1

	; restore FPSP
.3:	move.l	#$d8,a0
	bsr		xbra_restore
	move.l	#$d0,a0
	bsr		xbra_restore
	move.l	#$d4,a0
	bsr		xbra_restore
	move.l	#$cc,a0
	bsr		xbra_restore
	move.l	#$c8,a0
	bsr		xbra_restore
	move.l	#$c4,a0
	bsr		xbra_restore
	move.l	#$2c,a0
	bsr		xbra_restore
	move.l	#$dc,a0
	bsr		xbra_restore
	move.l	#$f0,a0
	bsr		xbra_restore
	
	; disable FPU
	fnop
	bset	#$1,d0
	dc.l	$4e7b0808			; movec d0,pcr

done:
	move.w	(sp)+,sr
	rts

xbra_restore:
	move.l	(a0),a1
	cmp.l	#$58425241,-12(a1)	; 'XBRA'
	bne.s	.1
	move.l	-4(a1),(a0)
.1:	rts
